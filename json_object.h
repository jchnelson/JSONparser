#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <initializer_list>

#include "json_base.h"
#include "json_value.h"



class JSONObject : public JSONBase
{
public:
    JSONObject() = default;
    JSONObject(const JSONObject&) = default;
    explicit JSONObject(const std::string fn);
    explicit JSONObject(std::istream*);
    explicit JSONObject(std::map<std::string, JSONBase*> vm);
    explicit JSONObject(const std::string&, std::vector<JSONValue*>);
    explicit JSONObject(const std::string&, std::vector<JSONObject*>);
    ~JSONObject();



    JSONObject& operator=(const JSONObject&) = default;
    JSONBase* operator[](const std::string&);
    JSONBase* at(const std::string&);
    std::ostream& print(std::ostream&);
    std::map<std::string, JSONBase*>::iterator begin() { return valmap.begin(); }
    std::map<std::string, JSONBase*>::iterator end() { return valmap.end(); }
    char type() { return 'j'; }
    const std::vector<std::string>& key_index() const { return keyindex; }
    std::string to_csv();
    bool to_file(const std::string&);
    void insert(const std::string&, JSONValue*);
    void insert(const std::string&, JSONObject*);
    void insert(const std::string&, std::vector<JSONValue*>);
    void insert(const std::string&, std::vector<JSONObject*>);
    template <typename T>
    void insert(const std::string&, T);
    template <typename T>
    void insert(const std::string&, std::initializer_list<T>);
    


private:
    std::istream* jsf = 0;
    std::map<std::string, JSONBase*> valmap;
    std::vector<std::string> keyindex;
    bool was_exp(std::istream&, const char);
    void nested_same(std::istream&, const char, const char, std::string&);
    JSONBase* fix_nested(std::istream&, std::string&);
    std::string get_next_key(std::istream&);
    JSONBase* get_next_value(std::istream&);  
    void out_fileobject(std::ofstream&, JSONObject&, size_t&, bool, bool);
};


std::ostream& operator<<(std::ostream&, JSONBase*);
JSONObject& keyobj(JSONBase* jsb);
JSONValue& keyval(JSONBase* jsb);

template <typename T>
void JSONObject::insert(const std::string& key, T val)
{
    valmap.insert({key, new JSONValue(val)});
    keyindex.push_back(key);
}

template <typename T>
void JSONObject::insert(const std::string& key, std::initializer_list<T> il)
{
    size_t count = 0;
    JSONObject* first = new JSONObject();
    for (auto& item : il)
    {
        std::string newkey = std::to_string(count);
        first->keyindex.push_back(newkey);
        first->valmap.insert({ newkey, new JSONValue(item) });
        ++count;
    }
    valmap.insert({ key, first });
    keyindex.push_back(key);
}
