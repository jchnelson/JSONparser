
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#include "json_object.h"
#include "json_value.h"


using std::string;
using std::vector;
using std::begin;
using std::end;
using std::cout;

bool zero_count = false;

std::ofstream outlog("log.txt");

JSONObject::JSONObject(const std::string fn)
    : jsf(new std::fstream(fn))
{

    if (jsf->get() != '{')
        throw std::runtime_error("Invalid JSON");

    while (jsf)
    {
        keys.push_back({ get_next_key(*jsf), ' ' });  // placeholder char
        values.push_back(get_next_value(*jsf));
        if (values.back() == 0)
            keys.back().second = 'j';
        else
            keys.back().second = values.back()->type();
        if (none_of(keys.back().first.cbegin(), keys.back().first.cend(), []
        (const char& c) { return isalpha(c) || isdigit(c); }))
        {
            keys.pop_back();
            values.pop_back();
            obj_values.pop_back();
        }
        if (zero_count)
            break;

    }
    delete jsf;
    jsf = 0;
}


JSONObject::JSONObject(std::istream* js)
    : jsf(js)
{
    jsf->get(); // discard opening bracket 
    
    outlog << "Initializing new JSONObject from istream pointer\n";

    while (*jsf)
    {
        keys.push_back({ get_next_key(*jsf), ' ' });  // placeholder char
        values.push_back(get_next_value(*jsf));
        if (values.back() == 0)
            keys.back().second = 'j';
        else
            keys.back().second = values.back()->type();
        if (none_of(keys.back().first.cbegin(), keys.back().first.cend(), []
        (const char& c) { return isalpha(c) || isdigit(c); }))
        {
            keys.pop_back();
            values.pop_back();
            obj_values.pop_back();
        }
        if (zero_count)
            break;
    }
    delete jsf;
    jsf = 0;
}

JSONValue* JSONObject::getv(int i)
{
    if (values[i] != 0)
        return values[i];
    else
        return 0;
}
JSONValue* JSONObject::getv(int i , int j)
{
    if (obj_values[i] != 0)
        return obj_values[i]->get_values()[j];
    else
        return 0;
}
JSONValue* JSONObject::getv(int i, int j, int k)
{
    if (obj_values[i] != 0 && obj_values[i]->get_nested()[j] != 0)
        return obj_values[i]->get_nested()[j]->get_values()[k];
    else
        return 0;
}
JSONValue* JSONObject::getv(int i , int j , int k , int l)
{
    if (obj_values[i] != 0 && obj_values[i]->get_nested()[j] != 0
        && obj_values[i]->get_nested()[j]->get_nested()[k] != 0)

        return obj_values[i]->get_nested()[j]->get_nested()[k]->get_values()[l];
    else
        return 0;
}

JSONObject* JSONObject::geto(int i)
{
    return get_nested()[i];
}
JSONObject* JSONObject::geto(int i, int j)
{
    if (get_nested()[i] != 0)
        return get_nested()[i]->get_nested()[j];
    else 
        return 0;
}
JSONObject* JSONObject::geto(int i, int j, int k)
{
    if (get_nested()[i] != 0 && get_nested()[i]->get_nested()[j] != 0)
        return get_nested()[i]->get_nested()[j]->get_nested()[k];
    else
        return 0;
}
JSONObject* JSONObject::geto(int i, int j, int k, int l)
{
    if (get_nested()[i] != 0 && get_nested()[i]->get_nested()[j] != 0
        && get_nested()[i]->get_nested()[j]->get_nested()[k])

        return get_nested()[i]->get_nested()[j]->get_nested()[k]->get_nested()[l];
    else
        return 0;
}

JSONValue* JSONObject::key_val(const std::string& s)
{
    auto itempos = find_if(keys.cbegin(), keys.cend(), [&]
    (std::pair<std::string, char> key) { return key.first == s; });

    if (itempos != keys.cend())
    {
        size_t index = itempos - keys.cbegin();
        return values[index];
    }
    else
    {
        return 0;
    }
    
}

JSONObject* JSONObject::key_obj(const std::string& s)
{
    auto itempos = find_if(keys.cbegin(), keys.cend(), [&]
    (std::pair<std::string, char> key) { return key.first == s; });

    if (itempos != keys.cend())
    {
        size_t index = itempos - keys.cbegin();
        return obj_values[index];
    }
    else
    {
        return 0;
    }
}



bool JSONObject::was_exp(std::istream& is, const char c)
{
    char what = is.peek();
    if (is.peek() == c)
    {
        is.get();
        return true;
    }
    else
    {
        return false;
    }
}

void JSONObject::nested_same(std::istream& is, 
                             const char open, const char close, string& s)
{
    char buffer[10000]{};
    size_t charcount = std::count(s.begin(), s.end(), open);
    //outlog << "\nnumber of extra '" << open << "' : " << charcount << '\n';
    size_t next_index = s.find(open);  // next nested open bracket
    size_t count = is.gcount();
    while (count != 0 && charcount > 0)
    {
        is.get(buffer, 10000, close); // to next close
        count = is.gcount();
        string bob = string(buffer, 0, count); 
        size_t bobcount = std::count(bob.begin(), bob.end(), open);
        charcount += bobcount;
        size_t prevsize = s.size();
        s += bob;  // add section 

        if (was_exp(is, close))
        {
            //outlog << "expected closing bracket found\n";
            s += close; // add the close bracket
            --charcount;
            //outlog << "charcount before condition: " << charcount << "\n";

            if (charcount == 0)
            {
                outlog << "end of nested section reached, final string size was : " 
                    << s.size();
                break;
            }
        }

    }

}

JSONObject* JSONObject::fix_nested(std::istream& is, const char c, string& s)
{
    if (s.size() == 0)
        return 0;

    char buffer[10000]{};
    size_t origsize = s.size();

    if (s.find('{') == string::npos)
        
    {
        size_t index = s.find('[', s.find('[')+1);
        if (index == string::npos) // then it's just an array of values
        {
            vector<JSONValue*> ret_initial;


            if (count(s.begin(), s.end(), '[') != count(s.begin(), s.end(), ']'))
                nested_same(is, '[', ']', s);


            // if you find a close end bracket before a colon, it's one value.  

            if (s.find(',') == string::npos)  // then its one value in square brackets
                                              // for some reason
            {    
                JSONValue* oneval = new JSONValue();
                s = s.substr(s.find('['));
                s.erase(0,1);
                s.erase(s.find(']'),1);
                string noquotes;
                for (const auto& ch : s)
                    if (ch != '"')
                        noquotes.push_back(ch);
                *oneval = noquotes;
                ret_initial.push_back(oneval);
                return new JSONObject(ret_initial);
            }
            
            s.insert(s.begin() + origsize, ',');
            s = s.substr(s.find('[')+1);
            std::istringstream psarr(s);

            while (psarr)
            {
                psarr.get(buffer, 10000, ',');
                std::streamsize read_count = psarr.gcount();
                if ( was_exp(psarr, ',') || !psarr )
                {
                    string slop;
                    JSONValue* jv = new JSONValue();
                    if (std::all_of(begin(buffer), begin(buffer) + read_count,
                        [](const char c)
                        { return isspace(c) ||  ispunct(c) || c == ']'; }))
                    {
                        *jv = "empty brackets";
                    }

                    else if (std::all_of(begin(buffer), begin(buffer) + read_count,
                        [](const char c)
                        { return isspace(c) || isdigit(c) || c == ']' || c == '.'; }))
                    {
                        for (size_t i = 0; i != read_count; ++i)
                        {
                            if (!isspace(buffer[i]) && buffer[i] != ']')
                            {
                                slop.push_back(buffer[i]);
                            }
                        }
                        if (slop.find('.') != string::npos)
                        {
                            //outlog << "argument to stod was:  " << slop << '\n';
                            *jv = std::stod(slop);
                        }
                        else
                        {
                            //outlog << "argument to stoi was:  " << slop << '\n';
                            *jv = std::stoi(slop);
                        }
                        ret_initial.push_back(jv);
                    }
                    else
                    {
                        bool initial_space_skipped = false;
                        for (size_t i = 0; i != read_count; ++i)
                        {
                            if (buffer[i] != '"' && buffer[i] != ']')
                            {
                                slop.push_back(buffer[i]);
                            }
                        }
                        if (slop == "null")
                            *jv = 0;
                        else
                            *jv = slop;

                        ret_initial.push_back(jv);
                    }
                }
            }
            was_exp(is, ',');
            return new JSONObject(ret_initial);
        }

        // then we have nested square brackets and no curlies, I don't know 
        // if this even happens in a JSON?

    }
    else if (s.find('{') != string::npos)
    { 
        // then we have a nested curly bracket object 

        JSONObject* nestret = new JSONObject();
        if (s.find('[') < s.find('{'))
        {
               // new "file" to be transformed in place, 
               // set it up how it would be
            nested_same(is, '[', ']', s);
            s.insert(s.begin() + origsize, ',');
            s = s.substr(s.find('[') + 1);
            was_exp(is, ',');

            auto first_curly = s.find('{');
            string nester(s, first_curly); // discard beginning section with '['
            auto last_curly = nester.find_last_of('}');
            nester = nester.substr(0, last_curly+1); // discard section with ending ']'
            std::istringstream prf(nester); // "pretend file"
            while (prf)
            { 
                prf.get(buffer, 10000, ',');
                size_t read_count = prf.gcount();
                string sub(buffer, 0, read_count);
                
                nestret->obj_values.push_back(fix_nested(prf, '}', sub));
                if (nestret->obj_values.back()->keys.size() != 0)
                { 
                    nestret->keys.push_back({nestret->obj_values.back()->keys[0].first,
                        'j'});  // place keyname of first key to avoid unnamed objects
                }
                else
                { 
                    nestret->keys.push_back({"no keys in object", 'j'});
                }
            }
            return nestret;
        }
        else if (s.find('{') != string::npos)
        { 
            nested_same(is, '{', '}', s);
            was_exp(is, ',');
            s.insert(s.begin() + origsize, ',');
            s = s.substr(s.find('{'));  // discard space before '{'
            return new JSONObject(new std::istringstream(s));
        }
    }
    was_exp(is, ',');
    return new JSONObject(new std::istringstream(s));
}


JSONValue* JSONObject::get_next_value(std::istream& is)
{
    JSONValue* ret = new JSONValue();
    JSONObject* objret = new JSONObject();
    char buffer[10000]{};
    string slop;
    is.get(buffer, 10000, ',');
    std::streamsize read_size = jsf->gcount();
    //outlog << "next_value read_size: " << read_size << '\n';
    string tester(buffer, 0, read_size);
    if (tester.find('"') != string::npos && 
        count(tester.begin(), tester.end(), '"') == 1)
    {
        is.get(buffer, 10000, '"');
        auto newread = is.gcount();
        read_size += newread;
        tester += string(buffer, 0, newread);
        was_exp(is, '"');
        tester += '"';
        read_size += 1;
        if (is.peek() == 44)
            outlog << "comma inside quotes, fixed and added " << newread 
            << " characters to string\n\n" << tester;        
    }
    // if quote index is not npos, and there is only one, grab until next quote
    // and then make sure comma was_exp.

    if (was_exp(is, ','))  // this area could yield an array, or a nested object
    {
        if (std::all_of(begin(buffer), begin(buffer) + read_size,
            [](const char c)
            { return isspace(c) || ispunct(c) || c == ']'; }))
        {
            *ret = "empty brackets";
        }

        else if (std::all_of(begin(buffer), begin(buffer) + read_size,
            [](const char c)
            { return isspace(c) || isdigit(c) || c == ']' || c == '.'; }))
        {
            for (size_t i = 0; i != read_size; ++i)
            {
                if (!isspace(tester[i]))
                {
                    slop.push_back(tester[i]);
                }
            }
            if (slop.find('.') != string::npos)
            {
                outlog << "argument to stod was:  " << slop << '\n';
                *ret = std::stod(slop);
            }
            else
            {
                outlog << "argument to stoi was:  " << slop << '\n';
                *ret = std::stoi(slop);
            }
        }
        else
        {
            bool has_square = tester.find('[') != string::npos;
            bool has_curly = tester.find('{') != string::npos;
            if (has_curly || has_square)
            {  
                delete ret; // JSONValue not needed, only placeholder 
                ret = 0;
                objret = fix_nested(is, ']', tester);
                obj_values.push_back(objret);
                return ret;               
            }
            else 
            {
                string noquotes;
                for (const auto& ch : tester)
                    if (ch != '"' && ch != ']')
                        noquotes += ch;
                if (noquotes == "null")
                    *ret = 0;
                else
                    *ret = noquotes;
            }
        }
    }
    // if we're here, there was no nested object, only a value
    delete objret; // so the JSONObject is not needed
    objret = 0;
    obj_values.push_back(objret);
    return ret;
}

std::string JSONObject::get_next_key(std::istream& is)
{
    char arr[10000]{};
    //outlog << "getting next key\n";
    is.get(arr, 10000, ':');
    
    std::streamsize read_size = is.gcount();
    //outlog << read_size << " gcount\n";
    was_exp(is, ':');
    string ret;
    for (size_t i = 0; i != read_size; ++i)
    {

        outlog << arr[i];
        if (arr[i] != '"' && !isspace(arr[i]))
            ret.push_back(arr[i]);
    }
    //outlog << '\n';
    //outlog << "returning " << ret << " as next key\n";
    if (read_size == 0)
        zero_count = true;
    return ret ;
}


void print_object_info(JSONObject js)
{
    cout << "\n\n";
    size_t objects_count = count_if(js.get_nested().cbegin(), js.get_nested().cend(),
        [](const JSONObject* j) { return j != 0; });

    size_t values_count = count_if(js.get_values().cbegin(), js.get_values().cend(),
        [](const JSONValue* jv) { return jv != 0; });
    cout << "number of keys: " << js.get_keys().size() << '\n';
    cout << "non-null objects : " << objects_count << '\n' << "non-null values: " << values_count << "\n\n";

    for (size_t i = 0; i != js.get_keys().size(); ++i)
    {
        cout << std::setw(20) << std::left << js.get_keys()[i].first << "type:"
            << js.get_keys()[i].second << "    ";

        if (js.get_keys()[i].second == 'j')
            cout << "nested object has " << js.get_nested()[i]->get_keys().size()
            << " keys, " << js.get_nested()[i]->get_values().size()
            << " values and " << count_if(js.get_nested()[i]->get_keys().cbegin(),
                js.get_nested()[i]->get_keys().cend(), []
                (std::pair<std::string, char> p) { return p.second == 'j'; })
            << " of those keys' values are objects\n";
        else
            cout << '\n';
    }
    cout << '\n';
}