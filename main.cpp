
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

void fix_nested(std::istream& is, const char c, string& s)
{
    size_t charcount = 1;
    s += ',';
    char stever[10000]{};
    is.get(stever, 10000, c);

    // like with the calculator parentheses, check for the
    // opening of other instances of the opener that this function
    // was called with ( '{' or '[' ) to appropriately ignore the inner sets
    // and get the full outer set

    // with current file "trade channel" is where it screws up

    size_t count = is.gcount();
    for (size_t i = 0; i != count; ++i)
        s += stever[i];
    if (got_exp(is, c))
        s += c;
}

std::string get_next_key(std::istream& is)
{
    char arr[100]{};
    cout << "getting next key\n";
    is.get(arr, 10, '"'); // grab until ", leave on stream
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

JSONValue get_next_value(std::istream& is)
{
    std::streamsize read_size = is.gcount();

    JSONValue bob;
    char steve[100]{};
    string slop;
    is.get(steve, 100, ',');
    read_size = is.gcount();
    cout << read_size << '\n';

    if (got_exp(is, ','))  // this area could yield an array, or a nested object
    {
        if (std::all_of(begin(steve), begin(steve) + read_size, [](const char c)
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
                    if (!isspace(steve[i + 1]))
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
            {
                bool has_square = slop.find('[') != string::npos;
                bool has_curly = slop.find('{') != string::npos;
                char stever[10000]{};
                if (has_square && !has_curly)
                {
                    fix_nested(is, ']', slop);
                }
                else if (!has_square && has_curly)
                {
                    fix_nested(is, '}', slop);
                }
                else if (has_square && has_curly && slop.find('[') < slop.find('{'))
                {
                    fix_nested(is, ']', slop);
                }
                else
                {
                    fix_nested(is, '}', slop);
                }
                bob = slop;
            }
        }
    }
    cout << bob << '\n';
    return bob;
}

int main()
{
    vector<string> toplevel_keys;
    vector<JSONValue> toplevel_values;

    std::ifstream jsf("counter_query.json");
    if (jsf.get() == '{')
        cout << "bob\n";

    for (int i = 0; i != 6; ++i)
    {
        toplevel_keys.push_back(get_next_key(jsf));
        toplevel_values.push_back(get_next_value(jsf));
    }
    toplevel_keys.push_back(get_next_key(jsf));
    JSONValue florb = get_next_value(jsf);
    cout << florb;
    //string florb_s = florb.get_sval();
    //size_t brackindex = florb_s.find('[');
    //char steve[1000]{};
    //if (brackindex != string::npos)
    //{ 
    //    florb_s += ',';
    //    string slop;
    //    jsf.get(steve, 1000, ']');
    //    size_t count = jsf.gcount();
    //    for (size_t i = 0; i != count; ++i)
    //        florb_s += steve[i];
    //}

    //if(got_exp(jsf, ']'))
    //    florb_s += ']';
    //cout << florb_s;
        
    //string first_entry = get_next_key(jsf);
    //std::streamsize read_size = jsf.gcount();

    //JSONValue bob = get_next_value(jsf);

    //JSONValue bill;
    //bill = get_next_key(jsf);
    //cout << bill;


    // a template....that creates.....pairs?  the pair.second would need to 
    // be a template parameter, but I guess the first one can always be a string?
}