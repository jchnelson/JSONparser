#include "json_value.h"

#include <string>
#include <new>

JSONValue::JSONValue(const JSONValue& j)
{
    copyUnion(j);
    val = j.val;
}

JSONBase* JSONValue::operator[](const std::string&)
{
    return 0;
}
JSONBase* JSONValue::at(const std::string&)
{
    return 0;
}

std::ostream& JSONValue::print(std::ostream& os)
{
    if (val == JSONValue::INT)
        os << ival;
    else if (val == JSONValue::DBL)
        os << dval;
    else if (val == JSONValue::BOOL)
        os << std::boolalpha << bval << std::noboolalpha;
    else if (val == JSONValue::STR)
        os << sval;

    return os;
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
JSONValue& JSONValue::b_eq(bool b)
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
