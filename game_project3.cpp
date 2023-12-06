#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
using std::cout;
using std::cin;
using std::string;
using std::vector;


class Game{
    private:
    string title;
    string genre;
    string year;
    string console;

    public:
    Game(string _title, string _genre, string _year, string _console);
    string GetTitle();
    string GetGenre();
    string GetYear();
    string GetConsole();
};

Game::Game(string _title, string _genre, string _year, string _console){
    title = _title;
    genre = _genre;
    year = _year;
    console = _console;
}

string Game::GetTitle(){
    return title;
}

string Game::GetGenre(){
    return genre;
}

string Game::GetYear(){
    return year;
}

string Game::GetConsole(){
    return console;
}

class GameHashMap{
    int capacity;
    int size;
    bool sep_chaining;
    vector<vector<Game*> > gamehash;
    vector<Game*> gamehash_probing;
    public:
    GameHashMap(int _capacity, bool _sep_chaining);
    int Hash(string genre, string console);
    void Insert(Game* game);
    vector<Game*> Search(string genre, string console);
};

GameHashMap::GameHashMap(int _capacity, bool _sep_chaining){
    sep_chaining = _sep_chaining;
    capacity = _capacity;

    if (sep_chaining){
    size = 0;
    for (int i = 0; i < capacity; i++){
        vector<Game*> empty;
        gamehash.push_back(empty);
    }
    }

    else{
    vector<Game*> empty(capacity, nullptr);
    gamehash_probing = empty;
    }
}

int GameHashMap::Hash(string genre, string console){
    string combined_string = genre + console;

    int hash = 0;
    for (char ch : combined_string) {
        hash = (hash * 31) + ch;  // Using a prime number multiplier
    }

    // Ensure the hash code is non-negative and within the vector size
    int positiveHash = (hash % capacity + capacity) % capacity;

    return positiveHash;
}

void GameHashMap::Insert(Game* game){
    int hash = Hash(game->GetGenre(), game->GetConsole());

    if (sep_chaining){
    // if separate chaining is being used
    for (int i = 0; i < gamehash[hash].size(); i++){
        if (gamehash[hash][i]->GetTitle() == game->GetTitle()){
            return;
        }
    }
    gamehash[hash].push_back(game);
    }

    else{
    // if linear probing is being used
    bool hit = false;
    for (int i = hash; i < capacity; i++){
        if (gamehash_probing[i] != nullptr){
        if (gamehash_probing[i]->GetTitle() == game->GetTitle()){
            return;
        }
        }
        if (gamehash_probing[i] == nullptr){
            gamehash_probing[i] = game;
            hit = true;
            break;
        }
    }
    if (hit == false){
        for (int i = 0; i < hash; i++){
        if (gamehash_probing[i] == nullptr){
            gamehash_probing[i] = game;
            hit = true;
            break;
        }
        }
    }
    }
}

vector<Game*> GameHashMap::Search(string genre, string console){
    int hash = Hash(genre, console);
    vector<Game*> games_rec;
    if (sep_chaining){
    // if separate chaining is being used
    if (gamehash[hash].empty()){
        return gamehash[hash];
    }
    else{
        for (int i = 0; i < gamehash[hash].size(); i++){
            if (gamehash[hash][i]->GetGenre() == genre && gamehash[hash][i]->GetConsole() == console){
                games_rec.push_back(gamehash[hash][i]);
                if (games_rec.size() > 5){
                    break;
                }
            }
        }
    }
    }

    else{
    // if linear probing is being used
    for (int i = hash; i < capacity; i++){
        if (gamehash_probing[i] != nullptr){
        if (gamehash_probing[i]->GetGenre() == genre && gamehash_probing[i]->GetConsole() == console){
            games_rec.push_back(gamehash_probing[i]);
            if (games_rec.size() > 5){
                break;
            }
        }
        }
    }
    if (games_rec.size() < 3){
        for (int i = 0; i < hash; i++){
        if (gamehash_probing[i] != nullptr){
        if (gamehash_probing[i]->GetGenre() == genre && gamehash_probing[i]->GetConsole() == console){
            games_rec.push_back(gamehash_probing[i]);
            if (games_rec.size() > 5){
                break;
            }
        }
        }
    }
    }
    }
    return games_rec;
}

void CSVDataToGameHashMap(const std::string& filename, GameHashMap& gameMap) {
    //read from csv file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "File did not open as expected" << std::endl;
        return;
    }

    Game* newgame;
    string id, ar_id, age_rating, gid, genre1, genre2, name, p_id, console, r_id, rh_id, year, r_date, agrate;
    string line;
    getline(file, line);
    int count = 0;
    while(getline(file, line)){
        std::stringstream ss(line);
        getline(ss, id, ',');
        getline(ss, ar_id, ',');
        getline(ss, age_rating, ',');
        age_rating = age_rating.substr(1, age_rating.length() - 2);
        getline(ss, gid, ',');
        getline(ss, genre1, ',');
        genre1 = genre1.substr(1, genre1.length() - 2);
        getline(ss, genre2, ',');
        getline(ss, name, ',');
        name = name.substr(1, name.length() - 2);
        getline(ss, p_id, ',');
        getline(ss, console, ',');
        console = console.substr(1, console.length() - 2);
        getline(ss, r_id, ',');
        getline(ss, rh_id, ',');
        getline(ss, year, ',');
        year = year.substr(1, year.length() - 2);
        getline(ss, r_date, ',');
        getline(ss, agrate, ',');

        if ((genre1 != "" && genre1 != "N/A") &&
        (name != "" && name != "N/A") &&
        (console != "" && console != "N/A") &&
        (year != "" && year != "N/A")){
            Game* new_game = new Game(name, genre1, year, console);
            newgame = new_game;
            gameMap.Insert(newgame);
            count++;
        }
    }
    file.close();
}

string genre_selection(string choice){
    if (choice == "1"){
        return "fighting";
    }
    else if (choice == "2"){
        return "adventure";
    }
    else if (choice == "3"){
        return "arcade";
    }
    else if (choice == "4"){
        return "puzzle";
    }
    else if (choice == "5"){
        return "simulator";
    }
    else if (choice == "6"){
        return "sport";
    }
    else if (choice == "7"){
        return "role-playing-rpg";
    }
    else if (choice == "8"){
        return "strategy";
    }
    else if (choice == "9"){
        return "shooter";
    }
    return "";
}

string console_selection(string choice){
    if (choice == "1"){
        return "Nintendo Switch";
    }
    else if (choice == "2"){
        return "Xbox One";
    }
    else if (choice == "3"){
        return "PC (Microsoft Windows)";
    }
    else if (choice == "4"){
        return "PlayStation 4";
    }
    else if (choice == "5"){
        return "PlayStation 5";
    }
    else if (choice == "6"){
        return "Wii";
    }
    return "";
}

void DisplayGames(vector<Game*> results){
    //show results
    if (results.empty()){
        cout << "No matching games! Sorry.\n";
        return;
    }
    for (int i = 0; i < results.size(); i++){
        cout << i + 1 << ". Title: " << results[i]->GetTitle() << '\n';
        cout << "    Released: " << results[i]->GetYear() << '\n' << '\n';
    }
}

int main(){
    bool choice;
    string genre;
    string console;
    bool keepon = true;
    while (keepon){
    cout << "\nWelcome to the Video Game Recommender! We can help you find your next favorite game\n\n";
    cout << "What collision resolution strategy would you prefer?\n\n";
    cout << "Type 1 for separate chaining, type 0 for linear probing\n";
    cin >> choice;
    GameHashMap gamemap(1205, choice);
    CSVDataToGameHashMap("igdb_data_10k.csv", gamemap);
    cout << "What is your favorite type of game? Enter a number corresponding to a category\n\n";
    cout << "1: Fighting          2: Adventure         3: Arcade\n";
    cout << "4: Puzzle            5: Simulator         6: Sports\n";
    cout << "7: RPG               8: Strategy          9: Shooter\n";

    cin >> genre;
    string genre_select = genre_selection(genre);

    cout << "\nWhat is your favorite console to game on? Enter a number corresponding to a console\n\n";
    cout << "1: Nintendo Switch   2: Xbox One          3: PC\n";
    cout << "4: PlaySation 4      5: Playstation 5     6: Nintendo Wii\n";

    cin >> console;
    string console_select = console_selection(console);

    cout << "\nHere are your results!\n\n";

    vector<Game*> results = gamemap.Search(genre_select, console_select);
    DisplayGames(results);
    string cont_use;
    cout << "Try again? Enter Y if yes, enter anything else if no \n";
    cin >> cont_use;
    if (cont_use == "Y" || cont_use == "y"){
        continue;
    }
    else {
        keepon = false;
    }
    }

    return 0;
}
