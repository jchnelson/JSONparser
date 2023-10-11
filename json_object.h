#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "json_value.h"

class JSONObject
{
public:
    explicit JSONObject(const std::string fn);
    std::vector<JSONObject*> get_nested() { return obj_values; }
    std::vector<std::string> get_keys() { return keys; }
    std::vector<JSONValue> get_values() { return values; }
private:
    std::string filename;
    std::ifstream jsf{filename};
    std::vector<JSONValue> values;
    std::vector<std::string> keys;
    std::vector<JSONObject*> obj_values;
    // the idea is to have these function kind of like a map, with a possible
    // object attached to the key instead of the value.
    bool was_exp(const char c);
    void nested_same(const char, const char, std::string&);
    void fix_nested(const char, std::string&);
    std::string get_next_key();
    JSONValue get_next_value();
    
};

bool got_exp(std::istream& is, const char c);