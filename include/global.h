#ifndef GLOBAL_H
#define GLOBAL_H

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

#endif