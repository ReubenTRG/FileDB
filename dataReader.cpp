#include "./include/file_handler.h"
#include "./include/global.h"

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
        
        vector<struct_col_dtype> columns = readTableSchema(file, table_info.second);
        cout << "Columns (" << columns.size() << "):" << endl;
        
        for (const auto& column : columns) {
            cout << "  - " << column.col_name << " (" << (int)column.id << ")" << endl;
        }
    }
}

void read_table_data(fstream &file) {
    uint8_t table_count = readPointersCount(file);
    for (uint8_t i = 0; i < table_count; i++) {
        pair<string, int> table_info = readPointers(file, i);
        cout << "\nTable " << (int)(i + 1) << " Data:" << endl;
        
        vector<struct_col_dtype> table_schema = readTableSchema(file, table_info.second);
        int total_schema_bytes = totalSchemaBytes(table_schema);
        
        vector<vector<string>> table_data = readTableData(file, table_info.second + 1 + total_schema_bytes, table_schema);
        
        for (const auto& row : table_data) {
            for (const auto& value : row) {
                cout << value << "\t";
            }
            cout << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    string filename = (argc > 1) ? (string(argv[1]) + ".fdb") : "School.fdb";
    
    fstream file(filename, ios::in | ios::binary);
    if (!file) {
        cerr << "Error opening database file: " << filename << endl;
        return 1;
    }
    
    read_database_info(file);
    read_pointers(file);
    read_table_info(file);
    read_table_data(file);
    
    file.close();
    return 0;
}
