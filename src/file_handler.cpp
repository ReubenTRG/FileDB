#include "../include/file_handler.h"
#include "../include/logic.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <utility>
#include <cstdint>

#define BIT8_MAX 256
#define POINTER_SIZE 4

using namespace std;

// TODO: making CRUD operations for each database file management operations

/*
TODO: implement the following functions

* CRUD for Pointers
	* Find table name - DONE
	* Insert table
	* Update table name
	* Update table pointer
* CRUD for Tables Schema
* CRUD for Tables Data
* CRUD for File

*/

// write the database name in the file 
void createDatabaseName(fstream &file, const string& name) {
	file.seekg(0, ios::beg);
	uint8_t databaseName[BIT8_MAX] = {0};
	strncpy(reinterpret_cast<char *>(databaseName), name.c_str(), sizeof(databaseName));
	file.write(reinterpret_cast<char *>(databaseName), sizeof(databaseName));
}

// // read the database name from the file
string readDatabaseName(fstream &file, string& name) {
	file.seekg(0, ios::beg);
	uint8_t databaseName[BIT8_MAX] = {0};
	file.read(reinterpret_cast<char *>(databaseName), sizeof(databaseName));
	return reinterpret_cast<char *>(databaseName);
}

// // update the database name in the file
// string updateDatabaseName(fstream &file, string& name) {
// 	file.seekg(0, ios::beg);
// 	uint8_t oldDatabaseName[BIT8_MAX] = {0};
// 	file.read(reinterpret_cast<char *>(oldDatabaseName), sizeof(oldDatabaseName));

// 	uint8_t newDatabaseName[BIT8_MAX] = {0};
// 	strncpy(reinterpret_cast<char *>(newDatabaseName), name.c_str(), sizeof(newDatabaseName));
// 	file.write(reinterpret_cast<char *>(newDatabaseName), sizeof(newDatabaseName));

// 	return reinterpret_cast<char *>(oldDatabaseName);
// }

// // create pointers in the file
void createPointersCount(fstream &file) {
	file.seekg(BIT8_MAX + 1, ios::beg);
	uint8_t table_count = 0;
	file.write(reinterpret_cast<char *>(&table_count), sizeof(table_count));
}

uint8_t readPointersCount(fstream &file) {
	file.seekg(BIT8_MAX + 1, ios::beg);
	uint8_t table_count;
	file.read(reinterpret_cast<char *>(&table_count), sizeof(table_count));
	return table_count;
}

uint8_t updatePointersCount(fstream &file, uint8_t table_count) {
	file.seekg(BIT8_MAX + 1, ios::beg);
	file.write(reinterpret_cast<char *>(&table_count), sizeof(table_count));
	return table_count;
}

void createPointers(fstream &file) {
	file.seekg(BIT8_MAX + 2, ios::beg);
	uint8_t table_name[BIT8_MAX] = {0};
	uint8_t table_pointer[POINTER_SIZE] = {0};
	for (int i = 0; i < BIT8_MAX; i++) {
		file.write(reinterpret_cast<char *>(&table_name), sizeof(table_name));
		file.write(reinterpret_cast<char *>(&table_pointer), sizeof(table_pointer));
	}
}

pair<string, int> readPointers(fstream &file, uint8_t table_count) {
	file.seekg(BIT8_MAX + 2 + (BIT8_MAX + POINTER_SIZE) * table_count, ios::beg);

	uint8_t table_name[BIT8_MAX] = {0};
	file.read(reinterpret_cast<char *>(&table_name), sizeof(table_name));
	
	int table_pointer = {0};
	file.read(reinterpret_cast<char *>(&table_pointer), sizeof(table_pointer));
	
	return make_pair(reinterpret_cast<char *>(table_name), table_pointer);
}

// uint32_t findTablePointers(fstream &file, const string& table_find_name) {
// 	uint8_t table_count = readPointersCount(file);
// 	uint32_t table_find_pointer = 0;
// 	bool found = false;
// 	for (uint8_t i = 0; i < table_count; i++) {
// 		auto [table_name, table_pointer] = readPointers(file, i);
// 		if (table_name == table_find_name) {
// 			table_find_pointer = table_pointer;
// 			break;
// 		}
// 	}
// 	return (found == true)? table_find_pointer: 0;
// }

int findTableIndex(fstream &file, const string& table_name) {
	uint8_t table_count = readPointersCount(file);
	for (int i = 0; i < table_count; i++) {
		auto [existing_name, _] = readPointers(file, i);
		if (existing_name == table_name) {
			return i;
		}
	}
	return -1; // Return an invalid index if not found
}

void insertTablePointer(fstream &file, const string& table_name, uint32_t table_ptr) {
	uint8_t table_count = readPointersCount(file);

	file.seekp(BIT8_MAX + 2 + (BIT8_MAX + POINTER_SIZE) * table_count, ios::beg);
	uint8_t table_name_arr[BIT8_MAX] = {0};
	strncpy(reinterpret_cast<char*>(table_name_arr), table_name.c_str(), sizeof(table_name_arr));
	file.write(reinterpret_cast<const char*>(table_name_arr), BIT8_MAX);
	file.write(reinterpret_cast<const char*>(&table_ptr), POINTER_SIZE);
	
	updatePointersCount(file, table_count + 1);
}

// void updateTablePointer(fstream &file, const string& table_name, uint32_t new_ptr) {
//     uint8_t index = findTableIndex(file, table_name);
//     if (index == 255) {
//         cerr << "Error: Table not found." << endl;
//         return;
//     }
//     file.seekp(BIT8_MAX + 2 + (BIT8_MAX + POINTER_SIZE) * index + BIT8_MAX, ios::beg);
//     file.write(reinterpret_cast<const char*>(&new_ptr), POINTER_SIZE);
// }

// void updateTableName(fstream &file, const string& old_name, const string& new_name) {
//     uint8_t index = findTableIndex(file, old_name);
//     if (index == 255) {
//         cerr << "Error: Table not found." << endl;
//         return;
//     }
//     file.seekp(BIT8_MAX + 2 + (BIT8_MAX + POINTER_SIZE) * index, ios::beg);
//     uint8_t new_name_arr[BIT8_MAX] = {0};
//     strncpy(reinterpret_cast<char*>(new_name_arr), new_name.c_str(), sizeof(new_name_arr));
//     file.write(reinterpret_cast<const char*>(new_name_arr), BIT8_MAX);
// }

// void deleteTablePointer(fstream &file, const string& table_name) {
//     uint8_t table_count = readPointersCount(file);
//     uint8_t index = findTableIndex(file, table_name);
//     if (index == 255) {
//         cerr << "Error: Table not found." << endl;
//         return;
//     }
//     if (index != table_count - 1) {
//         auto [last_name, last_ptr] = readPointers(file, table_count - 1);
//         file.seekp(BIT8_MAX + 2 + (BIT8_MAX + POINTER_SIZE) * index, ios::beg);
//         uint8_t last_name_arr[BIT8_MAX] = {0};
//         strncpy(reinterpret_cast<char*>(last_name_arr), last_name.c_str(), sizeof(last_name_arr));
//         file.write(reinterpret_cast<const char*>(last_name_arr), BIT8_MAX);
//         file.write(reinterpret_cast<const char*>(&last_ptr), POINTER_SIZE);
//     }
//     updatePointersCount(file, table_count - 1);
// }

void createTableSchema(fstream &file, int table_ptr, vector<pair<string, uint8_t>> &columns) {
    file.seekp(table_ptr, ios::beg);
    uint8_t num_columns = columns.size();
    file.write(reinterpret_cast<const char*>(&num_columns), sizeof(num_columns));
    for (const auto& [col_name, dtype] : columns) {
        uint8_t col_name_arr[256] = {0};
        strncpy(reinterpret_cast<char*>(col_name_arr), col_name.c_str(), sizeof(col_name_arr));
        file.write(reinterpret_cast<const char*>(col_name_arr), 256);
        file.write(reinterpret_cast<const char*>(&dtype), sizeof(dtype));
    }
}

uint8_t readTableSchemaSize(fstream &file, uint32_t table_ptr) {
    file.seekg(table_ptr, ios::beg);
    uint8_t num_columns;
    file.read(reinterpret_cast<char*>(&num_columns), sizeof(num_columns));
    return num_columns;
}

vector<pair<string, uint8_t>> readTableSchema(fstream &file, uint32_t table_ptr) {
    file.seekg(table_ptr, ios::beg);
    uint8_t num_columns;
    file.read(reinterpret_cast<char*>(&num_columns), sizeof(num_columns));
    vector<pair<string, uint8_t>> columns;
    for (uint8_t i = 0; i < num_columns; i++) {
        char col_name[256] = {0};
        file.read(col_name, 256);
        uint8_t dtype;
        file.read(reinterpret_cast<char*>(&dtype), sizeof(dtype));
        columns.push_back(make_pair(string(col_name), dtype));
    }
    return columns;
}

// void updateTableSchema(fstream &file, uint32_t table_ptr, const vector<pair<string, uint8_t>>& new_columns) {
//     createTableSchema(file, table_ptr, new_columns);
// }

// void deleteTableSchema(fstream &file, uint32_t table_ptr) {
//     file.seekp(table_ptr, ios::beg);
//     uint8_t zero = 0;
//     file.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
// }


void createTableData(fstream &file, int table_ptr, int total_schema_bytes) {
    file.seekp(table_ptr, ios::beg);

	char zero[BIT8_MAX * BIT8_MAX] = {0};

	for (int i = 0; i < BIT8_MAX; i++) {
		file.write(reinterpret_cast<const char*>(&zero), 1);
		file.write(reinterpret_cast<const char*>(&zero), total_schema_bytes);
	}
}

vector<vector<string>> readTableData(fstream &file, int table_offset, const vector<pair<string, uint8_t>> &columns) {
    vector<vector<string>> table_data;
    file.seekg(table_offset, ios::beg);
    
    for (int i = 0; i < 256; i++) { // Maximum 256 records
        uint8_t flag;
        file.read(reinterpret_cast<char*>(&flag), sizeof(uint8_t));
        if (file.eof()) break;
        
        if (flag == 1) { // Valid record
            vector<string> row;
            for (const auto& column : columns) {
                string value;
                switch (column.second) {
                    case 0x0: { // INT
                        int temp;
                        file.read(reinterpret_cast<char*>(&temp), sizeof(int));
                        value = to_string(temp);
                        break;
                    }
                    case 0x1: { // FLOAT
                        float temp;
                        file.read(reinterpret_cast<char*>(&temp), sizeof(float));
                        value = to_string(temp);
                        break;
                    }
                    case 0x2: { // CHAR
                        char temp;
                        file.read(&temp, sizeof(char));
                        value = string(1, temp);
                        break;
                    }
                    case 0x3: { // STRING
                        char temp[255];
                        file.read(temp, 255);
                        value = string(temp);
                        break;
                    }
                    case 0x4: { // BOOL
                        bool temp;
                        file.read(reinterpret_cast<char*>(&temp), sizeof(bool));
                        value = temp ? "true" : "false";
                        break;
                    }
                    default:
                        value = "UNKNOWN";
                        break;
                }
                row.push_back(value);
            }
            table_data.push_back(row);
        } else { // Skip over the record
            for (const auto& column : columns) {
                switch (column.second) {
                    case 0x0: file.seekg(sizeof(int), ios::cur); break;
                    case 0x1: file.seekg(sizeof(float), ios::cur); break;
                    case 0x2: file.seekg(sizeof(char), ios::cur); break;
                    case 0x3: file.seekg(255, ios::cur); break;
                    case 0x4: file.seekg(sizeof(bool), ios::cur); break;
                    default: break;
                }
            }
        }
    }
    return table_data;
}



void insertTableData(fstream &file, uint32_t table_ptr, const vector<struct_name_id_data> &data, int total_schema_bytes) {
    file.seekp(table_ptr, ios::beg);

	uint8_t flag = 0x0;
	for (int i = 0; i < BIT8_MAX; i++) {
		file.seekg(table_ptr + (1 + total_schema_bytes) * i, ios::beg);
		file.read(reinterpret_cast<char*>(&flag), sizeof(flag));
		if (flag == 0) {
			break;
		}
	}

	uint8_t insert_flag = 0x1;
	file.write(reinterpret_cast<const char*>(&insert_flag), sizeof(insert_flag));
	
    uint8_t num_columns = data.size();
    for (const auto& val : data) {
        if (val.id == 0) {
            int value = val.int_data;
            file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        } else if (val.id == 1) {
            double value = val.float_data;
            file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		} else if (val.id == 2) {
            char value = val.char_data;
            file.write(reinterpret_cast<const char*>(value), sizeof(value));
        } else if (val.id == 3) {
            char value[256] = {0};
            strncpy(value, val.string_data.c_str(), sizeof(value));
            file.write(value, strlen(value));  // Write only up to the actual length
        } else if (val.id == 4) {
            bool value = val.bool_data;
            file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        } else {
			cout << "Error: Invalid data type." << endl;
			return;
		}
    }
}