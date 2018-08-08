#include <vector>
#include <algorithm>
#include "bad_runs.h"
/* initialize the bad run numbers */
std::vector<int> get_bad_runs(){
    std::vector<int> bad_runs = std::vector<int>{16125024,16125035,16125038,16125039,16125050,
          16125052,16125053,16125054,16125056,16125057,16125058,16125060,16125061,
          16125062,16125063,16126010,16126011,16126012,16126013,16126014,16126015,
          16126016,16126017,16126018,16126030,16126043,16126044,16127005,16127014,
          16127015,16127016,16127048,16127049,16128010,16128011,16128012,16128047,
          16128048,16128051,16128056,16129003,16129005,16129015,16129019,16129022,
          16129025,16129046,16129047,16130012,16130015,16130016,16130032,16131026,
          16132001,16132046,16132047,16133085,16133086,16134012,16134042,16139054,
          16141036,16144037,16144069,16149001,16149002,16149003,16149004,16149005,
          16149007,16149008,16149009,16149010,16149011,16149013,16149014,16150001,
          16150003,16150010,16154010,16154021,16154022,16155017,16155031,16155039,
          16156010,16156028,16156030,16156031,16156032,16156033,16156034,16157059,
          16158015,16158021,16158039,16159009,16159010,16159019,
          // new bad runs:
          16150001,16154009,16155017,16155039,16156010,16158020
    };
    std::sort(bad_runs.begin(), bad_runs.end());
    return bad_runs;
};
