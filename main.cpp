#include <iostream>
#include "json_object.h"

using std::cout;

int main()
{
    JSONObject bob("counter_query.json");

    JSONBase* bobp = &bob;

    //bob.print(cout);
    cout << bob["totalHits"]->type() << '\n';

    for (const auto& key : bob.key_index())
        cout << key << '\n';

    JSONObject* steve = dynamic_cast<JSONObject*>(bob["foods"]->at("1")->at("foodNutrients"));

    for (const auto& key : steve->key_index())
        cout << key << '\n';


}