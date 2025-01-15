#ifndef READER_H
#define READER_H

#include <string>
#include <vector>
using namespace std;

struct struct_condition {
	string column;
	string oper;
	string value;
};

struct struct_column_datatype {
	string name;
	string type;
};

struct struct_insert {
    string table_name;
    vector<string> values;
};

struct struct_update {
    string table_name;
    string column_name;
    string value;
    bool has_condition;
    string condition_column;
    string condition_operator;
    string condition_value;
};

struct struct_delete {
    string table_name;
    bool has_condition;
    string condition_column;
    string condition_operator;
    string condition_value;
};

struct struct_drop {
    bool is_database;
    string name;
};

extern const int commands_count;
extern const string commands_list[];

string trimming(const string& str);
int parse_command(const string& str);

vector<string> parse_SELECT_column_list(const string& sql);
string parse_SELECT_table(const string& sql);
struct struct_condition parse_SELECT_condition(const string& sql);

string parse_CREATE_option(const string& sql);
string parse_CREATE_name(const string& sql);
vector<struct_column_datatype> parse_CREATE_columns(const string& sql);

struct_insert parse_INSERT(const string& sql);

struct_update parse_UPDATE(const string& sql);

struct_delete parse_DELETE(const string& sql);

string parse_USE(const string& sql);

struct_drop parse_DROP(const string& sql);

#endif
