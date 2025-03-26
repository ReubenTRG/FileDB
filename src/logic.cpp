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

void createTable(const string& name, const string& database_name,  vector<struct_column_datatype> columns) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return;
    }

	uint8_t table_count = readPointersCount(file);
	if (table_count >= BIT8_MAX) {
		cerr << "Error: Maximum number of tables reached." << endl;
		return;
	}

	updatePointersCount(file, table_count + 1);

	

	file.close();
}



// Old code
// void write_number(ofstream& file, const string& str) {
// 	uint8_t length = static_cast<uint8_t>(str.length());
// 	file.write(reinterpret_cast<const char*>(&length), sizeof(uint8_t));
// 	file.write(str.c_str(), length);
// }

// void createDatabase(const string& name) {
// 	// Create a file with the name as the parameter
// 	int result = createEmptyFile(name);
// 	if (result == -1) {
// 		cerr << "Error: Could not create the database file \"" << name << "\"." << endl;
// 	} else {
// 		cout << "Database \"" << name << "\" created successfully." << endl;
// 	}
// }

// int databaseNameLength(fstream &file) {
// 	uint8_t db_name_length;
// 	file.seekg(0, ios::beg);
// 	file.read(reinterpret_cast<char*>(&db_name_length), sizeof(uint8_t));
// 	return db_name_length;
// }

// int tableCount(fstream &file, int database_name_length) {
// 	uint8_t table_count;
// 	file.seekg(database_name_length + 1, ios::beg);
// 	file.read(reinterpret_cast<char*>(&table_count), sizeof(uint8_t));
// 	if (table_count == 256) {
// 		cerr << "Error: No tables found in the database." << endl;
// 		return -1;
// 	} else {
// 		return table_count;
// 	}
// }

// int tableCountIncrement(fstream &file, int curr_offset, int table_count) {
// 	uint8_t new_table_count = table_count + 1;
// 	file.seekp(curr_offset, ios::beg);
// 	file.write(reinterpret_cast<const char*>(&new_table_count), sizeof(uint8_t));
// 	return curr_offset + 1;
// }

// int tableEmptySlot(fstream &file, int curr_offset, int table_count) {
// 	file.seekg(curr_offset + 230 * table_count, ios::beg);
	
// }

// void createTable(const string& name, const string& database_name, vector<struct_column_datatype> columns) {
// 	string db_file = database_name + ".fdb"; // Replace with your actual database file name
// 	int curr_offset = 0;
// 	fstream file(db_file, ios::in | ios::out | ios::binary);

// 	if (!file) {
// 		cerr << "Error: Could not open the database file \"" << db_file << "\"." << endl;
// 		return;
// 	}

// 	// Read and increment the number of tables
// 	uint8_t database_name_length = databaseNameLength(file);
// 	file.seekg(database_name_length + 1, ios::beg); // Skip database name and move to table count

// 	uint8_t table_count = tableCount(file, database_name_length);
// 	if (table_count == -1) {
// 		cerr << "Error: Maximum table limit (256) reached." << endl;
// 		file.close();
// 		return;
// 	}

// 	// Increment the table count
// 	// curr_offset = tableCountIncrement(file, curr_offset, table_count);

// 	// curr_offset = tableWriteName(file, curr_offset, table_count, name);

// 	// curr_offset = tableWritePointer(file, curr_offset);

// 	// curr_offset = tableWriteNofDatatypes(file, curr_offset, columns.size());

// 	// curr_offset = tableWriteDatatypesCode(file, curr_offset, columns);

// 	// curr_offset = tableWriteEmpty(file, curr_offset, schema_length);

// 	// for (const auto& column : columns) {
// 	// 	bool found = false;
// 	// 	for (const auto& datatype : table_schema_datatype) {
// 	// 		if (datatype.first == column.type) {
// 	// 			file.write(reinterpret_cast<const char*>(&datatype.first), sizeof(uint8_t));
// 	// 			found = true;
// 	// 			break;
// 	// 		}

// 	// 	if (!found) {
// 	// 			cerr << "Error: Invalid column data type." << endl;
// 	// 			file.close();
// 	// 			return;
// 	// 		}
// 	// 	}
// 	// }



// 	file.close();
// 	cout << "Table \"" << name << "\" created successfully with schema." << endl;
// }
