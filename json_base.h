#pragma once

#include <string>

class JSONBase
{
public:
    virtual ~JSONBase() = default;
    virtual JSONBase* operator[](const std::string&) = 0;
    virtual JSONBase* at(const std::string&) = 0;
    virtual char type() = 0;
    virtual std::ostream& print(std::ostream&) = 0;
};
