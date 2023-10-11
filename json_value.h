#pragma once

#include <string>
#include <iostream>


class JSONValue
{
    friend std::ostream& operator<<(std::ostream&, const JSONValue&);
public:
    JSONValue() : val(INT), ival(0) { }
    ~JSONValue() { using std::string; if (val == STR) sval.~string(); }
    JSONValue(const JSONValue&);
    JSONValue& operator=(const JSONValue&);
    int get_ival() { return ival; }
    bool get_bval() { return bval; }
    double get_dval() { return dval; }
    std::string get_sval() { return sval; }
    JSONValue& operator=(int i);
    JSONValue& operator=(bool b);
    JSONValue& operator=(double d);
    JSONValue& operator=(std::string s);
    


private:
   enum { INT, DBL, BOOL, STR } val;
   union {
       int ival;
       double dval;
       bool bval;
       std::string sval;
   };
   void copyUnion(const JSONValue&);
};

std::ostream& operator<<(std::ostream&, const JSONValue&);