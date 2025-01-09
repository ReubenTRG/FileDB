#include <iostream>
#include <string>
#include <vector>
#include "HEADERS/reader.h"

using namespace std;

// to compile: g++ test.cpp HEADERS/reader.cpp -o test

int main() {
    // string sql = "   SELECT name, dept, age FROM Class WHERE age >= 12;   ";
    // string sql = "CREATE TABLE Students (id INT, name VARCHAR(50), age INT);";
    // string sql = "CREATE DATABASE School;";
    // string sql = "SELECT name, age FROM Students WHERE age >= 18;";
    // string sql = "UPDATE Students SET age = 21 WHERE name = 'John Doe';";
    // string sql = "DELETE FROM Students WHERE age < 18;";
    // string sql = "CREATE TABLE Students (id INT, name VARCHAR, age INT);";
    // string sql = "INSERT INTO Students VALUES (101, 'John Doe', 22);";
    // string sql = "UPDATE Students SET age = 20 WHERE name == 'John Doe';";
    // string sql = "DELETE FROM Students WHERE age < 18;";
    // string sql = "USE stud;";


    string trimmed = trimming(sql);
    int command_id = parse_command(trimmed);

    vector<string> columns;
    string table, option, name;
    struct_condition cond;
    vector<struct_column_datatype> column_datatype;
    struct_insert insert_data;
    struct_update update_data;
    struct_delete delete_data;
    string use_data;

    cout << "Original: " << sql << endl;
    cout << "Trimmed: " << trimmed << endl;
    cout << "Command: " << commands_list[command_id] << endl;

    switch (command_id) {
        case 0:
            // Create Code
            option = parse_CREATE_option(trimmed);
            name = parse_CREATE_name(trimmed);
            column_datatype = parse_CREATE_columns(trimmed);

            cout << "Option: " << option << endl;
            cout << "Name: " << name << endl;
            cout << "Columns: ";
            for (const struct_column_datatype& col : column_datatype) {
                cout << col.name << " ~> " << col.type << " ";
            }
            cout << endl;
            break;

        case 1:
            // Insert Code
            insert_data = parse_INSERT(sql);

            cout << "Table Name: " << insert_data.table_name << endl;
            cout << "Values: ";
            for (const string& value : insert_data.values) {
                cout << value << " ~ ";
            }
            cout << endl;
            break;
        
        case 2:
            // Select Code
            columns = parse_SELECT_column_list(trimmed);
            table = parse_SELECT_table(trimmed);
            cond = parse_SELECT_condition(trimmed);

            cout << "Columns: ";
            for (const string& col : columns) {
                cout << col << " ";
            }
            cout << endl;
            cout << "Table: " << table << endl;
            cout << "Condition Details(Column Condition Value): " << cond.column << " ~ " << cond.oper << " " << cond.value << endl;
            break;

        case 3:
            // Update Code
            update_data = parse_UPDATE(sql);

            cout << "Table Name: " << update_data.table_name << endl;
            cout << "Column to Update: " << update_data.column_name << endl;
            cout << "New Value: " << update_data.value << endl;

            if (update_data.has_condition) {
                cout << "Condition Column: " << update_data.condition_column << endl;
                cout << "Condition Operator: " << update_data.condition_operator << endl;
                cout << "Condition Value: " << update_data.condition_value << endl;
            } else {
                cout << "No Condition" << endl;
            }
            break;
        
        case 4:
            // Delete Code
            delete_data = parse_DELETE(sql);

            cout << "Table Name: " << delete_data.table_name << endl;

            if (delete_data.has_condition) {
                cout << "Condition Column: " << delete_data.condition_column << endl;
                cout << "Condition Operator: " << delete_data.condition_operator << endl;
                cout << "Condition Value: " << delete_data.condition_value << endl;
            } else {
                cout << "No Condition" << endl;
            }
            break;
        
        case 5:
            // Use Code
            use_data = parse_USE(sql);

            cout << "Database: " << use_data << endl;
            break;
        
        default:
            cout << "Invalid Command";
            break;
    }

    return 0;
}
