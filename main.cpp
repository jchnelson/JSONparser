#include <iostream>
#include "json_object.h"

using std::cout;

int main()
{
    JSONObject bob("sample.json");

    print_object_info(bob);

    cout << bob.geto(0)->get_keys().size() << " keys in nested object\n";

    cout << *bob.key_obj("glossary")->key_obj("GlossDiv")->key_obj("GlossList")->key_obj("GlossEntry")->key_obj("GlossDef")->key_val("para");
}