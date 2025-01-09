#include <iostream>
#include <string>
#include <vector>
#include "HEADERS/reader.h"

using namespace std;

// to compile: g++ main.cpp HEADERS/reader.cpp -o main

int main() {
    
    string sql_input, database = "?";
    string trimmed, command, table;
    vector<string> columns;
    struct_condition cond;
    cout << "FileDB" << endl;

    while (1) {
        cout << database + " >>";
        getline(cin, sql_input);

        if (sql_input.find("exit") == string::npos) {
            trimmed = trimming(sql_input);
            command = parse_command(trimmed);
            columns = parse_SELECT_column_list(trimmed);
            table = parse_SELECT_table(trimmed);
            cond = parse_SELECT_condition(trimmed);
        } else {
            exit(0);
        }
    }
    

    return 0;
}
