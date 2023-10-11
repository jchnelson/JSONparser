
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "json_value.h"

using std::cout;
using std::string;
using std::vector;
using std::begin;
using std::end;

bool got_exp(std::istream& is, const char c)
{
    if (is.peek() == c)
    {
        is.get();
        cout << "removed expected " << c << '\n';
        return true;
    }
    else 
        return false;
}

std::string get_next_key(std::istream& is, char* arr)
{
    cout << "getting next key\n";
    char throwaway[10]{};
    is.get(throwaway, 10, '"'); // grab until ", leave on stream
    got_exp(is, '"');
    is.get(arr, 100, '"');
    std::streamsize read_size = is.gcount();
    cout << read_size << " gcount\n";
    for (size_t i = 0; i != 100; ++i)
        cout << arr[i];
    cout << '\n';
    got_exp(is, '"');

    got_exp(is, ':');
    cout << "returning string\n";
    return string (arr, 0, read_size);
}

int main()
{
    vector<string> toplevel_keys;

    std::ifstream jsf("counter_query.json");
    if (jsf.get() == '{')
        cout << "bob\n";

    char steve[100]{};

    string first_entry = get_next_key(jsf, steve);
    std::streamsize read_size = jsf.gcount();

    JSONValue bob;

    string slop;
    jsf.get(steve, 100, ',');
    read_size = jsf.gcount();
    cout << read_size << '\n';

    if (got_exp(jsf, ','))  // this area could yield an array, or a nested object
    {
        if (std::all_of(begin(steve), begin(steve) + read_size, [] (const char c) 
            { return isspace(c) || isdigit(c); }))
        {
            cout << "it's a number\n";
            for (size_t i = 0; i != read_size; ++i)
            {
                if (!isspace(steve[i]))
                {
                    slop.push_back(steve[i]);
                }
                
            }
            bob = std::stoi(slop);
        }
        else
        {
            bool initial_space_skipped = false;
            for (size_t i = 0; i != read_size; ++i)
            {
                if (!initial_space_skipped && isspace(steve[i]))
                {
                    if (!isspace(steve[i+1]))
                        initial_space_skipped = true;
                    continue;
                }
                if (steve[i] != '"')
                {
                    slop.push_back(steve[i]);
                }
                
            }
            if (slop == "true")
                bob = true;
            else if (slop == "false")
                bob = false;
            else if (slop == "null")
                bob = 0;
            else
                bob = slop;
        }
    }
    cout << bob << '\n';

    JSONValue bill;
    bill = get_next_key(jsf, steve);
    cout << bill;
    
        
        





    // a template....that creates.....pairs?  the pair.second would need to 
    // be a template parameter, but I guess the first one can always be a string?

}