#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <thread>
#include <winsock2.h>

#include "./include/parser.h"
#include "./include/logic.h"

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

using namespace std;

void handle_client(SOCKET clientSocket) {
    char buffer[1024];

    vector<string> columns;
    string table, option, name;
    struct_condition cond;
    vector<struct_column_datatype> column_datatype;
    struct_select select_data;
    struct_insert insert_data;
    struct_update update_data;
    struct_delete delete_data;
    string use_data;
    struct_drop drop_data;
    
    string database = "?", response = "";

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        string sql_input(buffer);

        cout << "from GUI: " << sql_input << endl;

        if (sql_input == "exit") break;

        string trimmed = trimming(sql_input);
        int command_id = parse_command(trimmed);

        switch (command_id) {
            case 0:
                // Create Code
                option = parse_CREATE_option(trimmed);
                name = parse_CREATE_name(trimmed);
                column_datatype = parse_CREATE_columns(trimmed);
    
                if (option == "TABLE") {
                    createTable(name, database, column_datatype);
                    response = "Successfully created table\n";
                } else if (option == "DATABASE") {
                    createDatabase(name);
                    response = "Successfully created database";
                    database = name;
                }
    
                break;

            case 1:
                // Insert Code
                insert_data = parse_INSERT(trimmed);
                
                insertRow(insert_data.table_name, database, insert_data.values);
                response = "Successfully inserted the data.\n";
                
                break;

            case 2:
                // Select Code
                select_data = parse_SELECT(trimmed);

                if (select_data.has_condition && select_data.columns[0] == "*") {
                    response = selectTableAllCond(database, select_data.table_name, select_data.condition_column, select_data.condition_operator, select_data.condition_value);
                } else if (select_data.has_condition) {
                    response = selectTableCond(database, select_data.table_name, select_data.columns, select_data.condition_column, select_data.condition_operator, select_data.condition_value);
                } else if (select_data.columns[0] == "*") {
                    response = selectTableAll(database, select_data.table_name);
                } else {
                    response = selectTable(database, select_data.table_name, select_data.columns);
                }

                break;

            case 3:
                // Update Code
                update_data = parse_UPDATE(trimmed);

                if (update_data.has_condition) {
                    string result = updateTableCond(
                        database, 
                        update_data.table_name, 
                        update_data.column_name, 
                        update_data.value, 
                        update_data.condition_column, 
                        update_data.condition_operator, 
                        update_data.condition_value
                    );
                    response = "Successful updation.\n";
                    cout << result << endl;
                } else {
                    string result = updateTable(
                        database, 
                        update_data.table_name, 
                        update_data.column_name, 
                        update_data.value
                    );
                    response = "Successful updation.\n";
                    cout << result << endl;
                }
                break;

            case 4:
                // Delete Code
                delete_data = parse_DELETE(trimmed);
    
                if (delete_data.has_condition) {
                    // deleteRowsCondition(database, delete_data.table_name, delete_data.condition_column, delete_data.condition_operator, delete_data.condition_value);
                } else {
                    deleteRows(database, delete_data.table_name);
                }
                response = "Successful deletion.\n";
                break;

            case 5:
                // Use Code
                use_data = parse_USE(trimmed);

                if (useDatabase(use_data)) {
                    database = use_data;
                    response = "None";
                } else {
                    database = "?";
                    response = "Database not found.\n";
                    cout << "Database not found" << endl;
                }

                break;

            case 6:
                // Drop Code
                drop_data = parse_DROP(trimmed);

                if (drop_data.is_database) {
                    dropDatabase(drop_data.name);
                    database = "?";
                    response = "Successfully dropped database.\n";
                } else {
                    response = "Successfully dropped table.\n";
                    dropTable(database, drop_data.name);
                }

                break;

            default:
                response = "Invalid Command.\n";
                break;
        }

        send(clientSocket, database.c_str(), database.size(), 0);
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
}

void start_gui_mode() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    cout << "C++ Server (GUI Mode) started. Waiting for connections...\n";

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    handle_client(clientSocket);

    closesocket(serverSocket);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--gui") == 0) {
        start_gui_mode();
        return 0;
    }

    // Terminal Mode
    string sql_input, database = "?";

    string trimmed, command;
    int command_id;
    string response;

    vector<string> columns;
    string table, option, name;
    struct_condition cond;
    vector<struct_column_datatype> column_datatype;
    struct_select select_data;
    struct_insert insert_data;
    struct_update update_data;
    struct_delete delete_data;
    string use_data;
    struct_drop drop_data;

    cout << "FileDB (Terminal Mode)\n";

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
                select_data = parse_SELECT(trimmed);

                if (select_data.has_condition && select_data.columns[0] == "*") {
                    response = selectTableAllCond(database, select_data.table_name, select_data.condition_column, select_data.condition_operator, select_data.condition_value);
                } else if (select_data.has_condition) {
                    response = selectTableCond(database, select_data.table_name, select_data.columns, select_data.condition_column, select_data.condition_operator, select_data.condition_value);
                } else if (select_data.columns[0] == "*") {
                    response = selectTableAll(database, select_data.table_name);
                } else {
                    response = selectTable(database, select_data.table_name, select_data.columns);
                }

                cout << response;

                break;
    
            case 3:
                // Update Code
                update_data = parse_UPDATE(trimmed);

                if (update_data.has_condition) {
                    string result = updateTableCond(
                        database, 
                        update_data.table_name, 
                        update_data.column_name, 
                        update_data.value, 
                        update_data.condition_column, 
                        update_data.condition_operator, 
                        update_data.condition_value
                    );
                    cout << result << endl;
                } else {
                    string result = updateTable(
                        database, 
                        update_data.table_name, 
                        update_data.column_name, 
                        update_data.value
                    );
                    cout << result << endl;
                }
                break;
            
            case 4:
                // Delete Code
                delete_data = parse_DELETE(trimmed);
    
                if (delete_data.has_condition) {
                    // deleteRowsCondition(database, delete_data.table_name, delete_data.condition_column, delete_data.condition_operator, delete_data.condition_value);
                } else {
                    deleteRows(database, delete_data.table_name);
                }
                break;
            
            case 5:
                // Use Code
                use_data = parse_USE(trimmed);

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
