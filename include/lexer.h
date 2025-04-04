#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>

#include "./semantic.h"

using namespace std;

bool validate_CREATE(const string& sql);

bool validate_SELECT(const string& sql);

bool validate_INSERT(const string& sql);

bool validate_UPDATE(const string& sql);

bool validate_DELETE(const string& sql);

bool validate_USE(const string& sql);

bool validate_DROP(const string& sql);

#endif