#include "../include/parser.h"
#include "../include/logic.h"
#include "../include/file_handler.h"

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <fstream>

#define BIT8_MAX 256
#define POINTER_SIZE 4

const int name_length = 256;



using namespace std;

void createDatabase(const string& name) {
    fstream file(name + ".fdb", ios::in | ios::out | ios::binary | ios::trunc);
    if (!file) {
        cerr << "Error: Could not create database file." << endl;
        return;
    }

    createDatabaseName(file, name);

    createPointersCount(file);

    createPointers(file);

    file.close();
}

void createTable(const string& tabel_name, const string& database_name,  vector<struct_column_datatype> columns) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return;
    }

    int found = findTableIndex(file, tabel_name);
    if (found != -1) {
		cerr << "Error: Table \"" << tabel_name << "\" already exists." << endl;
		return;
	}

	uint8_t table_count = readPointersCount(file);
	if (table_count >= BIT8_MAX) {
		cerr << "Error: Maximum number of tables reached." << endl;
		return;
	}
    
    file.seekp(0, ios::end);
    int end_of_file_pos  = file.tellp();
    insertTablePointer(file, tabel_name, end_of_file_pos);

	updatePointersCount(file, table_count + 1);

    int total_schema_bytes = 0;
    vector<pair<string, uint8_t>> columns_schema;
    for (auto col: columns) {
        if (col.type == "INT") {
            columns_schema.push_back(make_pair(col.name, 0x0));
            total_schema_bytes += 0x4;
        } else if (col.type == "FLOAT") {
            columns_schema.push_back(make_pair(col.name, 0x1));
            total_schema_bytes += 0x8;
        } else if (col.type == "STRING") {
            columns_schema.push_back(make_pair(col.name, 0x2));
            total_schema_bytes += 0xff;
        } else if (col.type == "CHAR") {
            columns_schema.push_back(make_pair(col.name, 0x3));
            total_schema_bytes += 0x1;
        } else if (col.type == "BOOL") {
            columns_schema.push_back(make_pair(col.name, 0x4));
            total_schema_bytes += 0x1;
        } else {
            cerr << "Error: Invalid data type." << endl;
        }
    }
    
    file.seekp(0, ios::end);
    end_of_file_pos  = file.tellp();
    createTableSchema(file, end_of_file_pos, columns_schema);

    file.seekp(0, ios::end);
    end_of_file_pos  = file.tellp();
    createTableData(file, end_of_file_pos, total_schema_bytes);

	file.close();
}

void insertRow(const string& table_name, const string& database_name, vector<string> values) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return;
    }

    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
		cerr << "Error: Table \"" << table_name << "\" doesnt exists." << endl;
		return;
	}
    
    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    vector<pair<string, uint8_t>> table_schema = readTableSchema(file, table_schema_ptr);

    int total_schema_bytes = 0;
    vector<struct_name_id_data> data;
    for (size_t i = 0; i < table_schema.size(); i++) {
        struct_name_id_data temp;
        temp.name = table_schema[i].first;
        temp.id = table_schema[i].second;
        
        switch (table_schema[i].second) {
            case 0x0: // INT
                temp.int_data = stoi(values[i]);
                total_schema_bytes += 0x4;
                break;
            case 0x1: // FLOAT
                temp.float_data = stof(values[i]);
                total_schema_bytes += 0x8;
                break;
            case 0x2: // CHAR
                temp.char_data = values[i][0];
                total_schema_bytes += 0x1;
                break;
            case 0x3: // STRING
                temp.string_data = values[i];
                total_schema_bytes += 0xff;
                break;
            case 0x4: // BOOL
                temp.bool_data = (values[i] == "true" || values[i] == "1");
                total_schema_bytes += 0x1;
                break;
            default:
                cerr << "Error: Invalid data type." << endl;
                return;
        }
        data.push_back(temp);
    }

    insertTableData(file, table_schema_ptr + total_schema_bytes + 1, data, total_schema_bytes);

    file.close();
}