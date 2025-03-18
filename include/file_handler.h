#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include <map>

using namespace std;

int createEmptyFile(const string& name);

int databaseNameLength(fstream &file);

int tableCount(fstream &file, int database_name_length);

int tableCountIncrement(fstream &file, int curr_offset, int table_count);

int tableWriteName(fstream &file, int curr_offset, int table_count, string name);

int tableWritePointer(fstream &file, int curr_offset);

int tableWriteNofDatatypes(fstream &file, int curr_offset, int schema_length);

int tableWriteDatatypesCode(fstream &file, int curr_offset, vector<struct_column_datatype> columns);

#endif