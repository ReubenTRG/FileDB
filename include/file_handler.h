#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

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
void deleteTablePointer(fstream &file, const string& table_name);


void createTableSchema(fstream &file, int table_ptr, vector<pair<string, uint8_t>> &columns);
vector<pair<string, uint8_t>> readTableSchema(fstream &file, uint32_t table_ptr);
void updateTableSchema(fstream &file, uint32_t table_ptr, const vector<pair<string, uint8_t>>& new_columns);
void deleteTableSchema(fstream &file, uint32_t table_ptr);

void createTableData(fstream &file, int table_ptr, int total_schema_bytes);


// vector<pair<string, uint8_t>> readTableSchema(fstream &file, uint32_t table_ptr);

// Old code
int createEmptyFile(const string& name);

int databaseNameLength(fstream &file);

int tableCount(fstream &file, int database_name_length);

int tableCountIncrement(fstream &file, int curr_offset, int table_count);

// int tableWriteName(fstream &file, int curr_offset, int table_count, string name);

int tableWritePointer(fstream &file, int curr_offset);

int tableWriteNofDatatypes(fstream &file, int curr_offset, int schema_length);

// int tableWriteDatatypesCode(fstream &file, int curr_offset, vector<struct_column_datatype> columns);

#endif