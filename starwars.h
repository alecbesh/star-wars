

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

using namespace std;

class Starwars {
    public:

    struct Deployment {
        
        uint32_t timestamp;
        uint32_t general_id;
        uint32_t planet_num; // might not need this
        uint32_t force_sensitivity;
        mutable uint32_t num_troops;
        uint32_t read_id;
    };






    class JediCOMP {
        public:
        // comparator a min-heap for the jedi's
        bool operator()(const Deployment &d1, const Deployment &d2) {
            if (d1.force_sensitivity > d2.force_sensitivity) return true;
            else if (d1.force_sensitivity < d2.force_sensitivity) return false;
            else {
                return d1.read_id > d2.read_id;
            }
        }
    };


    class SithCOMP {
        public:
        // comparator for a max-heap for the Siths
        bool operator()(const Deployment &d1, const Deployment &d2) {
            if (d1.force_sensitivity < d2.force_sensitivity) return true;
            else if (d1.force_sensitivity > d2.force_sensitivity) return false;
            else {
                return d1.read_id > d2.read_id;
            }
        }
    };

    enum class State {Initial, SeenOne, SeenBoth, MaybeBoth};

    struct Planet {
        public:
        std::priority_queue<Deployment, std::vector<Deployment>, JediCOMP> Jedi;
        std::priority_queue<Deployment, std::vector<Deployment>, SithCOMP> Sith;

        std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> median_high;
        std::priority_queue<uint32_t> median_low;
        
        State ambush_current = State::Initial;
        State attack_current = State::Initial;
        uint32_t attack_best_jedi_time;
        uint32_t attack_best_sith_time;
        uint32_t attack_best_jedi_fs;
        uint32_t attack_best_sith_fs;
        uint32_t attack_maybe_best_time;
        uint32_t attack_maybe_best_fs;

        
        uint32_t ambush_best_jedi_time;
        uint32_t ambush_best_sith_time;
        uint32_t ambush_best_jedi_fs;
        uint32_t ambush_best_sith_fs;
        uint32_t ambush_maybe_best_time;
        uint32_t ambush_maybe_best_fs;
        uint32_t median;

        bool battle_occurred = false;
    };


    struct General {
        uint32_t NUM_JEDI = 0;
        uint32_t NUM_SITH = 0;
        uint32_t NUM_SURVIVE = 0;
        uint32_t NUM_DEPLOYED = 0;
    };



    void get_options(int argc, char** argv);
    void read_input();
    void fight(uint32_t i);
    void print_output();
    void print_GenEval();
    void print_watcher();

    void planets_init(uint32_t num_planets_in) {
        planets.resize(num_planets_in);
    }
    void generals_init(uint32_t num_generals_in) {
        generals.resize(num_generals_in);
    }

    void calculate_median(uint32_t planet_id, uint32_t num_lost);
    void watcher_mode(Deployment d, string depType);






    private:

    vector<Planet> planets;
    vector<General> generals;
    string input_mode;
    uint32_t num_generals;
    uint32_t num_planets;
    uint32_t current_timestamp = 0;
    uint32_t readID = 0;
    uint32_t num_battles = 0;

    bool verbose = false;
    bool median = false;
    bool general_eval = false;
    bool watcher = false;
};