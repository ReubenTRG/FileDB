#include "../include/global.h"
#include "../include/parser.h"

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <fstream>

#define BIT8_MAX 256
#define POINTER_SIZE 4

using namespace std;

vector<struct_col_dtype> typeNameToId(const vector<struct_column_datatype> &table_schema) {
    vector<struct_col_dtype> table_schema_id;
    for (auto col: table_schema) {
        struct_col_dtype temp;
        temp.col_name = col.name;
        if (col.type == "INT") {
            temp.id = 0x0;
            table_schema_id.push_back(temp);
        } else if (col.type == "FLOAT") {
            temp.id = 0x1;
            table_schema_id.push_back(temp);
        } else if (col.type == "CHAR") {
            temp.id = 0x2;
            table_schema_id.push_back(temp);
        } else if (col.type == "STRING") {
            temp.id = 0x3;
            table_schema_id.push_back(temp);
        } else if (col.type == "BOOL") {
            temp.id = 0x4;
            table_schema_id.push_back(temp);
        } else {
            cerr << "Error: Invalid data type." << endl;
        }
    }
	return table_schema_id;
}

int totalSchemaBytes(const vector<struct_col_dtype>& table_schema) {
    int total_schema_bytes = 0;
    for (size_t i = 0; i < table_schema.size(); i++) {
        switch (table_schema[i].id) {
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
                return -1;
        }
    }
    return total_schema_bytes;
}

vector<struct_name_id_data> schemaToValue(const vector<struct_col_dtype>& table_schema, const vector<string>& values) {
    vector<struct_name_id_data> data;
    for (size_t i = 0; i < table_schema.size(); i++) {
        struct_name_id_data temp;
        temp.name = table_schema[i].col_name;
        temp.id = table_schema[i].id;

		cout << "(test) Value: " << values[i] << " id " << temp.id << " name " << temp.name << endl;
        
        switch (table_schema[i].id) {
            case 0x0: // INT
                temp.int_data = stoi(values[i]);
                break;
            case 0x1: // FLOAT
                temp.float_data = stof(values[i]);
                break;
            case 0x2: // CHAR
                temp.char_data = values[i][0];
                break;
            case 0x3: // STRING
                temp.string_data = values[i];
                break;
            case 0x4: // BOOL
                temp.bool_data = (values[i] == "true" || values[i] == "1");
                break;
            default:
                vector<struct_name_id_data> empty;
                return empty;
        }
        data.push_back(temp);
    }
    return data;
}

