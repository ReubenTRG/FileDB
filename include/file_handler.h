#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

#include "./global.h"

using namespace std;

// New code

void createDatabaseName(fstream &file, const string& name);
string readDatabaseName(fstream &file, string& name);
string updateDatabaseName(fstream &file, string& name);


void createPointersCount(fstream &file);
uint8_t readPointersCount(fstream &file);
uint8_t updatePointersCount(fstream &file, uint8_t table_count);

void createPointers(fstream &file);
pair<string, int> readPointers(fstream &file, uint8_t table_count);
uint32_t findTablePointers(fstream &file, const string& table_find_name);
int findTableIndex(fstream &file, const string& table_name);
void insertTablePointer(fstream &file, const string& table_name, uint32_t table_ptr);
void updateTablePointer(fstream &file, const string& table_name, uint32_t new_ptr);
void updateTableName(fstream &file, const string& old_name, const string& new_name);
int deleteTablePointer(fstream &file, const string& table_name);


void createTableSchema(fstream &file, int table_ptr, vector<struct_col_dtype> &columns);
vector<struct_col_dtype> readTableSchema(fstream &file, uint32_t table_ptr);
void updateTableSchema(fstream &file, uint32_t table_ptr, const vector<struct_col_dtype>& new_columns);
void deleteTableSchema(fstream &file, uint32_t table_ptr);

void createTableData(fstream &file, int table_ptr, int total_schema_bytes);
vector<vector<string>> readTableData(fstream &file, int table_offset, const vector<struct_col_dtype> &columns);
void insertTableData(fstream &file, uint32_t table_ptr, const vector<struct_name_id_data> &data, int total_schema_bytes);
void deleteTableData(fstream &file, uint32_t table_ptr, int total_schema_bytes);

void dropTable(fstream &file, string table_name);

#endif