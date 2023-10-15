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
    JSONValue* getv(int);
    JSONValue* getv(int, int);
    JSONValue* getv(int, int, int);
    JSONValue* getv(int, int, int, int);

    JSONObject* geto(int);
    JSONObject* geto(int, int);
    JSONObject* geto(int, int, int);
    JSONObject* geto(int, int, int, int);

    JSONValue* key_val(const std::string&);
    JSONObject* key_obj(const std::string&);



private:
    std::istream* jsf = 0;
    std::vector<JSONValue*> values;
    std::vector<std::pair<std::string, char>> keys;
    std::vector<JSONObject*> obj_values;
    // the idea is to have these function kind of like a map, first will 
    // be the text of the second, and second will be a char denoting the data type
    // index will be kept equal across keys/values/obj_values and the pointers 
    // will be null in obj_values if that index is in values. duh, I guess.
    bool was_exp(std::istream&, const char);
    void nested_same(std::istream&, const char, const char, std::string&);
    JSONObject* fix_nested(std::istream&, const char, std::string&);
    std::string get_next_key(std::istream&);
    JSONValue* get_next_value(std::istream&);
    
};

void print_object_info(JSONObject js);
