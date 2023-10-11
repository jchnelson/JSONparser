#include "json_value.h"

#include <string>
#include <new>

JSONValue::JSONValue(const JSONValue& j)
{
    copyUnion(j);
    val = j.val;
}

JSONValue& JSONValue::operator=(const JSONValue& j)
{
    using std::string;
    if (val == STR && j.val != STR) sval.~string();
    if (val == STR && j.val == STR) sval = j.sval;
    else copyUnion(j);

    val = j.val;
    return *this;
}

JSONValue& JSONValue::operator=(int i)
{
    using std::string;
    if (val == STR) sval.~string();
    ival = i;
    val = INT;
    return *this;
}
JSONValue& JSONValue::operator=(bool b)
{
    using std::string;
    if (val == STR) sval.~string();
    bval = b;
    val = BOOL;
    return *this;
}
JSONValue& JSONValue::operator=(double d)
{
    using std::string;
    if (val == STR) sval.~string();
    dval = d;
    val = DBL;
    return *this;
}
JSONValue& JSONValue::operator=(std::string s)
{
    using std::string;
    if (val == STR) 
        sval = s;
    else
        new (&sval) string(s);
    val = STR;
    return *this;
}

void JSONValue::copyUnion(const JSONValue& j)
{
    using std::string;
    switch (j.val)
    { 
        case JSONValue::INT: ival = j.ival; break;
        case JSONValue::BOOL: bval = j.bval; break;
        case JSONValue::DBL: dval = j.dval; break;
        case JSONValue::STR: new (&sval) string(j.sval); break;
    }
}

std::ostream& operator<<(std::ostream& os, const JSONValue& j)
{
    if (j.val == JSONValue::INT)
        os << j.ival;
    else if (j.val == JSONValue::DBL)
        os << j.dval;
    else if (j.val == JSONValue::BOOL)
        os << std::boolalpha << j.bval << std::noboolalpha;
    else if (j.val == JSONValue::STR)
        os << j.sval;

    return os;
}