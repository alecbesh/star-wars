

#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <getopt.h>
#include "P2random.h"
#include "starwars.h"

using namespace std;


int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);

    Starwars s;

    s.get_options(argc, argv);
    s.read_input();
    s.print_output();


    return 0;
}