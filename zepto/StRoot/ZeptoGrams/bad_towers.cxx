#include <vector>
#include <algorithm>
#include "bad_towers.h"
/* initialize the bad tower numbers */
std::vector<int> get_bad_towers(){
    std::vector<int> bad_towers = std::vector<int>{
		6, 7, 8, 12, 13, 14, 2196, 2758, 2779, 2798,
		2817, 2818, 2840, 2858, 2860, 2877, 2878
    };
    std::sort(bad_towers.begin(), bad_towers.end());
    return bad_towers;
};
