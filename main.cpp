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


    //std::ofstream see_ess_vee("output.csv");

    //see_ess_vee << bob.to_csv();

    bob.to_file("output.json");


}