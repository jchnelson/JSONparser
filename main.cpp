
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
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
        //cout << "removed expected " << c << '\n';
        return true;
    }
    else 
        return false;
}

void nested_same(std::istream& is, const char open, const char close, string& s)
{
    char buffer[10000]{};
    cout << s;
    size_t charcount = std::count(s.begin()+1, s.end(), open);
    cout << "\nnumber of extra '" << open << "' : " << charcount << '\n';
    size_t next_index = s.find(open);  // next nested open bracket
    size_t count = is.gcount();
    while (count != 0 && charcount > 0)
    {
        is.get(buffer, 10000, close); // to next close
        count = is.gcount();
        cout << count << " characters read onto string and charcount is" 
            << charcount << "\n";
        string bob = string(buffer, 0, count);
        size_t bobcount = std::count(bob.begin(), bob.end(), open);
        charcount += bobcount;
        cout << bob << '\n';
        size_t prevsize = s.size();
        s += bob;  // add section 

        if (got_exp(is, close))
        {
            cout << "expected closing bracket found\n";
            s += close; // add the close bracket
            --charcount;
            cout << "charcount before condition: " <<charcount << "\n";
            
            if (charcount == 0)
                break;
        }

    }

        
}

void fix_nested(std::istream& is, const char c, string& s)
{
    size_t charcount = 1;
    s += ',';
    char buffer[10000]{};
    is.get(buffer, 10000, c);

    size_t count = is.gcount();
    cout << "last read was " << count << " characters\n";
    for (size_t i = 0; i != count; ++i)
        s += buffer[i];
    if (got_exp(is, c))
        s += c;

    char opener;
    if (c == ']')
        opener = '[';
    else
        opener = '{';

    size_t next_index = s.find(opener, 1);
    if (next_index == string::npos)
        cout << "additional " << opener << " not found: " << 
            next_index << "\n current string : ";
    cout << s << '\n';
    if (next_index != string::npos)
    { 
        cout << "additional " << opener << " found at " << next_index << '\n';
        nested_same(is, opener, c, s);
    }
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

    JSONValue ret;
    char readbuf[100]{};
    string slop;
    is.get(readbuf, 100, ',');
    read_size = is.gcount();
    cout << read_size << '\n';

    if (got_exp(is, ','))  // this area could yield an array, or a nested object
    {
        if (std::all_of(begin(readbuf), begin(readbuf) + read_size, [](const char c)
            { return isspace(c) || isdigit(c); }))
        {
            cout << "it's a number\n";
            for (size_t i = 0; i != read_size; ++i)
            {
                if (!isspace(readbuf[i]))
                {
                    slop.push_back(readbuf[i]);
                }

            }
            ret = std::stoi(slop);
        }
        else
        {
            bool initial_space_skipped = false;
            for (size_t i = 0; i != read_size; ++i)
            {
                if (!initial_space_skipped && isspace(readbuf[i]))
                {
                    if (!isspace(readbuf[i + 1]))
                        initial_space_skipped = true;
                    continue;
                }
                if (readbuf[i] != '"')
                {
                    slop.push_back(readbuf[i]);
                }

            }
            if (slop == "true")
                ret = true;
            else if (slop == "false")
                ret = false;
            else if (slop == "null")
                ret = 0;
            else
            {
                bool has_square = slop.find('[') != string::npos;
                bool has_curly = slop.find('{') != string::npos;
                if ( (has_square && !has_curly) ||
                    (has_square && has_curly && slop.find('[') < slop.find('{')) )
                {
                    fix_nested(is, ']', slop);
                }
                else 
                {
                    fix_nested(is, '}', slop);
                }
                ret = slop;
            }
        }
    }
    //cout << ret << '\n';
    return ret;
}

int main()
{
    vector<string> toplevel_keys;
    vector<JSONValue> toplevel_values;
    // vector<JSONObject>

    std::ifstream jsf("counter_query.json");
    if (jsf.get() == '{')
        cout << "bob\n";

    //for (int i = 0; i != 6; ++i)
    while (jsf)
    {
        toplevel_keys.push_back(get_next_key(jsf));
        toplevel_values.push_back(get_next_value(jsf));
    }

    cout << toplevel_keys.size() << '\n';
    cout << toplevel_values.size() << '\n';

  // last value is a blank string and 
  // 0, investigate later ig

 
}