#include "../include/file_handler.h"
#include "../include/logic.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <utility>

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
// string readDatabaseName(fstream &file, string& name) {
// 	file.seekg(0, ios::beg);
// 	uint8_t databaseName[BIT8_MAX] = {0};
// 	file.read(reinterpret_cast<char *>(databaseName), sizeof(databaseName));
// 	return reinterpret_cast<char *>(databaseName);
// }

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
	uint8_t table_pointer[POINTER_SIZE] = {0};
	file.read(reinterpret_cast<char *>(&table_name), sizeof(table_pointer));
	file.read(reinterpret_cast<char *>(&table_pointer), sizeof(table_name));
	return make_pair(reinterpret_cast<char *>(table_name), reinterpret_cast<int>(table_pointer));
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

uint8_t findTableIndex(fstream &file, const string& table_name) {
    uint8_t table_count = readPointersCount(file);
    for (uint8_t i = 0; i < table_count; i++) {
        auto [existing_name, _] = readPointers(file, i);
        if (existing_name == table_name) {
            return i;
        }
    }
    return 255; // Return an invalid index if not found
}

void insertTablePointer(fstream &file, const string& table_name, uint32_t table_ptr) {
    uint8_t table_count = readPointersCount(file);
    
    if (findTableIndex(file, table_name) != 255) {
        cerr << "Error: Table already exists." << endl;
        return;
    }
    
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

// void createTableSchema(fstream &file, uint32_t table_ptr, const vector<pair<string, uint8_t>>& columns) {
//     file.seekp(table_ptr, ios::beg);
//     uint8_t num_columns = columns.size();
//     file.write(reinterpret_cast<const char*>(&num_columns), sizeof(num_columns));
//     for (const auto& [col_name, dtype] : columns) {
//         uint8_t col_name_arr[256] = {0};
//         strncpy(reinterpret_cast<char*>(col_name_arr), col_name.c_str(), sizeof(col_name_arr));
//         file.write(reinterpret_cast<const char*>(col_name_arr), 256);
//         file.write(reinterpret_cast<const char*>(&dtype), sizeof(dtype));
//     }
// }

// vector<pair<string, uint8_t>> readTableSchema(fstream &file, uint32_t table_ptr) {
//     file.seekg(table_ptr, ios::beg);
//     uint8_t num_columns;
//     file.read(reinterpret_cast<char*>(&num_columns), sizeof(num_columns));
//     vector<pair<string, uint8_t>> columns;
//     for (uint8_t i = 0; i < num_columns; i++) {
//         char col_name[256] = {0};
//         file.read(col_name, 256);
//         uint8_t dtype;
//         file.read(reinterpret_cast<char*>(&dtype), sizeof(dtype));
//         columns.emplace_back(string(col_name), dtype);
//     }
//     return columns;
// }

// void updateTableSchema(fstream &file, uint32_t table_ptr, const vector<pair<string, uint8_t>>& new_columns) {
//     createTableSchema(file, table_ptr, new_columns);
// }

// void deleteTableSchema(fstream &file, uint32_t table_ptr) {
//     file.seekp(table_ptr, ios::beg);
//     uint8_t zero = 0;
//     file.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
// }




// Old file handling functions
int databaseNameLength(fstream &file) {
	uint8_t db_name_length;
	file.seekg(0, ios::beg);
	file.read(reinterpret_cast<char*>(&db_name_length), sizeof(uint8_t));
	return db_name_length;
}

int createEmptyFile(const string& name) {
	ofstream file(name + ".fdb", ios::binary);

	if (!file) {
		cerr << "Error: Could not create the database file \"" << name << "\"." << endl;
		return -1;
	}

	// Get the length of the name
	uint8_t name_length = static_cast<uint8_t>(name.length());

	// Write the length as a single byte
	file.write(reinterpret_cast<const char*>(&name_length), sizeof(uint8_t));

	// Write each character of the name as one byte
	file.write(name.c_str(), name.length());

	// Write a 1-byte hex counter for the number of tables (initialize to 0)
	uint8_t table_count = 0;
	file.write(reinterpret_cast<const char*>(&table_count), sizeof(uint8_t));

	// Write an array of table entries (BIT8_MAX bytes for table name, 4 bytes for pointer)
	for (int i = 0; i < BIT8_MAX; i++) {
		// Write BIT8_MAX bytes for the table name (all zeros initially)
		uint8_t table_name[BIT8_MAX] = {0};
		file.write(reinterpret_cast<const char*>(table_name), BIT8_MAX);

		// Write 4 bytes for the table pointer (all zeros initially)
		uint32_t table_pointer = 0;
		file.write(reinterpret_cast<const char*>(&table_pointer), sizeof(uint32_t));
	}

	file.close();
	return 0;
}



// int tableCount(fstream &file, int database_name_length) {
// 	uint8_t table_count;
// 	file.seekg(database_name_length + 1, ios::beg);
// 	file.read(reinterpret_cast<char*>(&table_count), sizeof(uint8_t));
// 	if (table_count == BIT8_MAX) {
// 		cerr << "Error: No tables found in the database." << endl;
// 		return -1;
// 	} else {
// 		return table_count;
// 	}
// }

int tableCountIncrement(fstream &file, int curr_offset, int table_count) {
	uint8_t new_table_count = table_count + 1;
	file.seekp(curr_offset, ios::beg);
	file.write(reinterpret_cast<const char*>(&new_table_count), sizeof(uint8_t));
	return curr_offset + 1;
}

int tableWriteName(fstream &file, int curr_offset, int table_count, const string& name) {
	file.seekp(curr_offset + 230 * table_count, ios::beg);
	uint8_t table_name[BIT8_MAX] = {0};
	strncpy(reinterpret_cast<char*>(table_name), name.c_str(), name.length());
	file.write(reinterpret_cast<const char*>(table_name), BIT8_MAX);
	return curr_offset + 230 * table_count + BIT8_MAX;
}

int tableWritePointer(fstream &file, int curr_offset) {
	file.seekp(0, ios::end);
	uint32_t schema_ptr = file.tellp();

	file.seekp(curr_offset + 1, ios::beg);
	file.write(reinterpret_cast<const char*>(&schema_ptr), sizeof(uint32_t));
	return schema_ptr;
}

int tableWriteNofDatatypes(fstream &file, int curr_offset, int schema_length) {
	file.seekp(curr_offset, ios::beg);
	file.write(reinterpret_cast<const char*>(&schema_length), sizeof(uint8_t));
	return curr_offset + 1;
}

// int tableWriteDatatypesCode(fstream &file, int curr_offset, vector<struct_column_datatype> columns) {
// 	int schema_length = 0;
// 	for (const auto& column : columns) {
// 		bool found = false;
// 		for (const auto& datatype : table_schema_datatype) {
// 			if (datatype.first == column.type) {
// 				file.write(reinterpret_cast<const char*>(&datatype.second), sizeof(uint8_t));
// 				schema_length += datatype;
// 				found = true;
// 				break;
// 			}
// 		}
// 		if (!found) {
// 			cerr << "Error: Invalid column data type." << endl;
// 			file.close();
// 			return -1;
// 		}
// 	}

// 	return curr_offset + schema_length;
// }

int tableWriteEmpty(fstream &file, int curr_offset, int schema_length) {
	uint8_t empty[BIT8_MAX] = {0};
	file.seekp(curr_offset, ios::beg);
	for (int i = 0; i < BIT8_MAX; i++) {
		file.write(reinterpret_cast<const char*>(&empty), schema_length);
	}

	return curr_offset + BIT8_MAX * schema_length;
}