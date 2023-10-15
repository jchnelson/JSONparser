
#include <iostream>
#include <iomanip>
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

void print_object_info(JSONObject js)
{
    cout << "\n\n";
    size_t objects_count = count_if(js.get_nested().cbegin(), js.get_nested().cend(),
        [](const JSONObject* j) { return j != 0; });

    size_t values_count = count_if(js.get_values().cbegin(), js.get_values().cend(),
        [](const JSONValue* jv) { return jv != 0; });
    cout << "number of keys: " << js.get_keys().size() << '\n';
    cout << "objects : " << objects_count << '\n' << "values: " << values_count << "\n\n";

    for (size_t i = 0; i != js.get_keys().size(); ++i)
    {
        cout << std::setw(30) << std::left << js.get_keys()[i].first << "type:"
            << js.get_keys()[i].second << "    ";

        if (js.get_keys()[i].second == 'j')
            cout << "nested object has " << js.get_nested()[i]->get_keys().size()
            << " keys and " << count_if(js.get_nested()[i]->get_keys().cbegin(),
                js.get_nested()[i]->get_keys().cend(), []
                (std::pair<std::string, char> p) { return p.second == 'j'; })
            << " of those keys' values are objects\n";
        else
            cout << '\n';
    }
}


int main()
{

    JSONObject bob("counter_query.json");

    print_object_info(bob);

    auto steve = bob.geto(4);

    for (const auto& key : steve->get_keys())
        cout << key.first << " " << key.second << '\n';

    cout << *steve->get_values()[4];

    auto& foods = *bob.key_obj("foods");

    auto& nuts = *foods.geto(1)->key_obj("foodNutrients");

    print_object_info(nuts);

 /*   auto steve = bob.geto(5);

    cout << "\n\nNEW OBJECT\n\n";

    print_object_info(*steve);

    auto& stever = *steve->get_nested().back();

    print_object_info(stever);

    auto& chris = *stever.get_nested()[11];

    auto& fml = chris.get_keys();
    auto& fmlv = chris.get_values();
    auto& fmlj = chris.get_nested();

    for (size_t i = 0; i != fml.size(); ++i)
    {

        cout << fml[i].first << '\n';
        for (size_t j = 0; j != fmlj[i]->get_keys().size(); ++j )
            cout << fmlj[i]->get_keys()[j].first << " : " 
                 << *fmlj[i]->get_values()[j] << '\n';

    }*/

    //print_object_info(chris);

    //for (const auto& obj : stever)
    //{
    //    if (obj != 0)
    //    {
    //        print_object_info(*obj);
    //    }
    //}


  // last value is a blank string and 
  // 0, investigate later ig
 
}