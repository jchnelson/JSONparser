#pragma once

#include <string>
#include <iostream>
#include <new>

#include "json_base.h"


class JSONValue : public JSONBase
{
    friend std::ostream& operator<<(std::ostream&, JSONBase*);
public:
    JSONValue() : val(INT), ival(0) { }
    JSONValue(int i) : val(INT), ival(i) { }
    JSONValue(double d) : val(DBL), dval(d) { }
    JSONValue(bool b) : val(BOOL), bval(b) { }
    JSONValue(const std::string& s) : val(STR) { new (&sval) std::string(s); }
    ~JSONValue() { using std::string; if (val == STR) sval.~string(); }
    JSONValue(const JSONValue&);

    JSONBase* operator[](const std::string&);

    std::ostream& print(std::ostream& os);
    JSONBase* at(const std::string&);
    JSONValue& operator=(int i);
    JSONValue& b_eq(bool b);
    JSONValue& operator=(double d);
    JSONValue& operator=(std::string s);
    JSONValue& operator=(const JSONValue&);

    int get_ival() { return ival; }
    bool get_bval() { return bval; }
    double get_dval() { return dval; }
    std::string get_sval() { return sval; }

    char type() { 
        if (val == INT)
            return 'i';
        else if (val == DBL)
            return 'd';
        else if (val == BOOL)
            return 'b';
        else if (val == STR)
            return 's'; 
        else
            return 0;
    }
    


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

std::ostream& operator<<(std::ostream&, JSONBase*);
std::ostream& print_object_info(std::ostream&, JSONBase*);