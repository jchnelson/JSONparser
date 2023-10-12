
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "json_object.h"
#include "json_value.h"


using std::string;
using std::vector;
using std::cout;
using std::begin;
using std::end;

bool fuck_off = false;

JSONObject::JSONObject(const std::string fn)
    : jsf(new std::fstream(fn)) 
{

    if (jsf->get() != '{')
        throw std::runtime_error("Invalid JSON");

    while (jsf)
    {
        keys.push_back({get_next_key(), ' '});  // placeholder char, change later
        values.push_back(get_next_value());
        if (values.back() == 0)
            keys.back().second = 'j';
        else
            keys.back().second = values.back()->get_val();
        if (fuck_off)
            break;
    }
}


JSONObject::JSONObject(std::istream* js)
    : jsf(js)
{
    jsf->get(); // discard opening bracket if it exists?
    //cout << "Initializing new JSONObject from istream pointer\n";

    while (*jsf)
    {
        keys.push_back({ get_next_key(), ' ' });  // placeholder char, change later
        values.push_back(get_next_value());
        if (values.back() == 0)
            keys.back().second = 'j';
        else 
            keys.back().second = values.back()->get_val();
        if (fuck_off)
            break;
    }
}

bool JSONObject::was_exp(const char c)
{
    if (jsf->peek() == c)
    {
        jsf->get();
        ////cout << "removed expected " << c << '\n';
        return true;
    }
    else
        return false;
}

bool JSONObject::was_exp(std::istream& is, const char c)
{
    if (is.peek() == c)
    {
        is.get();
        ////cout << "removed expected " << c << '\n';
        return true;
    }
    else
    {
        return false;
    }
}

void JSONObject::nested_same(const char open, const char close, string& s)
{
    char buffer[10000]{};
    //cout << s;
    size_t charcount = std::count(s.begin() + 1, s.end(), open);
    //cout << "\nnumber of extra '" << open << "' : " << charcount << '\n';
    size_t next_index = s.find(open);  // next nested open bracket
    size_t count = jsf->gcount();
    while (count != 0 && charcount > 0)
    {
        jsf->get(buffer, 10000, close); // to next close
        count = jsf->gcount();
        //cout << count << " characters read onto string and charcount is"
            //<< charcount << "\n";
        string bob = string(buffer, 0, count);
        size_t bobcount = std::count(bob.begin(), bob.end(), open);
        charcount += bobcount;
        ////cout << bob << '\n';
        size_t prevsize = s.size();
        s += bob;  // add section 

        if (was_exp(close))
        {
            //cout << "expected closing bracket found\n";
            s += close; // add the close bracket
            --charcount;
            //cout << "charcount before condition: " << charcount << "\n";

            if (charcount == 0)
                break;
        }

    }


}

JSONObject* JSONObject::fix_nested(const char c, string& s)
{
    // so I have a string with brackets in it.  At this point I know only
    // that there is one 'c' opening bracket.  I have only handled the case
    // of only square brackets so far.  I basically need to create a new
    // JSONObject every time I see an opening curly or a square curly,
    // and then its full substring needs to be subtracted from the 
    // string of the JSONObject that spawned it.  So, I need to feed ONLY
    // the string that directly belongs to the new JSONObject, and then 
    // immediately assign the string minus the substring to the parent string. 

    size_t charcount = 1;
    s += ',';
    char buffer[10000]{};
    jsf->get(buffer, 10000, c);

    size_t count = jsf->gcount();
    //cout << "last read was " << count << " characters\n";
    s += string(buffer, 0, count);

    if (was_exp(c))
        s += c;

    if (s.find('{') == string::npos)
        
    {
        size_t index = s.find('[', 1);
        if (index == string::npos) // then it's just an array of values
        {
            vector<JSONValue*> ret_initial;
            std::istringstream psarr(s);
            //cout << s;
            was_exp(psarr, '[');
            while (psarr)
            {
                psarr.get(buffer, 1000, ',');
                std::streamsize read_count = psarr.gcount();
                if ( was_exp(psarr, ',') || !psarr )
                {
                    
                    string slop;
                    JSONValue* jv = new JSONValue;
                    if (std::all_of(begin(buffer), begin(buffer) + psarr.gcount(),
                        [](const char c)
                        { return isspace(c) || isdigit(c); }))
                    {
                        //cout << "it's a number\n";
                        for (size_t i = 0; i != read_count; ++i)
                        {
                            if (!isspace(buffer[i]))
                            {
                                //cout << buffer[i];
                                slop.push_back(buffer[i]);
                            }


                        }
                        *jv = std::stoi(slop);
                        ret_initial.push_back(jv);
                    }
                    else
                    {
                        bool initial_space_skipped = false;
                        for (size_t i = 0; i != read_count; ++i)
                        {
                            if (!initial_space_skipped && isspace(buffer[i]))
                            {
                                if (!isspace(buffer[i + 1]))
                                    initial_space_skipped = true;
                                continue;
                            }
                            if (buffer[i] != '"')
                            {
                                slop.push_back(buffer[i]);
                            }
                        }
                        if (slop == "true")
                            *jv = bool(true);
                        else if (slop == "false")
                            *jv = bool(false);
                        else if (slop == "null")
                            *jv = 0;
                        else
                            *jv = slop;

                        ret_initial.push_back(jv);

                    }
              
                }
                else
                    //cout << "that's not good";
                    ;
            }
            was_exp(',');
            return new JSONObject(ret_initial);
        }

        // then we have nested square brackets

    }
    else if (c == '{' && s.find('{', 1) != string::npos)  // could be wrong I'm tired
    { 
        // then we have a nested curly bracket object 

        // probably return fix_nested something or other
    }
    char opener;
    if (c == ']')
        opener = '[';
    else
        opener = '{';

    size_t next_index = s.find(opener, 1);
    if (next_index == string::npos)
        //cout << "additional " << opener << " not found: " <<
        //next_index << "\n current string  ";
    //cout << s << '\n';
    if (next_index != string::npos)
    {
        //cout << "additional " << opener << " found at " << next_index << '\n';
        nested_same(opener, c, s);
    }

    // S should be the full open and close bracket of the type that opened it 
    // at this point. 
    was_exp(',');
    JSONObject* ret = new JSONObject(new std::istringstream(s));

    return ret;
}


JSONValue* JSONObject::get_next_value()
{
    std::streamsize read_size = jsf->gcount();

    JSONValue* ret = new JSONValue();
    JSONObject* objret = new JSONObject();
    char buffer[10000]{};
    string slop;
    jsf->get(buffer, 10000, ',');
    read_size = jsf->gcount();
    //cout << "next_value read_size: " << read_size << '\n';
    //cout << string(buffer, 0, read_size) << '\n';

    if (was_exp(','))  // this area could yield an array, or a nested object
    {
        if (std::all_of(begin(buffer), begin(buffer) + read_size, [](const char c)
            { return isspace(c) || isdigit(c); }))
        {
            //cout << "it's a number\n";
            for (size_t i = 0; i != read_size; ++i)
            {
                if (!isspace(buffer[i]))
                {
                    slop.push_back(buffer[i]);
                }

            }
            *ret = std::stoi(slop);
        }
        else
        {
            bool initial_space_skipped = false;

            for (size_t i = 0; i != read_size; ++i)
            {
                if (!initial_space_skipped && isspace(buffer[i]))
                {
                    if (!isspace(buffer[i + 1]))
                        initial_space_skipped = true;
                    continue;
                }

                slop.push_back(buffer[i]);


            }
            bool has_square = slop.find('[') != string::npos;
            bool has_curly = slop.find('{') != string::npos;
            if (slop == "true")
                *ret = bool(true);
            else if (slop == "false")
                *ret = bool(false);
            else if (slop == "null")
                *ret = 0;
            else if (has_curly || has_square)
            {  // this branch needs to push_back to obj_values at some point, then
                // return a null pointer 
                delete ret;  // this probably needs to be done in another function?
                ret = 0;
                if ((has_square && !has_curly) ||
                    (has_square && has_curly && slop.find('[') < slop.find('{')))
                {
                    objret = fix_nested(']', slop);
                    obj_values.push_back(objret);
                    return ret;
                }
                else
                {
                    objret = fix_nested('}', slop);
                    obj_values.push_back(objret);
                    return ret;
                }
                
            }
            else
                *ret = slop; // moved, was inside two lines above
        }
    }
    ////cout << ret << '\n';
    
    // if we're here, there was no nested object, only a value
    delete objret;
    objret = 0;
    obj_values.push_back(objret); // maintain matching indices
    return ret;
}

std::string JSONObject::get_next_key()
{
    char arr[10000]{};
    //cout << "getting next key\n";
    jsf->get(arr, 10000, ':'); // grab until ", leave on stream
    
    std::streamsize read_size = jsf->gcount();
    //cout << read_size << " gcount\n";
    was_exp(':');
    string ret;
    for (size_t i = 0; i != read_size; ++i)
    {

        //cout << arr[i];
        if (arr[i] != '"' && !isspace(arr[i]))
            ret.push_back(arr[i]);
    }
    //cout << '\n';
    //cout << "returning " << ret << " as next key\n";
    if (read_size == 0)
        fuck_off = true;
    return ret ;
}