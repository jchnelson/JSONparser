JSON Parser

==================


This parser is meant to allow loading, manipulation,
and saving of JSON files for use with C++ projects.

## Initializing and Accessing Data from JSON


A JSONObject is comprised of JSONValues and JSONObjects.
A JSONValue is a managed union of int, double, bool, and std::string.

A file can be read by passing a filename, or an istream pointer

```cplusplus

JSONObject j("example.json");

std::string json;
JSONObject j2(new std::istringstream(json));

```

The data can then be accessed with the "at" member function, 
which returns a pointer to the object/value.  If 
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

To access the nested structure of the JSON that was parsed, you 
need to know if you're accessing an "endpoint" of the JSON or 
another branching point where a nested object is beginning.

For example, again with the previous JSON structure:

```cplus

JSONObject j("example.json");

auto example = j.at("more")->at("example");

std::cout << example;

auto object = keyobj(example) // this non-member function returns the underlying JSONObject,
			     // but will cause an error if the item is not a 
			     // JSON Object, as in this case

auto value = keyval(example) // this is fine, value is a JSONValue.


auto more = keyobj(j.at("more"));  // more is a JSONObject with one key and one value.  

```

A JSONObject has a map which is being accessed with the at() member function.  
It also has a vector that can be accessed with the key_index() member function. The
purpose of this vector is to preserve the file order of the key/value pairs, so that 
when the member function to_file() is called, the resulting JSON should look nearly 
identical to the original structure. 
