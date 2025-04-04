#include "../include/parser.h"
#include "../include/logic.h"
#include "../include/file_handler.h"
#include "../include/semantic.h"

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

void createTable(const string& table_name, const string& database_name,  const vector<struct_column_datatype>& columns) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return;
    }

    int found = findTableIndex(file, table_name);
    if (found != -1) {
		cerr << "Error: Table \"" << table_name << "\" already exists." << endl;
		return;
	}

	uint8_t table_count = readPointersCount(file);
	if (table_count >= BIT8_MAX) {
		cerr << "Error: Maximum number of tables reached." << endl;
		return;
	}
    
    file.seekp(0, ios::end);
    int end_of_file_pos  = file.tellp();
    insertTablePointer(file, table_name, end_of_file_pos);

	updatePointersCount(file, table_count + 1);

    cout << "(test) table schema: ";
    for (auto& col: columns) {
        cout << col.name << ": " << col.type << " ";
    }
    
    vector<struct_col_dtype> columns_schema = typeNameToId(columns);
    cout << "(test) table schema2: ";
    for (auto& col : columns_schema) {
        cout << col.col_name << ": " << col.id << " ";
    }
    
    int total_schema_bytes = totalSchemaBytes(columns_schema);
    cout << "(test) totalschemabytes: " << total_schema_bytes << endl;
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
    for (auto& col : table_schema) {
        cout << col.col_name << ": " << col.id << " ";
    }
    cout << endl;

    int total_schema_bytes = totalSchemaBytes(table_schema);
    if (total_schema_bytes == -1) {
        cerr << "Error: invalid datatype." << endl;
        return;
    }

    vector<struct_name_id_data> data = schemaToValue(table_schema, values);
    if (data.size() == 0) {
        cerr << "Error: invalid datatype." << endl;
        return;
    }

    insertTableData(file, table_schema_ptr + 1 + (256 + 1) * table_schema.size(), data, total_schema_bytes);

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

    deleteTableData(file, table_schema_ptr + 1 + (1 + 256) * table_schema.size(), total_schema_bytes);

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

// void deleteRowsCondition(const string& database_name, const string& table_name, const string& column, const string& op, const string& value) {
//     fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
//     if (!file) {
//         cerr << "Error: Could not open database file." << endl;
//         return;
//     }
//
//     int pointer_ptr = findTableIndex(file, table_name);
//     if (pointer_ptr == -1) {
//         cerr << "Error: Table \"" << table_name << "\" doesnt exists." << endl;
//         return;
//     }
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
//     // Find the column index in the schema
//     int column_index = -1;
//     int column_offset = 0;
//     for (size_t i = 0; i < table_schema.size(); i++) {
//         if (table_schema[i].col_name == column) {
//             column_index = i;
//             break;
//         }
//         // Calculate offset for each column before the target column
//         switch (table_schema[i].id) {
//             case 0x0: // INT
//                 column_offset += 0x4;
//                 break;
//             case 0x1: // FLOAT
//                 column_offset += 0x8;
//                 break;
//             case 0x2: // CHAR
//                 column_offset += 0x1;
//                 break;
//             case 0x3: // STRING
//                 column_offset += 0xff;
//                 break;
//             case 0x4: // BOOL
//                 column_offset += 0x1;
//                 break;
//         }
//     }
//
//     if (column_index == -1) {
//         cerr << "Error: Column \"" << column << "\" not found in table." << endl;
//         return;
//     }
//
//     // Get column data type
//     uint8_t column_type = table_schema[column_index].id;
//
//     // Seek to the start of table data
//     uint32_t data_start_pos = table_schema_ptr + 1 + total_schema_bytes;
//     file.seekg(data_start_pos, ios::beg);
//
//     // Process each row
//     char flag;
//     bool condition_met;
//     int int_val, int_comp;
//     double float_val, float_comp;
//     char char_val, char_comp;
//     string str_val, str_comp;
//     bool bool_val, bool_comp;
//
//     // Convert comparison value based on column type
//     switch (column_type) {
//         case 0x0: // INT
//             int_comp = stoi(value);
//             break;
//         case 0x1: // FLOAT
//             float_comp = stod(value);
//             break;
//         case 0x2: // CHAR
//             char_comp = value[0];
//             break;
//         case 0x3: // STRING
//             str_comp = value;
//             break;
//         case 0x4: // BOOL
//             bool_comp = (value == "true" || value == "1");
//             break;
//     }  
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
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return;
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);
    int total_schema_bytes = totalSchemaBytes(table_schema);
    
    int column_index = -1, column_offset = 0;
    for (size_t i = 0; i < table_schema.size(); i++) {
        if (table_schema[i].col_name == column) {
            column_index = i;
            break;
        }
        column_offset += columnSize(table_schema[i].id);
    }
    if (column_index == -1) {
        cerr << "Error: Column \"" << column << "\" not found." << endl;
        return;
    }

    vector<pair<struct_name_id_data, int>> data = findTableDataByColumn(file, table_schema_ptr + 1 + total_schema_bytes, column_offset, total_schema_bytes, columnSize(table_schema[column_index].id));
    for (const auto& pair : data) {
        if (compare(pair.first, op, value, table_schema[column_index].id)) {
            file.seekp(pair.second, ios::beg);
            uint8_t zero = 0;
            file.write(reinterpret_cast<char*>(&zero), sizeof(zero));
        }
    }

    file.close();
}
string selectTableAll(const string& database_name, const string& table_name) {
    string response = "";

    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return "Error: Could not open database file.";
    }

    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return "Error: Table doesn't exist.";
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    // Read schema
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);
    
    response += "Table: " + table_name + "\nSchema:\n";
    
    for (const auto& col : table_schema) {
        response += col.col_name + "(" + datatypeIdToString(col.id) + ")\n";
    }

    // Read table data
    vector<vector<struct_name_id_data>> table_data = readTableData(
        file, 
        table_schema_ptr + 1 + (1 + 256) * table_schema.size(), 
        table_schema
    );

    response += "\nData:\n";
    for (const auto& row : table_data) {
        for (const auto& cell : row) {
            switch (cell.id) {
                case 0x0: // INT
                    response += to_string(cell.int_data) + " ";
                    break;
                case 0x1: // FLOAT
                    response += to_string(cell.float_data) + " ";
                    break;
                case 0x2: // CHAR
                    response += cell.char_data + " ";
                    break;
                case 0x3: // STRING
                    response += cell.string_data + " ";
                    break;
                case 0x4: // BOOL
                    response += to_string(cell.bool_data) + " ";
                    break;
                default:
                    return "INVALID";
            }
        }
        response += "\n";
    }

    file.close();
    return response;
}

string selectTable(const string& database_name, const string& table_name, const vector<string> columns) {
    string response = "";

    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return "Error: Could not open database file.";
    }

    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return "Error: Table doesn't exist.";
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    // Read schema
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);
    
    response += "Table: " + table_name + "\nSchema:\n";
    
    vector<int> selected_indices;
    for (size_t i = 0; i < table_schema.size(); ++i) {
        if (find(columns.begin(), columns.end(), table_schema[i].col_name) != columns.end()) {
            selected_indices.push_back(i);
            response += table_schema[i].col_name + "(" + datatypeIdToString(table_schema[i].id) + ")\n";
        }
    }

    // Read table data
    vector<vector<struct_name_id_data>> table_data = readTableData(
        file, 
        table_schema_ptr + 1 + (1 + 256) * table_schema.size(), 
        table_schema
    );

    response += "\nData:\n";
    for (const auto& row : table_data) {
        for (int index : selected_indices) {  // Only process selected columns
            const auto& cell = row[index];
            switch (cell.id) {
                case 0x0: // INT
                    response += to_string(cell.int_data) + " ";
                    break;
                case 0x1: // FLOAT
                    response += to_string(cell.float_data) + " ";
                    break;
                case 0x2: // CHAR
                    response += cell.char_data + " ";
                    break;
                case 0x3: // STRING
                    response += cell.string_data + " ";
                    break;
                case 0x4: // BOOL
                    response += to_string(cell.bool_data) + " ";
                    break;
                default:
                    return "INVALID";
            }
        }
        response += "\n";
    }

    file.close();
    return response;
}

string selectTableAllCond(const string& database_name, const string& table_name, const string column, const string oper, const string value) {
    string response = "";

    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return "Error: Could not open database file.";
    }

    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return "Error: Table doesn't exist.";
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    // Read schema
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);

    response += "Table: " + table_name + "\nSchema:\n";
    int condition_col_index = -1;
    int condition_col_type = -1;

    for (size_t i = 0; i < table_schema.size(); ++i) {
        response += table_schema[i].col_name + "(" + datatypeIdToString(table_schema[i].id) + ")\n";
        if (table_schema[i].col_name == column) {
            condition_col_index = i;
            condition_col_type = table_schema[i].id;
        }
    }

    if (condition_col_index == -1) {
        file.close();
        return "Error: Specified column does not exist.";
    }

    // Read table data
    vector<vector<struct_name_id_data>> table_data = readTableData(
        file, 
        table_schema_ptr + 1 + (1 + 256) * table_schema.size(), 
        table_schema
    );

    response += "\nData:\n";
    for (const auto& row : table_data) {
        if (compare(column, oper, value, condition_col_type, row[condition_col_index])) {
            for (const auto& cell : row) {
                switch (cell.id) {
                    case 0x0: // INT
                        response += to_string(cell.int_data) + " ";
                        break;
                    case 0x1: // FLOAT
                        response += to_string(cell.float_data) + " ";
                        break;
                    case 0x2: // CHAR
                        response += cell.char_data + " ";
                        break;
                    case 0x3: // STRING
                        response += cell.string_data + " ";
                        break;
                    case 0x4: // BOOL
                        response += to_string(cell.bool_data) + " ";
                        break;
                    default:
                        return "INVALID";
                }
            }
            response += "\n";
        }
    }

    file.close();
    return response;
}

string selectTableCond(const string& database_name, const string& table_name, const vector<string>& columns, const string& column, const string& oper, const string& value) {
    string response = "";

    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return "Error: Could not open database file.";
    }

    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return "Error: Table doesn't exist.";
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    // Read schema
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);

    response += "Table: " + table_name + "\nSchema:\n";

    // Find the indexes of selected columns
    vector<int> selected_indices;
    for (size_t i = 0; i < table_schema.size(); ++i) {
        if (find(columns.begin(), columns.end(), table_schema[i].col_name) != columns.end()) {
            selected_indices.push_back(i);
            response += table_schema[i].col_name + "(" + datatypeIdToString(table_schema[i].id) + ")\n";
        }
    }

    // Find the index and type of the column used in WHERE condition
    int condition_col_index = -1;
    int condition_col_type = -1;
    for (size_t i = 0; i < table_schema.size(); ++i) {
        if (table_schema[i].col_name == column) {
            condition_col_index = i;
            condition_col_type = table_schema[i].id;
            break;
        }
    }

    if (condition_col_index == -1) {
        file.close();
        return "Error: Specified column does not exist.";
    }

    // Read table data
    vector<vector<struct_name_id_data>> table_data = readTableData(
        file, 
        table_schema_ptr + 1 + (1 + 256) * table_schema.size(), 
        table_schema
    );

    response += "\nData:\n";
    for (const auto& row : table_data) {
        if (compare(column, oper, value, condition_col_type, row[condition_col_index])) {
            for (int index : selected_indices) {  // Print only selected columns
                const auto& cell = row[index];
                switch (cell.id) {
                    case 0x0: // INT
                        response += to_string(cell.int_data) + " ";
                        break;
                    case 0x1: // FLOAT
                        response += to_string(cell.float_data) + " ";
                        break;
                    case 0x2: // CHAR
                        response += cell.char_data + " ";
                        break;
                    case 0x3: // STRING
                        response += cell.string_data + " ";
                        break;
                    case 0x4: // BOOL
                        response += to_string(cell.bool_data) + " ";
                        break;
                    default:
                        return "INVALID";
                }
            }
            response += "\n";
        }
    }

    file.close();
    return response;
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

string updateTableCond(const string& database_name, const string& table_name, const string& column_name, const string& value, const string& condition_column, const string& oper, const string& condition_value) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return "Error: Could not open database file.";
    }

    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return "Error: Table doesn't exist.";
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    // Read schema
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);

    int condition_col_index = -1, update_col_index = -1;
    int condition_col_type = -1;

    // Find indexes of columns
    for (size_t i = 0; i < table_schema.size(); ++i) {
        if (table_schema[i].col_name == condition_column) {
            condition_col_index = i;
            condition_col_type = table_schema[i].id;
        }
        if (table_schema[i].col_name == column_name) {
            update_col_index = i;
        }
    }

    if (condition_col_index == -1 || update_col_index == -1) {
        file.close();
        return "Error: Column not found.";
    }

    vector<vector<struct_name_id_data>> table_data = readTableData(
        file, 
        table_schema_ptr + 1 + (1 + 256) * table_schema.size(), 
        table_schema
    );

    for (auto& row : table_data) {
        if (compare(condition_column, oper, condition_value, condition_col_type, row[condition_col_index])) {
            switch (row[update_col_index].id) {
                case 0x0: row[update_col_index].int_data = stoi(value); break;
                case 0x1: row[update_col_index].float_data = stod(value); break;
                case 0x2: row[update_col_index].char_data = value[0]; break;
                case 0x3: row[update_col_index].string_data = value; break;
                case 0x4: row[update_col_index].bool_data = (value == "true"); break;
                default: return "Error: Invalid data type.";
            }
        }
    }

    writeTableData(file, table_schema_ptr + 1 + (1 + 256) * table_schema.size(), table_data);
    file.close();
    return "Update successful.";
}

string updateTable(const string& database_name, const string& table_name, const string& column_name, const string& value) {
    fstream file(database_name + ".fdb", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not open database file." << endl;
        return "Error: Could not open database file.";
    }

    cout << "test: " << table_name << "\n";
    int pointer_ptr = findTableIndex(file, table_name);
    if (pointer_ptr == -1) {
        cerr << "Error: Table \"" << table_name << "\" doesn't exist." << endl;
        return "Error: Table doesn't exist.";
    }

    auto [_, table_schema_ptr] = readPointers(file, pointer_ptr);

    // Read schema
    vector<struct_col_dtype> table_schema = readTableSchema(file, table_schema_ptr);

    int update_col_index = -1;
    for (size_t i = 0; i < table_schema.size(); ++i) {
        if (table_schema[i].col_name == column_name) {
            update_col_index = i;
            break;
        }
    }

    if (update_col_index == -1) {
        file.close();
        return "Error: Column not found.";
    }

    vector<vector<struct_name_id_data>> table_data = readTableData(
        file, 
        table_schema_ptr + 1 + (1 + 256) * table_schema.size(), 
        table_schema
    );

    for (auto& row : table_data) {
        switch (row[update_col_index].id) {
            case 0x0: row[update_col_index].int_data = stoi(value); break;
            case 0x1: row[update_col_index].float_data = stod(value); break;
            case 0x2: row[update_col_index].char_data = value[0]; break;
            case 0x3: row[update_col_index].string_data = value; break;
            case 0x4: row[update_col_index].bool_data = (value == "true"); break;
            default: return "Error: Invalid data type.";
        }
    }

    writeTableData(file, table_schema_ptr + 1 + (1 + 256) * table_schema.size(), table_data);
    file.close();
    return "Update successful.";
}

