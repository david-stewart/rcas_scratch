#include <vector>
#include <algorithm>
#include "bad_towers.h"
/* initialize the bad tower numbers */
std::vector<int> get_bad_towers(){
    std::vector<int> bad_towers = std::vector<int>{
        80000
    };
    std::sort(bad_towers.begin(), bad_towers.end());
    return bad_towers;
};
