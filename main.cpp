#include <iostream>
#include <fstream>
#include "json_object.h"

using std::cout;


int main()
{
    JSONObject bob("sample.json");

    JSONBase* bobp = &bob;

    //bob.print(cout);
    //cout << bob["totalHits"]->type() << '\n';

    for (const auto& key : bob.key_index())
        cout << key << '\n';

    cout << '\n';

    //auto steve = keyobj(bob["foods"]->at("16"));

    //for (const auto& key : steve.key_index())
    //    cout << key << '\n';

    //bob["foodSearchCriteria"]->print(cout);

    //cout << bob["foodSearchCriteria"]->at("requireAllWords")->type();

    //std::ofstream see_ess_vee("output.csv");

    //see_ess_vee << bob.to_csv();


}