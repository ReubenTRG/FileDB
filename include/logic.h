#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>
#include <map>

#include "./parser.h"
#include "./semantic.h"

using namespace std;

void createDatabase(const string& name);
void createTable(const string& name, const string& database_name,  vector<struct_column_datatype> columns);

void insertRow(const string& table_name, const string& database_name, vector<string> values);

void deleteRows(const string& database_name, const string& table_name);
void deleteRowsCondition(const string& database_name, const string& table_name, const string& column, const string& op, const string& value);

string selectTableAll(const string& database_name, const string& table_name);
string selectTable(const string& database_name, const string& table_name, const vector<string> columns);
string selectTableAllCond(const string& database_name, const string& table_name, const string column, const string oper, const string value);
string selectTableCond(const string& database_name, const string& table_name, const vector<string>& columns, const string& column, const string& oper, const string& value);

int useDatabase(const string& database_name);

void dropDatabase(const string& database_name);
void dropTable(const string& database_name, const string& table_name);

string updateTableCond(const string& database_name, const string& table_name, const string& column_name, const string& value, const string& condition_column, const string& oper, const string& condition_value);
string updateTable(const string& database_name, const string& table_name, const string& column_name, const string& value);

#endif