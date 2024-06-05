#include <iostream>
#include <string>
#include <cstdlib>
#include <../include/sqlite3.h>

const char* gamePath = "";

int main() {
    sqlite3* database;
    int exit = 0;
    exit = sqlite3_open("example_database", &database);

    if (exit) {
        std::cerr << "Error opening database" << sqlite3_errmsg(database) << std::endl;
        return -1;
    } 
    else {
        std::cout << "Opened database successfully!" << std::endl;
    }
    sqlite3_close(database);
    
    
    //system(gamePath);
    return 0;
}