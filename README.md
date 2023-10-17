JSON Parser
==================


This parser is meant to allow loading, manipulation,
and saving of JSON files for use with C++ projects.

## Initializing and Accessing Data from JSON


A JSONObject is comprised of JSONValues and JSONObjects, which both inherit from JSONBase/
A JSONValue is a managed union of int, double, bool, and std::string.

A file can be read by passing a filename, or an istream pointer to a dynamically-allocated 
istream, which will be deleted after it is used.


```cplusplus

JSONObject j("example.json");

std::string json;
JSONObject j2(new std::istringstream(json));

```

The data can then be accessed with the "at" member function, 
which returns a pointer to the base class JSONBase.  If 
example.json had contained the following data:

```
{
    "example": "text",
    "more": {
        "example": true
    }
}

```

then it could be accessed as follows:

```cplusplus

JSONObject j("example.json");

std::cout << j.at("example") << '\n';

bool is_example = j.at("more")->at("example")->get_bval();

```

Note that the output operator can be used with the pointer from the return
of at() directly.  This will output the value in a JSONValue pointed to, or 
when used with a JSONObject will show the entire nested structure.  This can
be a bit much when it's a large JSON, so be aware.  

It might be better to output a specific nested item if there is a 
deeply-nested structure in the JSON you're currently using. 

If you're looking for just the top-level information, try key_index() which 
will return a vector of the keys in insertion order. This vector is used 
to keep the correct order of the keys, and can be handy to access the structure
when used with at().  

Both JSONValue and JSONObject have the at() function available.  For a JSONObject
this will return the pointer for the object at that point in the structure,  but when  
called on a JSONValue, this will always return a null pointer as there is no key to access
within a single value.  This, or the type() member function, can be used to test 
whether a pointer is an object or a value in the JSON structure.  type() returns a char,
 'j' for object, 's' for string, 'i' for int, 'd' for double, and 'b' for bool.

```cplusplus

JSONObject j("example.json");

if (j.at("example")->type() == 's')
    ; // manipulate string 

if (j.at("example") != 0)
    ; // do JSONObject things

```


To access the nested structure of the JSON that was parsed, you 
need to know if you're accessing an "endpoint" of the JSON or 
another branching point where a nested object is beginning.

For example, again with the previous JSON structure:

```cplus

JSONObject j("example.json");

auto example = j.at("more")->at("example");   // example is a JSONValue that holds a bool.

std::cout << example;  // outputs true

auto object = keyobj(example); // this non-member function returns the underlying JSONObject,
			      // but will cause an error if the item is not a 
			      // JSON Object, as in this case.

auto value = keyval(example); // this is fine, value is a JSONValue.


auto more = keyobj(j.at("more"));  // more is a JSONObject with one key and one value.  

```

If the structure contains an array of objects or values (i.e. they don't have a key), this 
array can be referred to by a string indicating its zero-based index in the array.  For example:

```
{
    "example": [
        "bob",
        "steve",
        "frank"
    ]
}
```
given this JSON, we could access "steve" with:

```cplus

JSONObject j("example.json");

JSONBase* steve_value = j.at("example")->at("1");   // steve_value points to a JSONValue that holds "steve"

std::string steve;
if (steve_value->type() == 's')
	steve = keyval(steve_value).get_sval();

```

Please note that these indexes are for convenience of access and will not be written
back to a JSON.  They will, however, be written when csv output is used.  

get_sval(), get_ival(), get_dval(), and get_bval() can be used to retrieve the value from 
the JSONValue union, but you'll want to make sure you know the type that the union holds, or
check that the value is the type you think with type() first. 


## Adding or changing items in the structure

The insert() member function can be used to add items to the JSON structure.  Note
that the items will be added to the internal map as well as the vector index, so
the insertion order will still determine the order of the resulting JSON file's structure.  
The key_index() member function can be used to see this order or change it if you like, by 
sorting this vector in a different way.  


```cplusplus

JSONObject* bill = new JSONObject();

bill->insert("Bob", {1, 4, 5});  // initializer list must have all the same type
bill->insert("Steve", 3);  // any single acceptable type for JSONValue but bool (int, double, string)
bill->insert("Frank", "An Old Man");

JSONObject* bob = new JSONObject();

bob->insert("Bill", 3.45);
bob->insert("Steve", {"one", "four", "a lovely walk"});

std::vector<JSONBase*> another{bill, bob};

JSONObject toplevel_object;

toplevel_object.insert("this one", bill);
toplevel_object.insert("this other one", bob);

toplevel_object.to_file("readme_example.json");


```

results in the following JSON structure:

```
{
    "this one": {
        "Bob": [
            1,
            4,
            5
        ],
        "Steve": 3,
        "Frank": "An Old Man"
    },
    "this other one": {
        "Bill": 3.45,
        "Steve": [
            "one",
            "four",
            "a lovely walk"
        ]
    }
}

```

To create a JSONValue that holds a bool, use the b_eq() member function 

```cplusplus

JSONValue* jv = new JSONValue();
jv->b_eq(true);
std::cout << jv->type();  // will output 'b'

```


## Writing back to a file

A JSONObject has a map which is being accessed with the at() member function.  
It also has a vector that can be accessed with the key_index() member function. The
purpose of this vector is to preserve the file order of the key/value pairs, so that 
when the member function to_file(const std::string&) is called, the resulting JSON should look nearly 
identical to the original structure. 

to_file(const std::string&) is a member function that accepts a filename to write the json to.
Please be aware that if you write back to the same filename, the file will be overwritten, so
use with care.  This function has no return value. 

to_csv() is also available as a member function, but there is not yet any functionality for
parsing the resulting csv. This function returns a string.

Please note that to_csv() and to_file() are only available for JSONObject, so 
keyobj(JSONBase*) can be useful if you know an endpoint is a JSONObject.  
