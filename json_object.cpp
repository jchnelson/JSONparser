
#include <vector>
#include <string>
#include <iostream>
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

bool fuck_off = false;

std::ofstream outlog("log.txt");

JSONObject::JSONObject(const std::string fn)
    : jsf(new std::fstream(fn))
{

    if (jsf->get() != '{')
        throw std::runtime_error("Invalid JSON");

    while (jsf)
    {
        keys.push_back({ get_next_key(*jsf), ' ' });  // placeholder char, change later
        values.push_back(get_next_value(*jsf));
        if (values.back() == 0)
            keys.back().second = 'j';
        else
            keys.back().second = values.back()->type();
        if (fuck_off)
            break;
    }
}


JSONObject::JSONObject(std::istream* js)
    : jsf(js)
{
    jsf->get(); // discard opening bracket if it exists?
    // previous function identified that there is at least some kind
    // of opening bracket before this.  Is it still useful to know what kind?
    
    outlog << "Initializing new JSONObject from istream pointer\n";

    while (*jsf)
    {
        keys.push_back({ get_next_key(*jsf), ' ' });  // placeholder char, change later
        values.push_back(get_next_value(*jsf));
        if (values.back() == 0)
            keys.back().second = 'j';
        else
            keys.back().second = values.back()->type();
        if (fuck_off)
            break;
    }
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



bool JSONObject::was_exp(std::istream& is, const char c)
{
    if (is.peek() == c)
    {
        is.get();
        //outlog << "removed expected " << c << '\n';
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
    //outlog << s << '\n';
    size_t charcount = std::count(s.begin(), s.end(), open);
    //outlog << "\nnumber of extra '" << open << "' : " << charcount << '\n';
    size_t next_index = s.find(open);  // next nested open bracket
    size_t count = is.gcount();
    while (count != 0 && charcount > 0)
    {
        is.get(buffer, 10000, close); // to next close
        count = is.gcount();
        if (count < 9999 && !was_exp(is, close))
            cout << "how the fuck";
        //outlog << count << " characters read onto string and charcount is"
            //<< charcount << "\n";
        string bob = string(buffer, 0, count);
        size_t bobcount = std::count(bob.begin(), bob.end(), open);
        charcount += bobcount;
        //outlog << bob << '\n';
        size_t prevsize = s.size();
        s += bob;  // add section 

        if (was_exp(is, close))
        {
            outlog << "expected closing bracket found\n";
            s += close; // add the close bracket
            --charcount;
            outlog << "charcount before condition: " << charcount << "\n";

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
    // so I have a string with brackets in it.  At this point I know only
    // that there is one 'c' opening bracket.  I have only handled the case
    // of only square brackets so far.  I basically need to create a new
    // JSONObject every time I see an opening curly or a square curly,
    // and then its full substring needs to be subtracted from the 
    // string of the JSONObject that spawned it.  So, I need to feed ONLY
    // the string that directly belongs to the new JSONObject, and then 
    // immediately assign the string minus the substring to the parent string. 
    size_t charcount = 1;
    char buffer[10000]{};
    size_t count = s.size();
    


    if (s.find('{') == string::npos)
        
    {
        //outlog << s;
        size_t index = s.find('[', s.find('[')+1);
        if (index == string::npos) // then it's just an array of values
        {
            vector<JSONValue*> ret_initial;

            // if you find a close end bracket before a colon, it's one value.  

            if (s.find(',') == string::npos)  // then its one value in square brackets
                                              // for some reason
            {    
                JSONValue* oneval = new JSONValue();
                s.pop_back(); // get rid of comma
                s.pop_back(); // get rid of the brackets
                s.erase(0);
                string noquotes;
                for (const auto& ch : s)
                    if (ch != '"')
                        noquotes.push_back(ch);
                *oneval = noquotes;
                ret_initial.push_back(oneval);
                return new JSONObject(ret_initial);
            }

            is.get(buffer, 10000, ']');
            s += string(buffer, 0, is.gcount());
            while (s[0] != '[')
                s.erase(0,1);
            std::istringstream psarr(s);
            psarr.get(buffer, 10000, '['); // discard space before opening bracket
            was_exp(psarr, '[');
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
                        { return isspace(c) || isdigit(c) || c == ']'; }))
                    {
                        //outlog << "it's a number\n";
                        for (size_t i = 0; i != read_count; ++i)
                        {
                            if (!isspace(buffer[i]) && buffer[i] != ']')
                            {
                                slop.push_back(buffer[i]);
                            }


                        }
                        outlog << "argument to stoi was " << slop << '\n';
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
                            if (buffer[i] != '"' && buffer[i] != ']')
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
                    outlog << "that's not good";
                    ;
            }
            was_exp(is, ']');
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
            
             // for stream, not to be changed, new "file"
               // to be transformed in place, set it up how it would be 
            nested_same(is, '[', ']', s);
            //if (s.find('[',1) != string::npos)
            //    nested_same(is, '[', ']', s);
            was_exp(is, ',');
            //was_exp(is, '[');
            // outlog << s;
            auto first_curly = s.find('{');

            string nester(s, first_curly);
            //outlog << nester;
            std::istringstream prf(nester); // "pretend file"
            while (prf)
            { 
                prf.get(buffer, 10000, ',');
                size_t read_count = prf.gcount();
                string sub(buffer, 0, count);
                fix_nested(prf, '}', sub);
                if (sub.find('{') != string::npos)
                {
                    nested_same(prf, '{', '}', sub);
                }
                //outlog << sub << '\n\n';
                while (sub[0] != '{')
                    sub.erase(0,1);
                std::istringstream* elemstream = new std::istringstream(sub);
                nestret->obj_values.push_back(new JSONObject(elemstream));

            }
            // get next {}
            return nestret;

            // create the string to be passed to another istringstream to 
            // initialize the next JSONObject

        }
        else if (s.find('{') != string::npos && 
            s.find('{', s.find('{') +1) == string::npos)
        {
            was_exp(is, ',');
            while (1)
            { 
                is.get(buffer, 10000, '}');
                s += string(buffer, 0, is.gcount());
                if (was_exp(is, '}'))
                    break;
            }
            was_exp(is,',');
            while (s[0] != '{')
                s.erase(0,1);
            return new JSONObject(new std::istringstream(s));
        }
        //else if (s[0] == '{' && s.find('{', 1) != string::npos)
        else if (s.find('{',s.find('{')+1) != string::npos)
        {
            outlog << "this is the new case";

            was_exp(is, ',');
            nested_same(is, '{', '}', s);
            //was_exp(is, ','); // is this necessary?
            //was_exp(is, '[');
            //outlog << s << '\n';
            while (s[0] != '{')
                s.erase(0, 1);
            std::istringstream* newstring = new std::istringstream(s);
            return new JSONObject(newstring);
            //auto first_curly = s.find('{');
            //const string nester(s, first_curly);
            //outlog << nester;
            //std::istringstream prf(nester); // "pretend file"
            //prf.get(buffer, 10000, ',');
            //size_t read_count = prf.gcount();
            //string ohgod(buffer, 0, count);
            
            //fix_nested(prf, '}', ohgod);
            
            //std::istringstream* newnest = new std::istringstream(ohgod);
            //nestret->obj_values.push_back(new JSONObject(newnest));
            //// get next {}
            //return nestret;
            
        }
        // probably return fix_nested something or other
    }
    //char opener;
    //if (c == ']')
    //    opener = '[';
    //else
    //    opener = '{';

    //size_t next_index = s.find(opener, 1);
    //if (next_index == string::npos)
    //    outlog << "additional " << opener << " not found: " <<
    //    next_index << "\n current string  ";
    //outlog << s << '\n';
    //if (next_index != string::npos)
    //{
    //    outlog << "additional " << opener << " found at " << next_index << '\n';
    //    nested_same(is, opener, c, s);
    //}

    // S should be the full open and close bracket of the type that opened it 
    // at this point. i.e. it can be used to parse nested objects even though it
    // didn't identify them
    was_exp(is, ',');
    JSONObject* ret = new JSONObject(new std::istringstream(s));

    return ret;
}


JSONValue* JSONObject::get_next_value(std::istream& is)
{
    

    JSONValue* ret = new JSONValue();
    JSONObject* objret = new JSONObject();
    char buffer[10000]{};
    string slop;
    is.get(buffer, 10000, ',');
    std::streamsize read_size = jsf->gcount();
    outlog << "next_value read_size: " << read_size << '\n';
    string tester(buffer, 0, read_size);
    //outlog << tester << '\n';
    if (tester.find('"') != string::npos && count(tester.begin(), tester.end(), '"') == 1)
    {
        is.get(buffer, 10000, '"');
        auto newread = is.gcount();
        read_size += newread;
        tester += string(buffer, 0, newread);
        if (is.peek() == 44)
            outlog << "comma inside quotes, fixed and added " << newread 
            << " characters to string\n\n" << tester;

        
    }

    

    // if quote index is not npos, and there is only one, grab until next quote
    // and then make sure comma was_exp.

    if (was_exp(is, ','))  // this area could yield an array, or a nested object
    {
        tester += ',';
        read_size += 1;
        
        if (std::all_of(begin(tester), begin(tester) + read_size, [](const char c)
            { return isspace(c) || isdigit(c); }))
        {
            //outlog << "it's a number\n";
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
                try {
                    *ret = std::stod(slop);
                }
                catch (std::exception e) {
                    outlog << e.what() << " after stod call";
                    throw;
                }
            }
            else
            {
                outlog << "argument to stoi was:  " << slop << '\n';
                try {
                *ret = std::stoi(slop);
                }
                catch (std::exception e) {
                    outlog << e.what() << " after stoi call";
                    throw;
                }
            }
        }
        else
        {
            bool has_square = tester.find('[') != string::npos;
            bool has_curly = tester.find('{') != string::npos;
            if (has_curly || has_square)
            {  // this branch needs to push_back to obj_values at some point, then
                // return a null pointer 
                delete ret;  // this probably needs to be done in another function?
                ret = 0;
                objret = fix_nested(is, ']', tester);
                obj_values.push_back(objret);
                return ret;               
            }
            else if (tester == "true")
                *ret = bool(true);
            else if (tester == "false")
                *ret = bool(false);
            else if (tester == "null")
                *ret = 0;
            else
                *ret = tester; // moved, was inside two lines above
        }
    }
    //outlog << ret << '\n';
    
    // if we're here, there was no nested object, only a value
    delete objret;
    objret = 0;
    obj_values.push_back(objret); // maintain matching indices
    return ret;
}

std::string JSONObject::get_next_key(std::istream& is)
{
    char arr[10000]{};
    outlog << "getting next key\n";
    is.get(arr, 10000, ':'); // grab until ", leave on stream
    
    std::streamsize read_size = is.gcount();
    outlog << read_size << " gcount\n";
    was_exp(is, ':');
    string ret;
    for (size_t i = 0; i != read_size; ++i)
    {

        outlog << arr[i];
        if (arr[i] != '"' && !isspace(arr[i]))
            ret.push_back(arr[i]);
    }
    outlog << '\n';
    outlog << "returning " << ret << " as next key\n";
    if (read_size == 0)
        fuck_off = true;
    return ret ;
}