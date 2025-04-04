#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "./parser.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

struct struct_dtype_id_size {
	string name;
	int code;
	int size;
};

struct struct_name_id_data {
	string name;
	int id;

	int int_data;
	double float_data;
	char char_data;
	string string_data;
	bool bool_data;
};

struct struct_col_dtype {
	string col_name;
	int id;
};

const vector<struct_dtype_id_size> table_schema_datatype = {
	{"INT", 0x0, 0x4},
	{"FLOAT", 0x1, 0x8},
	{"CHAR", 0x2, 0x1},
	{"STRING", 0x3, 0xff},
	{"BOOL", 0x4, 0x1}
};

vector<struct_col_dtype> typeNameToId(const vector<struct_column_datatype>& table_schema);

int totalSchemaBytes(const vector<struct_col_dtype>& table_schema);

vector<struct_name_id_data> schemaToValue(const vector<struct_col_dtype>& table_schema, const vector<string>& values);

int columnSize(int id);

string datatypeIdToString(int id);

bool compare(const string& column, const string& oper, const string& value, int dataTypeId, const struct_name_id_data& data);

#endif