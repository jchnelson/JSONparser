#include <iostream>
#include <fstream>
#include <vector>
#include "json_object.h"

using std::cout;


int main()
{
    //JSONObject bob("counter_query.json");

    //JSONBase* bobp = &bob;

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

    JSONObject* bill = new JSONObject();

    bill->insert("Bob", { 1, 4, 5 });  // initializer list must have all the same type
    bill->insert("Steve", 3);  
    // any single acceptable type for JSONValue but bool (int, double, string)
    bill->insert("Frank", "An Old Man");

    JSONObject* bob = new JSONObject();

    bob->insert("Bill", 3.45);
    bob->insert("Steve", { "one", "four", "a lovely walk" });

    std::vector<JSONBase*> another{ bill, bob };

    JSONObject toplevel_object;

    toplevel_object.insert("this one", bill);
    toplevel_object.insert("this other one", bob);

    toplevel_object.to_file("readme_example.json");

    JSONValue* jvv = new JSONValue();
    jvv->b_eq(true);
    std::cout << jvv->type();

}