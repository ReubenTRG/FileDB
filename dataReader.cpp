// to compile: g++ -std=c++20 dataReader.cpp src/file_handler.cpp -o dataReader && ./dataReader


#include "./include/file_handler.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void read_database_info(fstream &file) {
    string db_name;
    db_name = readDatabaseName(file, db_name);
    cout << "Database Name: " << db_name << endl;
}

void read_pointers(fstream &file) {
    uint8_t table_count = readPointersCount(file);

    for (uint8_t i = 0; i < table_count; i++) {
        pair<string, int> table_info = readPointers(file, i);
        cout << "\nTable " << (int)(i + 1) << ": " << table_info.first << ", pointer: " << table_info.second << endl;
    }
}

void read_table_info(fstream &file) {
    uint8_t table_count = readPointersCount(file);
    cout << "Number of Tables: " << (int)table_count << endl;
    
    for (uint8_t i = 0; i < table_count; i++) {
        pair<string, int> table_info = readPointers(file, i);
        cout << "\nTable " << (int)(i + 1) << ": " << table_info.first << endl;
        
        vector<pair<string, uint8_t>> columns = readTableSchema(file, table_info.second);
        cout << "Columns (" << columns.size() << "):" << endl;
        
        for (const auto& column : columns) {
            cout << "  - " << column.first << " (" << (int)column.second << ")" << endl;
        }
    }
}

void read_table_data(fstream &file) {
    uint8_t table_count = readPointersCount(file);
    for (uint8_t i = 0; i < table_count; i++) {
        pair<string, int> table_info = readPointers(file, i);
        cout << "\nTable " << (int)(i + 1) << " Data:" << endl;
        
        vector<pair<string, uint8_t>> table_schema = readTableSchema(file, table_info.second);

        int total_schema_bytes = 0;
        for (size_t i = 0; i < table_schema.size(); i++) {
            
            switch (table_schema[i].second) {
                case 0x0: // INT
                    total_schema_bytes += 0x4;
                    break;
                case 0x1: // FLOAT
                    total_schema_bytes += 0x8;
                    break;
                case 0x2: // CHAR
                    total_schema_bytes += 0x1;
                    break;
                case 0x3: // STRING
                    total_schema_bytes += 0xff;
                    break;
                case 0x4: // BOOL
                    total_schema_bytes += 0x1;
                    break;
                default:
                    cerr << "Error: Invalid data type." << endl;
                    return;
            }
        }

        vector<vector<string>> table_data = readTableData(file, table_info.second + total_schema_bytes, table_schema);
        
        for (const auto& row : table_data) {
            for (const auto& value : row) {
                cout << value << "\t";
            }
            cout << endl;
        }
    }
}

int main() {
    fstream file("School.fdb", ios::in | ios::binary);
    if (!file) {
        cerr << "Error opening database file." << endl;
        return 1;
    }
    read_database_info(file);
    read_pointers(file);
    read_table_info(file);
    read_table_data(file);
    file.close();
    return 0;
}
