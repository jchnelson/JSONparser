#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

#include "json_value.h"

class JSONObject
{
public:
    JSONObject() = default;
    explicit JSONObject(const std::string fn);
    JSONObject(std::istream*);
    JSONObject(std::vector<JSONValue*> v)
        : values(v) { }
    const std::vector<JSONObject*>&  get_nested() const { return obj_values; }
    const std::vector<std::pair<std::string, char>>& get_keys() const { return keys; }
    const std::vector<JSONValue*>& get_values() const { return values; }

private:
    std::istream* jsf = 0;
    std::vector<JSONValue*> values;
    std::vector<std::pair<std::string, char>> keys;
    std::vector<JSONObject*> obj_values;
    // the idea is to have these function kind of like a map, first will 
    // be the text of the second, and second will be a char denoting the data type
    // index will be kept equal across keys/values/obj_values and the pointers 
    // will be null in obj_values if that index is in values. duh, I guess.
    bool was_exp(const char);
    bool was_exp(std::istream&, const char);
    void nested_same(const char, const char, std::string&);
    JSONObject* fix_nested(const char, std::string&);
    std::string get_next_key();
    JSONValue* get_next_value();
    
};
