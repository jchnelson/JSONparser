#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

#include "json_base.h"
#include "json_value.h"



class JSONObject : public JSONBase
{
public:
    JSONObject() = default;
    JSONObject(const JSONObject&) = default;
    explicit JSONObject(const std::string fn);
    explicit JSONObject(std::istream*);
    explicit JSONObject(std::map<std::string, JSONBase*> vm)
        : valmap(vm) { }
    JSONObject& operator=(const JSONObject&) = default;
    JSONBase* operator[](const std::string&);
    JSONBase* at(const std::string&);
    std::ostream& print(std::ostream&);
    std::map<std::string, JSONBase*>::iterator begin() { return valmap.begin(); }
    std::map<std::string, JSONBase*>::iterator end() { return valmap.end(); }
    char type() { return 'j'; }
    const std::vector<std::string>& key_index() const { return keyindex; }
    std::string to_csv();





private:
    std::istream* jsf = 0;
    std::map<std::string, JSONBase*> valmap;
    std::vector<std::string> keyindex;
    bool was_exp(std::istream&, const char);
    void nested_same(std::istream&, const char, const char, std::string&);
    JSONBase* fix_nested(std::istream&, const char, std::string&);
    std::string get_next_key(std::istream&);
    JSONBase* get_next_value(std::istream&);   
};


std::ostream& operator<<(std::ostream&, JSONBase*);
JSONObject& keyobj(JSONBase* jsb);
JSONValue& keyval(JSONBase* jsb);