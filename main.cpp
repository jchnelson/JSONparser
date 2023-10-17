#include <iostream>
#include <fstream>
#include <vector>
#include "json_object.h"

using std::cout;


int main()
{
    JSONObject bob("counter_query.json");

    //JSONBase* bobp = &bob;

    keyval(bob.at("totalHits")) = "steve";

    bob.to_file("counter_steve.json");

    //bob.print(cout);
    //cout << bob["totalHits"]->type() << '\n';

    //for (const auto& key : bob.key_index())
    //    cout << key << '\n';

    //cout << '\n';


    //std::ofstream see_ess_vee("output.csv");

    //see_ess_vee << bob.to_csv();

    //bob.to_file("output.json");



    /*std::vector<JSONValue*> steve = {new JSONValue(2), new JSONValue(4)};

    JSONObject bob("bob", steve);

    cout << &bob;

    bob.insert("bill", {1,4,3});

    cout << &bob;

    keyval(bob.at("bob")->at("0")) = 173;

    cout << &bob;

    bob.to_file("bobtest.json");*/

}