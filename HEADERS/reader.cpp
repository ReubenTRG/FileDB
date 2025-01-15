#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include "reader.h"

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

int parse_command(const string& str) {
    string upper_str = str;
    transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
    for (int i = 0; i < commands_count; i++) {
        if (upper_str.find(commands_list[i]) != string::npos) {
            return i;
        }
    }
    return -1;
}

vector<string> parse_SELECT_column_list(const string& sql) {
    vector<string> columns;
    regex column_regex(R"(SELECT\s+(.+?)\s+FROM)", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, column_regex)) {
        string columns_str = match[1].str();
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
    }
    return columns;
}


string parse_SELECT_table(const string& sql) {
    string table;
    regex table_regex(R"(FROM\s+([^\s;]+))", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, table_regex)) {
        table = match[1].str();
    }
    return table;
}

struct struct_condition parse_SELECT_condition(const string& sql) {
    struct struct_condition cond;
    regex condition_regex(R"(WHERE\s+(\w+)\s*(=|>|<|>=|<=|!=)\s*(['\"]?.+?['\"]?))", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, condition_regex)) {
        cout << "testing: " << match[0].str() << endl;
        cond.column = match[1].str(); // Extract column name
        cond.oper = match[2].str();   // Extract operator
        cond.value = match[3].str(); // Extract value
    }

    return cond;
}

string parse_CREATE_option(const string& sql) {
    string option;
    // Regex to match CREATE TABLE or CREATE DATABASE
    regex option_regex(R"(CREATE\s+(TABLE|DATABASE))", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, option_regex)) {
        option = match[1].str(); // Extract the matched option
    }

    return option;
}

string parse_CREATE_name(const string& sql) {
    string name;
    // Regex to match CREATE TABLE or CREATE DATABASE with the name
    regex name_regex(R"(CREATE\s+(TABLE|DATABASE)\s+(\w+))", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, name_regex)) {
        name = match[2].str(); // Extract the name (table or database)
    }

    return name;
}

vector<struct_column_datatype> parse_CREATE_columns(const string& sql) {
    vector<struct_column_datatype> columns;

    // Regex to match columns within the CREATE TABLE statement
    regex column_regex(R"(CREATE\s+TABLE\s+\w+\s*\((.+?)\))", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, column_regex)) {
        string columns_str = match[1].str(); // Extract everything inside the parentheses

        // Regex to split individual column definitions by commas
        regex split_regex(R"(\s*,\s*)");
        sregex_token_iterator iter(columns_str.begin(), columns_str.end(), split_regex, -1);
        sregex_token_iterator end;

        // Process each column definition
        regex column_def_regex(R"((\w+)\s+(\w+(\(\d+\))?))", regex_constants::icase);
        smatch column_match;
        while (iter != end) {
            string column_def = *iter++;
            if (regex_match(column_def, column_match, column_def_regex)) {
                struct_column_datatype col;
                col.name = column_match[1].str(); // Extract column name
                col.type = column_match[2].str(); // Extract data type
                columns.push_back(col);
            }
        }
    } else {
        cerr << "Error: Invalid CREATE TABLE syntax." << endl;
    }

    return columns;
}

struct_insert parse_INSERT(const string& sql) {
    struct_insert insert_data;
    regex insert_regex(R"(INSERT\s+INTO\s+(\w+)\s*VALUES\s*\((.*?)\))", regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, insert_regex)) {
        insert_data.table_name = match[1].str(); // Extract table name

        // Extract values
        string values_str = match[2].str();
        regex split_regex(R"(\s*,\s*)");
        sregex_token_iterator iter(values_str.begin(), values_str.end(), split_regex, -1);
        sregex_token_iterator end;

        while (iter != end) {
            insert_data.values.push_back(*iter++);
        }
    } else {
        cerr << "Error: Invalid INSERT INTO syntax." << endl;
    }

    return insert_data;
}

struct_update parse_UPDATE(const string& sql) {
    struct_update update_data = {"", "", "", false, "", "", ""};

    // Regex for UPDATE without WHERE clause
    regex update_no_where_regex(
        R"(UPDATE\s+(\w+)\s+SET\s+(\w+)\s*=\s*(['"]?.*['"]?))",
        regex_constants::icase
    );

    // Regex for UPDATE with WHERE clause
    regex update_with_where_regex(
        R"(UPDATE\s+(\w+)\s+SET\s+(\w+)\s*=\s*(['"]?.+?['"]?)\s+WHERE\s+(\w+)\s*(==|>=|<=|!=)\s*(['"]?.*['"]?))",
        regex_constants::icase
    );

    smatch match;

    if (regex_search(sql, match, update_with_where_regex)) {
        // Match the structure with WHERE clause
        update_data.table_name = match[1].str();        // Extract table name
        update_data.column_name = match[2].str();      // Extract column name
        update_data.value = match[3].str();            // Extract value
        update_data.has_condition = true;
        update_data.condition_column = match[4].str(); // Extract condition column
        update_data.condition_operator = match[5].str(); // Extract condition operator
        update_data.condition_value = match[6].str();  // Extract condition value
    } else if (regex_search(sql, match, update_no_where_regex)) {
        // Match the structure without WHERE clause
        update_data.table_name = match[1].str();        // Extract table name
        update_data.column_name = match[2].str();      // Extract column name
        update_data.value = match[3].str();            // Extract value
        update_data.has_condition = false;
    } else {
        cerr << "Error: Invalid UPDATE statement syntax." << endl;
    }

    return update_data;
}

struct_delete parse_DELETE(const string& sql) {
    struct_delete delete_data = {"", false, "", "", ""};

    // Regex for DELETE without WHERE clause
    regex delete_no_where_regex(R"(DELETE\s+FROM\s+(\w+))", regex_constants::icase);

    // Regex for DELETE with WHERE clause
    regex delete_with_where_regex(R"(DELETE\s+FROM\s+(\w+)\s+WHERE\s+(\w+)\s*(==|>=|<=|!=|>|<)\s*(['"]?.*['"]?);)", regex_constants::icase);

    smatch match;

    if (regex_search(sql, match, delete_with_where_regex)) {
        // Match the structure with WHERE clause
        delete_data.table_name = match[1].str();        // Extract table name
        delete_data.has_condition = true;
        delete_data.condition_column = match[2].str(); // Extract condition column
        delete_data.condition_operator = match[3].str(); // Extract condition operator
        delete_data.condition_value = match[4].str();  // Extract condition value
    } else if (regex_search(sql, match, delete_no_where_regex)) {
        // Match the structure without WHERE clause
        delete_data.table_name = match[1].str();        // Extract table name
        delete_data.has_condition = false;
    } else {
        cerr << "Error: Invalid DELETE statement syntax." << endl;
    }

    return delete_data;
}

string parse_USE(const string& sql) {
    string use_data = "";

    // Regex to capture the database name after USE
    regex use_regex(R"(USE\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;)", regex_constants::icase);

    smatch match;

    if (regex_search(sql, match, use_regex)) {
        use_data = match[1].str();  // Extract the database name
    } else {
        cerr << "Error: Invalid USE statement syntax." << endl;
    }

    return use_data;
}

struct_drop parse_DROP(const string& sql) {
    struct_drop drop_data = {false, ""};

    regex drop_regex(R"(DROP\s+(TABLE|DATABASE)\s+(\w+))", regex_constants::icase);

    smatch match;

    if (regex_search(sql, match, drop_regex)) {
        drop_data.is_database = (match[1].str() == "DATABASE");
        drop_data.name = match[2].str();
    } else {
        cerr << "Error: Invalid DROP statement syntax." << endl;
    }

    return drop_data;
}