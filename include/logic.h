#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>
#include <map>

#include "parser.h"

using namespace std;

struct struct_dtype_id_size {
	string name;
	int code;
	int size;
};

struct struct_name_dtype_data {
	string name;
	int code;
	
	int int_data;
	float float_data;
	char char_data;
	string string_data;
	bool bool_data;
};

const vector<struct_dtype_id_size> table_schema_datatype = {
	{"INT", 0x0, 0x4},
	{"FLOAT", 0x1, 0x8},
	{"CHAR", 0x2, 0x1},
	{"STRING", 0x3, 0xff},
	{"BOOL", 0x4, 0x1}
};

void createDatabase(const string& name);

void createTable(const string& name, const string& database_name,  vector<struct_column_datatype> columns);

// void 

#endif