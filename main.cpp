
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

  // last value is a blank string and 
  // 0, investigate later ig
 
}