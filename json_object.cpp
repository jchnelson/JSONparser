
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "json_object.h"
#include "json_value.h"


using std::string;
using std::vector;
using std::cout;

bool zero_count = false;

std::ofstream outlog("log.txt");

JSONBase* JSONObject::operator[](const std::string& s)
{
    return valmap.at(s);
}
JSONBase* JSONObject::at(const std::string& s)
{
    return valmap.at(s);
}

std::ostream& JSONObject::print(std::ostream& os)
{

    size_t objects_count = count_if(begin(), end(),
        [](const std::pair<std::string, JSONBase*> p)
        { return p.second->type() == 'j'; });

    size_t values_count = count_if(begin(), end(),
        [](const std::pair<std::string, JSONBase*> p)
        { return p.second->type() != 'j'; });
    os << "number of keys: " << keyindex.size() << '\n';
    os << "objects : " << objects_count << '\n' << "values: "
        << values_count << "\n\n";

    for (size_t i = 0; i != keyindex.size(); ++i)
    {
        os << std::setw(20) << std::left << keyindex[i] << "type: "
            << type() << "  \n";
        valmap[keyindex[i]]->print(os);
        os << '\n';
    }
    return os;
}

JSONObject::JSONObject(const std::string fn)
    : jsf(new std::fstream(fn))
{
    if (jsf->get() != '{')
        throw std::runtime_error("Invalid JSON");

    while (jsf)
    {
        string newkey = get_next_key(*jsf);
        auto newvalue = get_next_value(*jsf);
        valmap.insert({ newkey, newvalue });
        keyindex.push_back(newkey);

        if (none_of(keyindex.back().cbegin(), keyindex.back().cend(), []
        (const char& c) { return isalpha(c) || isdigit(c); }) || newvalue == 0)
        {
            keyindex.pop_back();
            valmap.erase(newkey);
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
        string newkey = get_next_key(*jsf);
        auto newvalue = get_next_value(*jsf);
        valmap.insert({ newkey, newvalue });
        keyindex.push_back(newkey);

        if (none_of(keyindex.back().cbegin(), keyindex.back().cend(), []
        (const char& c) { return isalpha(c) || isdigit(c); }) || newvalue == 0)
        {
            keyindex.pop_back();
            valmap.erase(newkey);
        }
        if (zero_count)
            break;
    }
    delete jsf;
    jsf = 0;
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
    size_t next_index = s.find(open);
    size_t count = 1;  // non-zero just to enter condition
    while (count != 0 && charcount > 0)
    {
        is.get(buffer, 10000, close); // to next close
        count = is.gcount();
        string bob = string(buffer, 0, count); 
        size_t bobcount = std::count(bob.begin(), bob.end(), open);
        charcount += bobcount;
        size_t prevsize = s.size();
        s += bob;

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

JSONBase* JSONObject::fix_nested(std::istream& is, const char c, string& s)
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
            if (count(s.begin(), s.end(), '[') != count(s.begin(), s.end(), ']'))
                nested_same(is, '[', ']', s);

            if (s.find(',') == string::npos)  // then its one value in square brackets
                                              // for some reason
            {    
                JSONBase* oneval;
                s = s.substr(s.find('['));
                s.erase(0,1);
                s.erase(s.find(']'),1);
                string noquotes;
                for (const auto& ch : s)
                    if (ch != '"' && ch != '\n')
                        noquotes.push_back(ch);
                oneval = new JSONValue(noquotes);
                return oneval;
            }
            JSONObject* ret_initial = new JSONObject();
            s.insert(s.begin() + origsize, ',');
            s = s.substr(s.find('[')+1);
            std::istringstream psarr(s);
            int pos = 0;
            while (psarr)
            {
                psarr.get(buffer, 10000, ',');
                std::streamsize read_count = psarr.gcount();
                was_exp(psarr, ',');

                string slop;
                JSONBase* jv;
                string pairstring = std::to_string(pos);
                if ( std::all_of(std::begin(buffer), std::begin(buffer) + read_count,
                    [](const char c)
                    { return isspace(c) ||  ispunct(c) || c == ']'; }))
                {
                    jv = new JSONValue("Empty Brackets");
                }

                else if (std::all_of(std::begin(buffer), std::begin(buffer) + read_count,
                    [](const char c)
                    { return isspace(c) || isdigit(c) 
                        || c == ']' || c == '.' || c =='}'; }))
                {
                    for (size_t i = 0; i != read_count; ++i)
                    {
                        if (!isspace(buffer[i]) && buffer[i] != ']' && buffer[i] != '\n')
                        {
                            slop.push_back(buffer[i]);
                        }
                    }
                    if (slop.find('.') != string::npos)
                    {
                        //outlog << "argument to stod was:  " << slop << '\n';
                        jv = new JSONValue(std::stod(slop));
                    }
                    else
                    {
                        //outlog << "argument to stoi was:  " << slop << '\n';
                        jv = new JSONValue(std::stoi(slop));
                    }
                }
                else
                {
                    bool initial_space_skipped = false;
                    for (size_t i = 0; i != read_count; ++i)
                    {
                        if (buffer[i] != '"' && buffer[i] != ']' 
                            && buffer[i] != '}' && buffer[i] != '\n')
                        {
                            slop.push_back(buffer[i]);
                        }
                    }
                    if (slop.find("true") != string::npos)
                    {
                        string the_rest(slop);
                        the_rest.erase(the_rest.find("true"), 4);
                        if (all_of(the_rest.cbegin(), the_rest.cend(), [] 
                        (const char& c) { return isspace(c) || c == ']' 
                            || c == '}' || !isalpha(c) || !isdigit(c); } ))
                        {
                            JSONValue* jb = new JSONValue();
                            jb->b_eq(true);
                            jv = jb;
                        }
                    }
                    else if (slop.find("false") != string::npos)
                    {
                        string the_rest(slop);
                        the_rest.erase(the_rest.find("false"), 5);
                        if (all_of(the_rest.cbegin(), the_rest.cend(), []
                        (const char& c) { return isspace(c) || c == ']'
                            || c == '}' || !isalpha(c) || !isdigit(c); }))
                        {
                            JSONValue* jb = new JSONValue();
                            jb->b_eq(false);
                            jv = jb;
                        }
                    }
                    else if (slop == "null")
                        jv = new JSONValue(0);
                    else
                        jv = new JSONValue(slop);
                }
                ret_initial->valmap.insert({ pairstring, jv });
                ret_initial->keyindex.push_back(pairstring);
                ++pos;
            }
            was_exp(is, ',');
            return ret_initial;
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
            int pos = 0;
            while (prf)
            { 
                prf.get(buffer, 10000, ',');
                size_t read_count = prf.gcount();
                string sub(buffer, 0, read_count);
                string pairstring = std::to_string(pos);
                nestret->valmap.insert({pairstring, fix_nested(prf, '}', sub)});
                nestret->keyindex.push_back(pairstring);
                ++pos;        
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


JSONBase* JSONObject::get_next_value(std::istream& is)
{
    JSONBase* ret = 0;
    char buffer[10000]{};
    string slop;
    is.get(buffer, 10000, ',');
    std::streamsize read_size = jsf->gcount();
    //outlog << "next_value read_size: " << read_size << '\n';
    string tester(buffer, 0, read_size);
    if (tester.find('"') != string::npos && 
        count(tester.begin(), tester.end(), '"') == 1)
    {    // if quote index is not npos, and there is only one, grab until next quote
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
    was_exp(is, ','); // this area could yield an array, or a nested object
    was_exp(is, ','); // why are there two sometimes????
    if (std::all_of(std::begin(buffer), std::begin(buffer) + read_size,
        [](const char c)
        { return isspace(c) || ispunct(c) || c == ']'; }))
    {
        string empty = "empty brackets";
        ret = new JSONValue(empty);
    }
    else if (std::all_of(std::begin(buffer), std::begin(buffer) + read_size,
        [](const char c)
        { return isspace(c) || isdigit(c) 
            || c == ']' || c == '.' || c == '}';}))
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
            ret = new JSONValue(std::stod(slop));
        }
        else
        {
            outlog << "argument to stoi was:  " << slop << '\n';
            ret = new JSONValue(std::stoi(slop));
        }
    }
    else
    {
        bool has_square = tester.find('[') != string::npos;
        bool has_curly = tester.find('{') != string::npos;
        if (has_curly || has_square)
        {  

            ret = fix_nested(is, ']', tester);
            return ret;               
        }
        else 
        {
            string noquotes;
            for (const auto& ch : tester)
                if (ch != '"' && ch != ']' && ch != '}' && ch != '\n')
                    noquotes += ch;
            if (noquotes.find("true") != string::npos)
            {
                string the_rest(noquotes);
                the_rest.erase(the_rest.find("true"), 4);
                if (all_of(the_rest.cbegin(), the_rest.cend(), []
                (const char& c) { return isspace(c) || c == ']'
                    || c == '}' || !isalpha(c) || !isdigit(c); }))
                {
                    JSONValue* jb = new JSONValue();
                    jb->b_eq(true);
                    ret = jb;
                }
            }
            else if (noquotes.find("false") != string::npos)
            {
                string the_rest(noquotes);
                the_rest.erase(the_rest.find("false"), 5);
                if (all_of(the_rest.cbegin(), the_rest.cend(), []
                (const char& c) { return isspace(c) || c == ']'
                    || c == '}' || !isalpha(c) || !isdigit(c); }))
                {
                    JSONValue* jb = new JSONValue();
                    jb->b_eq(false);
                    ret = jb;
                }
            }
            else if (noquotes == "null")
                ret = new JSONValue(0);
            else
                ret = new JSONValue(noquotes);
        }
    }
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
        if (arr[i] != '"' && !isspace(arr[i]) && arr[i] != '\n')
            ret.push_back(arr[i]);
    }
    //outlog << '\n';
    //outlog << "returning " << ret << " as next key\n";
    if (ret.find("finalFoodInputFoods") != string::npos)
        cout << "bob";
    if (read_size == 0)
        zero_count = true;
    return ret ;
}

std::ostream& operator<<(std::ostream& os, JSONBase* jbp)
{
    jbp->print(os);
    return os;
}

JSONObject& keyobj(JSONBase* jsb)
{

    return *dynamic_cast<JSONObject*>(jsb);
}

JSONValue& keyval(JSONBase* jsb)
{
    return *dynamic_cast<JSONValue*>(jsb);
}

void out_object(std::ostringstream& fields, 
    std::ostringstream& records, JSONObject& job, const std::string& key)
{
    for (const auto& nextkey : job.key_index())
    {

        if (job.at(nextkey)->type() != 'j')
        {
            fields << key << "." << nextkey << ',';
            records << job.at(nextkey) << ',';
        }
        else
        {
            string forward = key + '.' + nextkey;
            out_object(fields, records, keyobj(job.at(nextkey)), forward );
        }
    }
}

std::string JSONObject::to_csv()
{
    std::ostringstream fields;
    std::ostringstream records;
    for (const auto& key : keyindex)
    { 
        
        if (at(key)->type() != 'j')
        {
            fields << key << ',';
            records << at(key) << ',';
        }
        else
        {
            out_object(fields, records, keyobj(at(key)), key);
        }
    }
    string field = fields.str();
    field.pop_back();
    string record = records.str();
    record.pop_back();
    return field + '\n' + record + '\n';
}
