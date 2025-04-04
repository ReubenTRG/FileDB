#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

#include "../include/lexer.h";
#include "../include/semantic.h"

using namespace std;

const int commands_count = 7;
const string commands_list[commands_count] = {
    "CREATE",
    "INSERT",
    "SELECT",
    "UPDATE",
    "DELETE",
    "USE",
    "DROP"
};

string ltrim(const string str) {
    size_t start = str.find_first_not_of(" \t");
    return (start == string::npos) ? "" : str.substr(start);
}

string rtrim(const string str) {
    size_t end = str.find_last_not_of(" \t");
    return (end == string::npos) ? "" : str.substr(0, end + 1);
}

string trimming(const string& str) {
    return rtrim(ltrim(str));
}

int lexer_command(const string& str) {
    string upper_str = str;
    transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
    for (int i = 0; i < commands_count; i++) {
        if (upper_str.find(commands_list[i]) != string::npos) {
            return i;
        }
    }
    return -1;
}

vector<string> splitColumns(const string& columns_str) {
    vector<string> columns;
    if (columns_str == "*") {
        columns.push_back("*");
    } else {
        regex split_regex(R"(\s*,\s*)");
        sregex_token_iterator iter(columns_str.begin(), columns_str.end(), split_regex, -1);
        sregex_token_iterator end;
        while (iter != end) {
            columns.push_back(*iter++);
        }
    }
    return columns;
}

bool validate_CREATE(const string& sql) {
    regex create_table(R"(CREATE\s+TABLE\s+(\w+)\s*\((.+?)\)\s*;?\s*$)", regex_constants::icase);
    regex create_database(R"(CREATE\s+DATABASE\s+(\w+)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, create_table) || regex_match(sql, create_database)) {
        return true;
    }

    cerr << "Error: Invalid CREATE statement syntax." << endl;
    cerr << "Hint: Use CREATE TABLE table_name (column1 datatype, column2 datatype, ...); or CREATE DATABASE database_name;" << endl;
    return false;
}

bool validate_SELECT(const string& sql) {
    regex select_with_where(R"(SELECT\s+(.+?)\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*(==|>=|<=|!=|>|<)\s*(['"]?.*?['"]?)\s*;?\s*$)", regex_constants::icase);
    regex select_no_where(R"(SELECT\s+(.+?)\s+FROM\s+(\w+)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, select_with_where) || regex_match(sql, select_no_where)) {
        return true;
    }

    cerr << "Error: Invalid SELECT statement syntax." << endl;
    cerr << "Hint: Use SELECT col1, col2 FROM table_name [WHERE col == value];" << endl;
    return false;
}

bool validate_INSERT(const string& sql) {
    regex insert(R"(INSERT\s+INTO\s+(\w+)\s*VALUES\s*\((.*?)\)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, insert)) {
        return true;
    }

    cerr << "Error: Invalid INSERT INTO syntax." << endl;
    cerr << "Hint: Use INSERT INTO table_name VALUES (val1, val2, val3);" << endl;
    return false;
}

bool validate_UPDATE(const string& sql) {
    regex update_with_where(R"(UPDATE\s+(\w+)\s+SET\s+(\w+)\s*=\s*(['"]?.+?['"]?)\s+WHERE\s+(\w+)\s*(==|>=|<=|!=|>|<)\s*(['"]?.*?['"]?)\s*;?\s*$)", regex_constants::icase);
    regex update_no_where(R"(UPDATE\s+(\w+)\s+SET\s+(\w+)\s*=\s*(['"]?.*?['"]?)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, update_with_where) || regex_match(sql, update_no_where)) {
        return true;
    }

    cerr << "Error: Invalid UPDATE statement syntax." << endl;
    cerr << "Hint: Use UPDATE table_name SET column = value [WHERE column == value];" << endl;
    return false;
}

bool validate_DELETE(const string& sql) {
    regex delete_with_where(R"(DELETE\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*(==|>=|<=|!=|>|<)\s*(['"]?.*?['"]?)\s*;?\s*$)", regex_constants::icase);
    regex delete_no_where(R"(DELETE\s+FROM\s+(\w+)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, delete_with_where) || regex_match(sql, delete_no_where)) {
        return true;
    }

    cerr << "Error: Invalid DELETE statement syntax." << endl;
    cerr << "Hint: Use DELETE FROM table_name [WHERE column == value];" << endl;
    return false;
}

bool validate_USE(const string& sql) {
    regex use(R"(USE\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, use)) {
        return true;
    }

    cerr << "Error: Invalid USE statement syntax." << endl;
    cerr << "Hint: Use USE database_name;" << endl;
    return false;
}

bool validate_DROP(const string& sql) {
    regex drop(R"(DROP\s+(TABLE|DATABASE)\s+(\w+)\s*;?\s*$)", regex_constants::icase);

    if (regex_match(sql, drop)) {
        return true;
    }

    cerr << "Error: Invalid DROP statement syntax." << endl;
    cerr << "Hint: Use DROP TABLE table_name; or DROP DATABASE db_name;" << endl;
    return false;
}