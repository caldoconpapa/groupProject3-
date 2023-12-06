import requests
import json
import os

def get_access_token(client_id, client_secret):
    url = 'https://id.twitch.tv/oauth2/token'  # IGDB uses Twitch for authentication
    payload = {
        'client_id': client_id,
        'client_secret': client_secret,
        'grant_type': 'client_credentials'
    }
    response = requests.post(url, data=payload)
    if response.status_code == 200:
        return response.json()['access_token']
    else:
        raise Exception(f"Error obtaining access token: {response.text}")

def fetch_data(access_token, limit ,offset ):
    url = 'https://api.igdb.com/v4/games'  # Endpoint for games data
    headers = {
        'Client-ID': client_id,
        'Authorization': f'Bearer {access_token}',
        'Accept': 'application/json'
    }
    # Updated body to fetch specific fields
    body = f'fields name,age_ratings.rating, platforms.name, release_dates.human,genres.slug, aggregated_rating; limit {limit}; offset{offset};'
    response = requests.post(url, headers=headers, data=body)
    if response.status_code == 200:
        return response.json()
    else:
        raise Exception(f"Error fetching data: {response.text}")

client_id = '5ajlhqvmkc6ilbgodj1eqchr1imi35'
client_secret = 'xav3xv303xz2sr1cfzfxiw5fsm8x8s'
filename = 'igdb_data.json'
total_records_needed = 10000
batch_size = 100  # Adjust based on the API's limits

try:
    token = get_access_token(client_id, client_secret)
    offset = 0

    while offset < total_records_needed:
        batch_data = fetch_data(token, batch_size, offset)
    
        for game in batch_data:
            # Process genres to keep only the first one
            if 'genres' in game and game['genres']:
                first_genre = game['genres'][0]['slug']  # Assume 'slug' field is always present
                game['genres'] = first_genre  # Replace the list of genres with the first genre
            else:
                game['genres'] = 'N/A'  # Assign 'N/A' if no genres are available

            # Extract only the year from release_dates.human
            if 'release_dates' in game and game['release_dates']:
                first_release_date = game['release_dates'][0]['human']
                year = first_release_date.split(', ')[-1]  # Split and take the last part for the year
                game['release_dates'] = year
            else:
                game['release_dates'] = 'N/A'

            game['aggregated_rating'] = game.get('aggregated_rating', 'N/A')  # Assign 'N/A' if missing

            # Include only ratings from 6 to 12 (focus on ESRB)
            if 'age_ratings' in game:
                game['age_ratings'] = [rating for rating in game['age_ratings'] if 6 <= rating['rating'] <= 12]

            # Remove 'id' field from age_ratings
            if 'age_ratings' in game:
                for rating in game['age_ratings']:
                    if 'id' in rating:
                        del rating['id']

            # Remove 'id' field from platforms
            if 'platforms' in game:
                for platform in game['platforms']:
                    if 'id' in platform:
                        del platform['id']
        
            # Remove all 'id' fields
            if 'id' in game:
                del game['id']


        # Read existing data from file and append new data
            if os.path.exists(filename) and os.path.getsize(filename) > 0:
                with open(filename, 'r') as file:
                    existing_data = json.load(file)
            else:
                existing_data = []

            existing_data.extend(batch_data)

            # Write combined data back to file
            with open(filename, 'w') as file:
                json.dump(existing_data, file, indent=4)

            offset += batch_size
            if len(batch_data) < batch_size:
                break  # Break if the last batch returned less data than the batch size

except Exception as e:
    print(e)