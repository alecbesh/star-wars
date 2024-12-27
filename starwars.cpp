

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


void Starwars::get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    opterr = false;

    // use getopt to find command line options
    struct option longOpts[] = {{ "verbose", 0, nullptr, 'v' },
                                { "median", 0, nullptr, 'm' },
                                { "general-eval", 0, nullptr, 'g' },
                                { "watcher", 0, nullptr, 'w' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "vmgw", longOpts, &option_index)) != -1) {
        switch (option) {

            case 'v':
                verbose = true;
                break;

            case 'm':
                median = true;
                break;

            case 'g':
                general_eval = true;
                break;

            case 'w':
                watcher = true;
                break;
            
            // All short/long options not in the spec should result in program term. w non zero exit code
            default:
                exit(1);
        }
    }
}


void Starwars::read_input() {
    Deployment d;
    string str;
    char letter;
    getline(cin, str);
    cin >> str >> input_mode;
    cin >> str >> num_generals;
    cin >> str >> num_planets;
    planets_init(num_planets);
    if (general_eval) generals_init(num_generals);
    cout << "Deploying troops...\n";
    if (input_mode == "DL") {
        while (cin >> d.timestamp) {
            if (d.timestamp < current_timestamp) {
                cerr << "Invalid decreasing timestamp\n";
                exit(1);
            }
            if (d.timestamp != current_timestamp) {
                if (median) {
                    for (uint32_t i = 0; i < num_planets; ++i) {
                        if (planets[i].battle_occurred) {
                            cout << "Median troops lost on planet " << i << " at time " << current_timestamp 
                            << " is " << planets[i].median << ".\n";
                        }
                    }
                }
                current_timestamp = d.timestamp;
            }

            cin >> str;
            cin >> letter >> d.general_id; // going to have to take this out of jedi/sith PQ, instead put in generals<>
            if ((int)d.general_id < 0 || d.general_id >= num_generals) {
                cerr << "Invalid general ID\n";
                exit(1);
            }
            cin >> letter >> d.planet_num;
            if ((int)d.planet_num < 0 || d.planet_num >= num_planets) {
                cerr << "Invalid planet ID\n";
                exit(1);
            }

            cin >> letter >> d.force_sensitivity;
            if (d.force_sensitivity <= 0) {
                cerr << "Invalid force sensitivity level\n";
                exit(1);
            }

            cin >> letter >> d.num_troops;
            if (d.num_troops <= 0) {
                cerr << "Invalid number of troops\n";
                exit(1);
            }
            readID++;
            d.read_id = readID;
            if (str == "JEDI") {
                planets[d.planet_num].Jedi.push(d);
                if (general_eval) generals[d.general_id].NUM_JEDI += d.num_troops;
            }
            else if (str == "SITH") {
                planets[d.planet_num].Sith.push(d);
                if (general_eval) generals[d.general_id].NUM_SITH += d.num_troops;
            }
            if (general_eval) {
                generals[d.general_id].NUM_DEPLOYED += d.num_troops;
                generals[d.general_id].NUM_SURVIVE += d.num_troops;
            }
            if (watcher) watcher_mode(d, str);
            fight(d.planet_num);
        }
    }


    else if (input_mode == "PR") {
        stringstream ss;
        uint32_t seed;
        uint32_t num_deploys;
        uint32_t rate;

        cin >> str >> seed;
        cin >> str >> num_deploys;
        cin >> str >> rate;

        P2random::PR_init(ss, seed, num_generals, num_planets, num_deploys, rate);
        istream &inputStream = input_mode == "PR" ? ss : cin;
        
        while (inputStream >> d.timestamp >> str >> letter >> d.general_id >> letter
            >> d.planet_num >> letter >> d.force_sensitivity >> letter >> d.num_troops) {
            if (d.timestamp != current_timestamp) {
                if (median) {
                    for (uint32_t i = 0; i < num_planets; ++i) {
                        if (planets[i].battle_occurred) {
                            cout << "Median troops lost on planet " << i << " at time " << current_timestamp 
                            << " is " << planets[i].median << ".\n";
                        }
                    }
                }
                current_timestamp = d.timestamp;
            }
            readID++;
            d.read_id = readID;
            if (str == "JEDI") {
                planets[d.planet_num].Jedi.push(d);
                if (general_eval) generals[d.general_id].NUM_JEDI += d.num_troops;
            }
            else if (str == "SITH") {
                planets[d.planet_num].Sith.push(d);
                if (general_eval) generals[d.general_id].NUM_SITH += d.num_troops;
            }
            if (general_eval) {
                generals[d.general_id].NUM_DEPLOYED += d.num_troops;
                generals[d.general_id].NUM_SURVIVE += d.num_troops;
            }

            if (watcher) watcher_mode(d, str);
            fight(d.planet_num);
        }
    }


    if (median) {
        for (uint32_t i = 0; i < num_planets; ++i) {
            if (planets[i].battle_occurred) {
                cout << "Median troops lost on planet " << i << " at time " << current_timestamp 
                    << " is " << planets[i].median << ".\n";
            }
        }
    }
}


void Starwars::fight(uint32_t i) {
        if (!(planets[i].Sith.empty()) && !(planets[i].Jedi.empty()) &&
                planets[i].Sith.top().force_sensitivity >= planets[i].Jedi.top().force_sensitivity) {
            num_battles++;
            // If Sith Wins:
            if (planets[i].Sith.top().num_troops > planets[i].Jedi.top().num_troops) {
                planets[i].Sith.top().num_troops -= planets[i].Jedi.top().num_troops;
                if (general_eval) {
                    generals[planets[i].Sith.top().general_id].NUM_SURVIVE -= planets[i].Jedi.top().num_troops;
                    generals[planets[i].Jedi.top().general_id].NUM_SURVIVE -= planets[i].Jedi.top().num_troops;
                }
                if (verbose) {
                    cout << "General " << planets[i].Sith.top().general_id << 
                        "'s battalion attacked General " << planets[i].Jedi.top().general_id <<
                        "'s battalion on planet " << i << ". " << (2*planets[i].Jedi.top().num_troops) <<
                        " troops were lost.\n";
                }
                if (median) calculate_median(i, 2*planets[i].Jedi.top().num_troops);
                planets[i].Jedi.pop();
            }
            // If Jedi wins:
            else if (planets[i].Sith.top().num_troops < planets[i].Jedi.top().num_troops) {
                planets[i].Jedi.top().num_troops -= planets[i].Sith.top().num_troops;
                if (general_eval) {
                    generals[planets[i].Sith.top().general_id].NUM_SURVIVE -= planets[i].Sith.top().num_troops;
                    generals[planets[i].Jedi.top().general_id].NUM_SURVIVE -= planets[i].Sith.top().num_troops;
                }
                if (verbose) {
                    cout << "General " << planets[i].Sith.top().general_id << 
                        "'s battalion attacked General " << planets[i].Jedi.top().general_id <<
                        "'s battalion on planet " << i << ". " << (2*planets[i].Sith.top().num_troops) <<
                        " troops were lost.\n";
                }
                if (median) calculate_median(i, 2*planets[i].Sith.top().num_troops);
                planets[i].Sith.pop();
            }
            else {
                if (verbose) {
                    cout << "General " << planets[i].Sith.top().general_id << 
                        "'s battalion attacked General " << planets[i].Jedi.top().general_id <<
                        "'s battalion on planet " << i << ". " << (2*planets[i].Sith.top().num_troops) <<
                        " troops were lost.\n";
                }
                if (median) calculate_median(i, 2*planets[i].Sith.top().num_troops);
                if (general_eval) {
                    generals[planets[i].Sith.top().general_id].NUM_SURVIVE -= planets[i].Jedi.top().num_troops;
                    generals[planets[i].Jedi.top().general_id].NUM_SURVIVE -= planets[i].Jedi.top().num_troops;
                }
                planets[i].Jedi.pop();
                planets[i].Sith.pop();
            }
            planets[i].battle_occurred = true;
            if (!(planets[i].Sith.empty()) && !(planets[i].Jedi.empty()) &&
                planets[i].Sith.top().force_sensitivity >= planets[i].Jedi.top().force_sensitivity) {
                fight(i);
            }
        }
}



void Starwars::calculate_median(uint32_t planet_id, uint32_t num_lost) {
    // Adding next item to one of the PQs
    if (planets[planet_id].median_low.empty()) planets[planet_id].median_low.push(num_lost);
    else if (planets[planet_id].median_high.empty() && (num_lost > planets[planet_id].median_low.top())) {
        planets[planet_id].median_high.push(num_lost);
    }
    else if (num_lost < planets[planet_id].median_low.top()) {
        planets[planet_id].median_low.push(num_lost);
    }
    else planets[planet_id].median_high.push(num_lost);
    
    // Balancing the PQs
    // cout << planets[planet_id].median_high.size() << planets[planet_id].median_low.size(); // DEBUGGING
    if (!(planets[planet_id].median_low).empty() && ((planets[planet_id].median_low).size() > (planets[planet_id].median_high).size())) {
        while (((planets[planet_id].median_low).size() - (planets[planet_id].median_high).size()) > 1) {
            planets[planet_id].median_high.push(planets[planet_id].median_low.top());
            planets[planet_id].median_low.pop();
        }
    }
    if (!(planets[planet_id].median_high).empty() && ((planets[planet_id].median_high).size() > (planets[planet_id].median_low).size())) {
        while (((planets[planet_id].median_high).size() - (planets[planet_id].median_low).size()) > 1) {
            planets[planet_id].median_low.push(planets[planet_id].median_high.top());
            planets[planet_id].median_high.pop();
        }
    }

    // Calculate median
    if ((planets[planet_id].median_low).size() == (planets[planet_id].median_high).size()) {
        planets[planet_id].median = (planets[planet_id].median_high.top() + planets[planet_id].median_low.top()) / 2;
    }
    else if ((planets[planet_id].median_low).size() > (planets[planet_id].median_high).size()) {
        planets[planet_id].median = planets[planet_id].median_low.top();
    }
    else if ((planets[planet_id].median_high).size() > (planets[planet_id].median_low).size()) {
        planets[planet_id].median = planets[planet_id].median_high.top();
    }
}




void Starwars::watcher_mode(Deployment d, string depType) {

    // Attack:

    if (planets[d.planet_num].attack_current == State::Initial) {
        // First Jedi:
        if (depType == "JEDI") {
            planets[d.planet_num].attack_current = State::SeenOne;
            planets[d.planet_num].attack_best_jedi_fs = d.force_sensitivity;
            planets[d.planet_num].attack_best_jedi_time = d.timestamp;
        }
    }

    else if (planets[d.planet_num].attack_current == State::SeenOne) {
        if (depType == "JEDI") {
            // check for better jedi:
            if (d.force_sensitivity < planets[d.planet_num].attack_best_jedi_fs) {
                planets[d.planet_num].attack_best_jedi_fs = d.force_sensitivity;
                planets[d.planet_num].attack_best_jedi_time = d.timestamp;
            }
        }
        // First Sith:
        else if (depType == "SITH" && (d.force_sensitivity >= planets[d.planet_num].attack_best_jedi_fs)) {
            planets[d.planet_num].attack_current = State::SeenBoth;
            planets[d.planet_num].attack_best_sith_fs = d.force_sensitivity;
            planets[d.planet_num].attack_best_sith_time = d.timestamp;
        }
    }

    else if (planets[d.planet_num].attack_current == State::SeenBoth) {
        if (depType == "SITH") {
            // check for better sith:
            if (d.force_sensitivity > planets[d.planet_num].attack_best_sith_fs) {
                planets[d.planet_num].attack_best_sith_fs = d.force_sensitivity;
                planets[d.planet_num].attack_best_sith_time = d.timestamp;
            }
        }
        // Check for maybe better Jedi:
        else if (depType == "JEDI") {
            if (d.force_sensitivity < planets[d.planet_num].attack_best_jedi_fs) {
                planets[d.planet_num].attack_current = State::MaybeBoth;
                planets[d.planet_num].attack_maybe_best_fs = d.force_sensitivity;
                planets[d.planet_num].attack_maybe_best_time = d.timestamp;
            }
        }
    }

    else if (planets[d.planet_num].attack_current == State::MaybeBoth) {
        // Check for an even better Jedi:
        if (depType == "JEDI") {
            if (d.force_sensitivity < planets[d.planet_num].attack_maybe_best_fs) {
                planets[d.planet_num].attack_maybe_best_fs = d.force_sensitivity;
                planets[d.planet_num].attack_maybe_best_time = d.timestamp;
            }
        }
        // Check for better difference:
        else if (depType == "SITH") {
            if ((d.force_sensitivity >= planets[d.planet_num].attack_maybe_best_fs) && 
            (planets[d.planet_num].attack_best_sith_fs >= planets[d.planet_num].attack_best_jedi_fs)) {
                if ((d.force_sensitivity - planets[d.planet_num].attack_maybe_best_fs) > 
                    (planets[d.planet_num].attack_best_sith_fs - planets[d.planet_num].attack_best_jedi_fs)) {
                        planets[d.planet_num].attack_current = State::SeenBoth;
            
                        planets[d.planet_num].attack_best_jedi_fs = planets[d.planet_num].attack_maybe_best_fs;
                        planets[d.planet_num].attack_best_jedi_time = planets[d.planet_num].attack_maybe_best_time;

                        planets[d.planet_num].attack_best_sith_fs = d.force_sensitivity;
                        planets[d.planet_num].attack_best_sith_time = d.timestamp;
                }
            }
        }
    }



    // Ambush:

    if (planets[d.planet_num].ambush_current == State::Initial) {
        // First sith:
        if (depType == "SITH") {
            planets[d.planet_num].ambush_current = State::SeenOne;
            planets[d.planet_num].ambush_best_sith_fs = d.force_sensitivity;
            planets[d.planet_num].ambush_best_sith_time = d.timestamp;
        }
    }

    else if (planets[d.planet_num].ambush_current == State::SeenOne) {
        if (depType == "SITH") {
            // check for better sith:
            if (d.force_sensitivity > planets[d.planet_num].ambush_best_sith_fs) {
                planets[d.planet_num].ambush_best_sith_fs = d.force_sensitivity;
                planets[d.planet_num].ambush_best_sith_time = d.timestamp;
            }
        }
        // First jedi:
        else if (depType == "JEDI" && (d.force_sensitivity <= planets[d.planet_num].ambush_best_sith_fs)) {
            planets[d.planet_num].ambush_current = State::SeenBoth;
            planets[d.planet_num].ambush_best_jedi_fs = d.force_sensitivity;
            planets[d.planet_num].ambush_best_jedi_time = d.timestamp;
        }
    }

    else if (planets[d.planet_num].ambush_current == State::SeenBoth) {
        if (depType == "JEDI") {
            // check for better jedi:
            if (d.force_sensitivity < planets[d.planet_num].ambush_best_jedi_fs) {
                planets[d.planet_num].ambush_best_jedi_fs = d.force_sensitivity;
                planets[d.planet_num].ambush_best_jedi_time = d.timestamp;
            }
        }
        // Check for better sith:
        else if (depType == "SITH") {
            if (d.force_sensitivity > planets[d.planet_num].ambush_best_sith_fs) {
                planets[d.planet_num].ambush_current = State::MaybeBoth;
                planets[d.planet_num].ambush_maybe_best_fs = d.force_sensitivity;
                planets[d.planet_num].ambush_maybe_best_time = d.timestamp;
            }
        }
    }

    else if (planets[d.planet_num].ambush_current == State::MaybeBoth) {
        // Check for an even better sith:
        if (depType == "SITH") {
            if (d.force_sensitivity > planets[d.planet_num].ambush_maybe_best_fs) {
                planets[d.planet_num].ambush_maybe_best_fs = d.force_sensitivity;
                planets[d.planet_num].ambush_maybe_best_time = d.timestamp;
            }
        }
        // Check for better difference:
        else if (depType == "JEDI") {
            if ((planets[d.planet_num].ambush_maybe_best_fs - d.force_sensitivity) > 
                (planets[d.planet_num].ambush_best_sith_fs - planets[d.planet_num].ambush_best_jedi_fs)) {
                    planets[d.planet_num].ambush_current = State::SeenBoth;
        
                    planets[d.planet_num].ambush_best_sith_fs = planets[d.planet_num].ambush_maybe_best_fs;
                    planets[d.planet_num].ambush_best_sith_time = planets[d.planet_num].ambush_maybe_best_time;

                    planets[d.planet_num].ambush_best_jedi_fs = d.force_sensitivity;
                    planets[d.planet_num].ambush_best_jedi_time = d.timestamp;
                }
        }
    }
}








void Starwars::print_output() {
    cout << "---End of Day---\n";
    cout << "Battles: " << num_battles << "\n";
    if (general_eval) print_GenEval();
    if (watcher) print_watcher();
}

void Starwars::print_GenEval() {
    cout << "---General Evaluation---\n";
    for (uint32_t i = 0; i < num_generals; ++i) {
        cout << "General " << i << " deployed " << generals[i].NUM_JEDI << " Jedi troops and " << 
        generals[i].NUM_SITH << " Sith troops, and " << generals[i].NUM_SURVIVE << "/" << generals[i].NUM_DEPLOYED << 
        " troops survived.\n";
    }
}

void Starwars::print_watcher() {
    cout << "---Movie Watcher---\n";
    for (uint32_t i = 0; i < num_planets; ++i) {
        if (planets[i].ambush_current == State::Initial || planets[i].ambush_current == State::SeenOne) {
            cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time -1 and Jedi at time -1 with a force difference of 0.\n";
        }
        else {
            cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time " << 
            planets[i].ambush_best_sith_time << " and Jedi at time " << planets[i].ambush_best_jedi_time << 
            " with a force difference of " << (planets[i].ambush_best_sith_fs - planets[i].ambush_best_jedi_fs) << ".\n";
        }

        if (planets[i].attack_current == State::Initial || planets[i].attack_current == State::SeenOne) {
            cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time -1 and Sith at time -1 with a force difference of 0.\n";
        }
        else {
            cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time " << 
            planets[i].attack_best_jedi_time << " and Sith at time " << planets[i].attack_best_sith_time << 
            " with a force difference of " << (planets[i].attack_best_sith_fs - planets[i].attack_best_jedi_fs) << ".\n";
        }
    }
}