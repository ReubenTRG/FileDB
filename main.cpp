#include <iostream>
#include <string>
#include <vector>

#include "./include/parser.h"
#include "./include/logic.h"

using namespace std;

// to compile: g++ main.cpp src/parser.cpp src/logic.cpp src/file_handler.cpp src/global.cpp -o main && ./main

int main() {
    
    string sql_input, database = "?";
    string trimmed, command;
    int command_id;
    
    vector<string> columns;
    string table, option, name;
    struct_condition cond;
    vector<struct_column_datatype> column_datatype;
    struct_insert insert_data;
    struct_update update_data;
    struct_delete delete_data;
    string use_data;
    struct_drop drop_data;

    cout << "FileDB" << endl;

    while (1) {
        cout << database + " >> ";
        getline(cin, sql_input);

        if (sql_input.find("exit") == string::npos) {
            trimmed = trimming(sql_input);
            command_id = parse_command(trimmed);
        } else {
            exit(0);
        }

        switch (command_id) {
            case 0:
                // Create Code
                option = parse_CREATE_option(trimmed);
                name = parse_CREATE_name(trimmed);
                column_datatype = parse_CREATE_columns(trimmed);
    
                if (option == "TABLE") {
                    createTable(name, database, column_datatype);
                } else if (option == "DATABASE") {
                    createDatabase(name);
                    database = name;
                }
    
                break;
    
            case 1:
                // Insert Code
                insert_data = parse_INSERT(trimmed);
                
                insertRow(insert_data.table_name, database, insert_data.values);
                
                break;

            case 2:
                // Select Code
                columns = parse_SELECT_column_list(trimmed);
                table = parse_SELECT_table(trimmed);
                cond = parse_SELECT_condition(trimmed);

                break;
    
            case 3:
                // Update Code
                update_data = parse_UPDATE(trimmed);
    
                if (update_data.has_condition) {

                } else {

                }
                break;
            
            case 4:
                // Delete Code
                delete_data = parse_DELETE(trimmed);
    
                if (delete_data.has_condition) {
                    deleteRowsCondition(database, table, delete_data.condition_column, delete_data.condition_operator, delete_data.condition_value);
                } else {
                    deleteRows(database, table);
                }
                break;
            
            case 5:
                // Use Code
                use_data = parse_USE(trimmed);

                // cout << "Database: " << use_data << endl;

                if (useDatabase(use_data)) {
                    database = use_data;
                } else {
                    database = "?";
                    cout << "Database not found" << endl;
                }

                break;
    
            case 6:
                // Drop Code
                drop_data = parse_DROP(trimmed);

                if (drop_data.is_database) {
                    dropDatabase(drop_data.name);
                    database = "?";
                } else {
                    dropTable(database, drop_data.name);
                }

                break;
            
            default:
                cout << "Invalid Command";
                break;
        }
    }
    

    return 0;
}
