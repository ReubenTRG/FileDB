#include "../include/parser.h"
#include "../include/logic.h"
#include "../include/file_handler.h"
#include "../include/global.h"

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <fstream>
#include <filesystem>

#define BIT8_MAX 256
#define POINTER_SIZE 4

using namespace std;
namespace fs = std::filesystem;

// int totalSchemaBytes(const vector<struct_col_dtype>& table_schema) {
//     int total_schema_bytes = 0;
//     for (size_t i = 0; i < table_schema.size(); i++) {
//         switch (table_schema[i].id) {
//             case 0x0: // INT
//                 total_schema_bytes += 0x4;
//                 break;
//             case 0x1: // FLOAT
//                 total_schema_bytes += 0x8;
//                 break;
//             case 0x2: // CHAR
//                 total_schema_bytes += 0x1;
//                 break;
//             case 0x3: // STRING
//                 total_schema_bytes += 0xff;
//                 break;
//             case 0x4: // BOOL
//                 total_schema_bytes += 0x1;
//                 break;
//             default:
//                 return -1;
//         }
//     }
//     return total_schema_bytes;
// }

// vector<struct_name_id_data> schemaToValue(const vector<struct_col_dtype>& table_schema, const vector<string>& values) {
//     vector<struct_name_id_data> data;
//     for (size_t i = 0; i < table_schema.size(); i++) {
//         struct_name_id_data temp;
//         temp.name = table_schema[i].col_name;
//         temp.id = table_schema[i].id;
//         switch (table_schema[i].id) {
//             case 0x0: // INT
//                 temp.int_data = stoi(values[i]);
//                 break;
//             case 0x1: // FLOAT
//                 temp.float_data = stof(values[i]);
//                 break;
//             case 0x2: // CHAR
//                 temp.char_data = values[i][0];
//                 break;
//             case 0x3: // STRING
//                 temp.string_data = values[i];
//                 break;
//             case 0x4: // BOOL
//                 temp.bool_data = (values[i] == "true" || values[i] == "1");
//                 break;
//             default:
//                 vector<struct_name_id_data> empty;
//                 return empty;
//         }
//         data.push_back(temp);
//     }
//     return data;
// }

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
    
    vector<struct_col_dtype> columns_schema = typeNameToId(columns);
    
    int total_schema_bytes = totalSchemaBytes(columns_schema);
    if (total_schema_bytes == -1) {
        cerr << "Error: Invalid data type." << endl;
        return;
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

    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);
    cout << "(test) table schema: ";
    for (auto& col : table_schema) {
        cout << col.col_name << ": " << col.id << " ";
    }
    cout << endl;

    int total_schema_bytes = totalSchemaBytes(table_schema);
    cout << "(test) totalschemabytes: " << total_schema_bytes << endl;
    if (total_schema_bytes == -1) {
        cerr << "Error: invalid datatype." << endl;
        return;
    }

    vector<struct_name_id_data> data = schemaToValue(table_schema, values);
    if (data.size() == 0) {
        cerr << "Error: invalid datatype." << endl;
        return;
    }

    // test
    file.seekg(table_schema_ptr, ios::beg);
    uint8_t a;
    file.read(reinterpret_cast<char*>(&a), sizeof(a));
    cout << "(test) val at schema ptr: " << (int)a << endl;

    insertTableData(file, table_schema_ptr + 1 + total_schema_bytes, data, total_schema_bytes);

    file.close();
}

void deleteRows(const string& database_name, const string& table_name) {
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

    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);

    int total_schema_bytes = totalSchemaBytes(table_schema);
    if (total_schema_bytes == -1) {
        cerr << "Error: invalid datatype." << endl;
        return;
    }

    deleteTableData(file, table_schema_ptr + 1, total_schema_bytes);

    file.close();
}

// void deleteRowsCondition(const string& table_name, const string& database_name, const string& column, const string& op, const string& value) {
//     fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
//     if (!file) {
//         cerr << "Error: Could not open database file." << endl;
//         return;
//     }
//
//     int pointer_ptr = findTableIndex(file, table_name);
//     if (pointer_ptr == -1) {
// 		cerr << "Error: Table \"" << table_name << "\" doesnt exists." << endl;
// 		return;
// 	}
//
//     auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);
//
//     vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);
//
//     int total_schema_bytes = totalSchemaBytes(table_schema);
//     if (total_schema_bytes == -1) {
//         cerr << "Error: invalid datatype." << endl;
//         return;
//     }
//
//     // add more code
//
//     file.close();
// }

void deleteRowsCondition(const string& database_name, const string& table_name, const string& column, const string& op, const string& value) {
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

    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);

    int total_schema_bytes = totalSchemaBytes(table_schema);
    if (total_schema_bytes == -1) {
        cerr << "Error: invalid datatype." << endl;
        return;
    }

    // Find the column index in the schema
    int column_index = -1;
    int column_offset = 0;
    for (size_t i = 0; i < table_schema.size(); i++) {
        if (table_schema[i].col_name == column) {
            column_index = i;
            break;
        }
        // Calculate offset for each column before the target column
        switch (table_schema[i].id) {
            case 0x0: // INT
                column_offset += 0x4;
                break;
            case 0x1: // FLOAT
                column_offset += 0x8;
                break;
            case 0x2: // CHAR
                column_offset += 0x1;
                break;
            case 0x3: // STRING
                column_offset += 0xff;
                break;
            case 0x4: // BOOL
                column_offset += 0x1;
                break;
        }
    }

    if (column_index == -1) {
        cerr << "Error: Column \"" << column << "\" not found in table." << endl;
        return;
    }

    // Get column data type
    uint8_t column_type = table_schema[column_index].id;

    // Seek to the start of table data
    uint32_t data_start_pos = table_schema_ptr + 1 + total_schema_bytes;
    file.seekg(data_start_pos);

    // Process each row
    char flag;
    bool condition_met;
    int int_val, int_comp;
    float float_val, float_comp;
    char char_val, char_comp;
    string str_val, str_comp;
    bool bool_val, bool_comp;

    // Convert comparison value based on column type
    switch (column_type) {
        case 0x0: // INT
            int_comp = stoi(value);
            break;
        case 0x1: // FLOAT
            float_comp = stof(value);
            break;
        case 0x2: // CHAR
            char_comp = value[0];
            break;
        case 0x3: // STRING
            str_comp = value;
            break;
        case 0x4: // BOOL
            bool_comp = (value == "true" || value == "1");
            break;
    }

    // Read through all records
    while (file.read(&flag, 1)) {
        uint32_t record_start = file.tellg() - 1;
        
        // Skip deleted records
        if (flag == 0) {
            file.seekg(record_start + 1 + total_schema_bytes);
            continue;
        }

        // Move to the target column position
        file.seekg(record_start + 1 + column_offset);
        
        // Read value and compare based on column type
        condition_met = false;
        
        switch (column_type) {
            case 0x0: // INT
                file.read(reinterpret_cast<char*>(&int_val), sizeof(int_val));
                if (op == "=") condition_met = (int_val == int_comp);
                else if (op == "!=") condition_met = (int_val != int_comp);
                else if (op == ">") condition_met = (int_val > int_comp);
                else if (op == "<") condition_met = (int_val < int_comp);
                else if (op == ">=") condition_met = (int_val >= int_comp);
                else if (op == "<=") condition_met = (int_val <= int_comp);
                break;
                
            case 0x1: // FLOAT
                file.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
                if (op == "=") condition_met = (float_val == float_comp);
                else if (op == "!=") condition_met = (float_val != float_comp);
                else if (op == ">") condition_met = (float_val > float_comp);
                else if (op == "<") condition_met = (float_val < float_comp);
                else if (op == ">=") condition_met = (float_val >= float_comp);
                else if (op == "<=") condition_met = (float_val <= float_comp);
                break;
                
            case 0x2: // CHAR
                file.read(&char_val, 1);
                if (op == "=") condition_met = (char_val == char_comp);
                else if (op == "!=") condition_met = (char_val != char_comp);
                else if (op == ">") condition_met = (char_val > char_comp);
                else if (op == "<") condition_met = (char_val < char_comp);
                else if (op == ">=") condition_met = (char_val >= char_comp);
                else if (op == "<=") condition_met = (char_val <= char_comp);
                break;
                
            case 0x3: // STRING
                {
                    char str_buffer[0xff] = {0};
                    file.read(str_buffer, 0xff);
                    str_val = str_buffer;
                    
                    if (op == "=") condition_met = (str_val == str_comp);
                    else if (op == "!=") condition_met = (str_val != str_comp);
                    else if (op == ">") condition_met = (str_val > str_comp);
                    else if (op == "<") condition_met = (str_val < str_comp);
                    else if (op == ">=") condition_met = (str_val >= str_comp);
                    else if (op == "<=") condition_met = (str_val <= str_comp);
                    else if (op == "LIKE") {
                        // Simple pattern matching for LIKE operator
                        // This is a basic implementation and might need to be expanded
                        if (str_comp.front() == '%' && str_comp.back() == '%') {
                            // %text%
                            string pattern = str_comp.substr(1, str_comp.length() - 2);
                            condition_met = (str_val.find(pattern) != string::npos);
                        } else if (str_comp.front() == '%') {
                            // %text
                            string pattern = str_comp.substr(1);
                            condition_met = (str_val.length() >= pattern.length() && 
                                           str_val.substr(str_val.length() - pattern.length()) == pattern);
                        } else if (str_comp.back() == '%') {
                            // text%
                            string pattern = str_comp.substr(0, str_comp.length() - 1);
                            condition_met = (str_val.length() >= pattern.length() && 
                                           str_val.substr(0, pattern.length()) == pattern);
                        } else {
                            // Exact match if no wildcard
                            condition_met = (str_val == str_comp);
                        }
                    }
                }
                break;
                
            case 0x4: // BOOL
                file.read(reinterpret_cast<char*>(&bool_val), 1);
                if (op == "=") condition_met = (bool_val == bool_comp);
                else if (op == "!=") condition_met = (bool_val != bool_comp);
                break;
        }
        
        // If condition is met, mark the record as deleted by setting flag to 0
        if (condition_met) {
            file.seekp(record_start);
            flag = 0;
            file.write(&flag, 1);
        }
        
        // Move to the next record
        file.seekg(record_start + 1 + total_schema_bytes);
    }

    file.close();
}

int useDatabase(const string& database_name) {
    string file_name = database_name + ".fdb";

    if (fs::exists(file_name)) {
        return 1;
    } else {
        return -1;
    }
}

void dropDatabase(const string& database_name) {
    string file_name = database_name + ".fdb";

    if (fs::exists(file_name)) {
        try {
            fs::remove(file_name);
            cout << "Database '" << database_name << "' has been deleted successfully.\n";
        } catch (const fs::filesystem_error& e) {
            cerr << "Error: Failed to delete database '" << database_name << "' - " << e.what() << endl;
        }
    } else {
        cerr << "Error: Database '" << database_name << "' not found.\n";
    }
}

void dropTable(const string& database_name, const string& table_name) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return;
    }

    dropTable(file, table_name);

    file.close();
}