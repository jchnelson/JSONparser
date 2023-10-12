
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "json_object.h"

using std::cout;
using std::string;
using std::vector;
using std::begin;
using std::end;


int main()
{

    JSONObject bob("counter_query.json");

    cout << bob.get_keys().size() << '\n';
    cout << bob.get_values().size() << '\n';


    size_t objects_count = count_if(bob.get_nested().cbegin(), bob.get_nested().cend(),
        [] ( const JSONObject* j) { return j != 0; });

    size_t values_count = count_if(bob.get_values().cbegin(), bob.get_values().cend(),
        [](const JSONValue* jv) { return jv != 0; });

    cout << objects_count << '\n' << values_count << '\n';

    for (const auto& key : bob.get_keys())
        cout << key.first << '\n';

    cout << bob.get_keys()[3].first << '\n';

    if (bob.get_nested()[3] != 0)
        cout << bob.get_nested()[3]->get_values().size() << '\n';
    for (const auto& i : bob.get_nested()[3]->get_values())
        cout << *i << " ";

    cout << '\n';

    if (bob.get_nested()[4] != 0)
        cout << "four is object\n";
    if (bob.get_keys()[4].second == 'j')
        cout << "four is object\n";

    auto steve = *bob.get_nested()[5];

    cout << steve.get_keys().size() << '\n';
    for (const auto& key : steve.get_keys())
        cout << key.first << " " << key.second << '\n';


  // last value is a blank string and 
  // 0, investigate later ig
 
}