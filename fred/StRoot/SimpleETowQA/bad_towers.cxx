#include <vector>
#include <algorithm>
#include "bad_towers.h"

// Note: all numbers are EmcTrigger numbers (Tower numbers +1)
// Bad towers: are from nHits (H) or from Energy_transverse(E)
// Total: 1092 bad towers:
//     N(H&E):  244
//     N(H&!E): 435
//     N(E&!H): 413

std::vector<int> get_bad_towers(){
    std::vector<int> bad_towers = std::vector<int>{ 
        0
// nHit && Et:  244 towers
          /* 114,  181,  275,  287,  308,  421,  508,  561,  681,  692, */
          /* 740,  768,  799,  822,  840,  860,  880,  882,  941,  961, */
         /* 1063, 1081, 1125, 1130, 1160, 1161, 1198, 1220, 1221, 1223, */
         /* 1232, 1238, 1244, 1257, 1280, 1294, 1427, 1440, 1728, 1823, */
         /* 1838, 1879, 1921, 1984, 2021, 2022, 2023, 2024, 2025, 2026, */
         /* 2027, 2028, 2029, 2030, 2033, 2035, 2037, 2039, 2040, 2041, */
         /* 2042, 2043, 2044, 2045, 2046, 2047, 2048, 2049, 2050, 2051, */
         /* 2052, 2053, 2055, 2058, 2060, 2061, 2062, 2063, 2064, 2065, */
         /* 2066, 2067, 2068, 2069, 2070, 2071, 2072, 2074, 2076, 2077, */
         /* 2078, 2079, 2080, 2081, 2082, 2083, 2084, 2085, 2086, 2087, */
         /* 2088, 2089, 2090, 2091, 2092, 2093, 2100, 2101, 2102, 2103, */
         /* 2105, 2106, 2107, 2108, 2109, 2110, 2111, 2112, 2115, 2118, */
         /* 2121, 2122, 2123, 2124, 2125, 2126, 2127, 2128, 2129, 2130, */
         /* 2131, 2132, 2135, 2137, 2138, 2139, 2140, 2141, 2143, 2145, */
         /* 2146, 2147, 2148, 2149, 2150, 2151, 2152, 2153, 2157, 2158, */
         /* 2159, 2161, 2162, 2163, 2164, 2165, 2166, 2167, 2168, 2169, */
         /* 2170, 2171, 2172, 2173, 2174, 2175, 2176, 2177, 2178, 2180, */
         /* 2202, 2223, 2300, 2340, 2521, 2561, 2580, 2581, 2633, 2652, */
         /* 2758, 2779, 2792, 2798, 2801, 2808, 2817, 2818, 2828, 2840, */
         /* 2858, 2860, 2861, 2877, 2878, 2901, 2902, 2903, 2904, 2921, */
         /* 2922, 2923, 2924, 2941, 2942, 2943, 2944, 2961, 2963, 2964, */
         /* 2981, 2982, 2983, 2984, 3002, 3003, 3020, 3022, 3023, 3024, */
         /* 3041, 3042, 3043, 3044, 3241, 3361, 3508, 3588, 3604, 3611, */
         /* 3692, 3741, 3761, 3781, 3838, 3861, 4013, 4141, 4401, 4421, */
         /* 4464, 4621, 4766, 4781, */
/* // Only nHit: 435 towers */
          /*   8,   22,   30,   48,   50,   51,   59,   82,   88,  103, */
          /* 122,  128,  145,  157,  162,  168,  170,  188,  208,  214, */
          /* 237,  245,  246,  251,  261,  285,  288,  316,  327,  328, */
          /* 338,  348,  349,  385,  405,  468,  478,  479,  493,  502, */
          /* 507,  513,  526,  528,  536,  537,  542,  551,  555,  570, */
          /* 571,  576,  579,  583,  665,  671,  691,  704,  745,  750, */
          /* 759,  772,  774,  776,  779,  788,  794,  801,  802,  808, */
          /* 815,  826,  835,  839,  885,  895,  897,  912,  923,  926, */
          /* 927,  936,  946,  957,  972,  982,  986,  988,  990,  998, */
         /* 1003, 1007, 1014, 1022, 1029, 1030, 1042, 1044, 1050, 1062, */
         /* 1067, 1072, 1074, 1076, 1078, 1083, 1084, 1085, 1088, 1089, */
         /* 1096, 1108, 1109, 1120, 1126, 1128, 1131, 1137, 1142, 1143, */
         /* 1145, 1146, 1147, 1148, 1151, 1153, 1154, 1162, 1166, 1176, */
         /* 1183, 1185, 1192, 1196, 1206, 1213, 1227, 1233, 1239, 1245, */
         /* 1248, 1250, 1256, 1263, 1268, 1282, 1302, 1304, 1306, 1313, */
         /* 1318, 1338, 1343, 1375, 1406, 1435, 1456, 1475, 1502, 1537, */
         /* 1542, 1564, 1565, 1592, 1606, 1622, 1630, 1645, 1662, 1665, */
         /* 1683, 1691, 1707, 1709, 1726, 1730, 1735, 1759, 1768, 1789, */
         /* 1809, 1825, 1867, 1873, 1880, 1882, 1887, 1892, 1904, 1906, */
         /* 1909, 1912, 1935, 1938, 1943, 1945, 1976, 1983, 1986, 2190, */
         /* 2250, 2267, 2272, 2288, 2299, 2303, 2313, 2322, 2327, 2339, */
         /* 2368, 2378, 2383, 2388, 2403, 2407, 2423, 2424, 2433, 2439, */
         /* 2453, 2459, 2476, 2486, 2493, 2504, 2506, 2507, 2510, 2520, */
         /* 2522, 2529, 2539, 2548, 2562, 2570, 2573, 2585, 2591, 2607, */
         /* 2618, 2624, 2644, 2649, 2679, 2687, 2700, 2703, 2706, 2709, */
         /* 2712, 2723, 2750, 2773, 2812, 2825, 2842, 2844, 2863, 2868, */
         /* 2874, 2889, 2890, 3006, 3007, 3030, 3055, 3063, 3072, 3084, */
         /* 3092, 3093, 3099, 3100, 3102, 3110, 3112, 3137, 3139, 3143, */
         /* 3147, 3150, 3154, 3160, 3164, 3171, 3175, 3185, 3206, 3220, */
         /* 3225, 3238, 3250, 3259, 3264, 3266, 3271, 3272, 3277, 3292, */
         /* 3303, 3319, 3330, 3336, 3349, 3365, 3370, 3373, 3375, 3377, */
         /* 3378, 3385, 3390, 3392, 3396, 3397, 3404, 3406, 3419, 3420, */
         /* 3439, 3451, 3453, 3462, 3483, 3485, 3507, 3509, 3513, 3515, */
         /* 3528, 3532, 3544, 3547, 3568, 3573, 3579, 3580, 3582, 3589, */
         /* 3593, 3596, 3599, 3622, 3627, 3629, 3630, 3643, 3649, 3655, */
         /* 3659, 3666, 3667, 3677, 3682, 3711, 3720, 3726, 3737, 3747, */
         /* 3749, 3753, 3764, 3790, 3793, 3802, 3821, 3827, 3831, 3834, */
         /* 3868, 3876, 3886, 3912, 3913, 3932, 3933, 3964, 3978, 4024, */
         /* 4027, 4050, 4058, 4063, 4064, 4106, 4122, 4130, 4136, 4165, */
         /* 4166, 4184, 4209, 4232, 4233, 4249, 4252, 4302, 4335, 4353, */
         /* 4359, 4386, 4389, 4392, 4399, 4430, 4438, 4440, 4453, 4460, */
         /* 4472, 4480, 4498, 4499, 4518, 4530, 4534, 4546, 4563, 4566, */
         /* 4595, 4598, 4602, 4638, 4647, 4657, 4699, 4708, 4713, 4722, */
         /* 4727, 4728, 4746, 4778, 4793, */
/* // Only Et:  413 towers */
          /*   0,   31,   34,   35,   95,  106,  113,  134,  166,  175, */
          /* 220,  224,  240,  257,  266,  267,  280,  281,  282,  286, */
          /* 293,  301,  360,  368,  392,  395,  401,  410,  424,  426, */
          /* 433,  474,  481,  483,  484,  492,  504,  505,  509,  527, */
          /* 533,  539,  541,  562,  615,  616,  629,  633,  635,  637, */
          /* 638,  644,  650,  653,  657,  661,  673,  674,  693,  696, */
          /* 701,  749,  762,  789,  790,  791,  792,  796,  806,  809, */
          /* 810,  811,  812,  813,  814,  817,  821,  823,  824,  829, */
          /* 830,  831,  832,  837,  841,  842,  843,  844,  846,  849, */
          /* 850,  851,  852,  857,  873,  875,  881,  893,  899,  900, */
          /* 903,  916,  939,  953,  954,  956,  974,  981,  989,  993, */
         /* 1005, 1012, 1020, 1026, 1027, 1028, 1045, 1046, 1048, 1057, */
         /* 1061, 1064, 1080, 1100, 1101, 1104, 1132, 1141, 1159, 1171, */
         /* 1180, 1184, 1187, 1189, 1190, 1197, 1199, 1200, 1201, 1202, */
         /* 1204, 1207, 1214, 1217, 1218, 1219, 1222, 1224, 1237, 1240, */
         /* 1241, 1242, 1243, 1258, 1259, 1260, 1261, 1270, 1273, 1274, */
         /* 1281, 1303, 1312, 1319, 1321, 1329, 1341, 1348, 1353, 1354, */
         /* 1369, 1378, 1382, 1383, 1384, 1387, 1388, 1402, 1403, 1404, */
         /* 1405, 1407, 1409, 1423, 1424, 1434, 1442, 1444, 1448, 1466, */
         /* 1486, 1487, 1511, 1561, 1567, 1574, 1575, 1581, 1587, 1588, */
         /* 1597, 1599, 1612, 1619, 1654, 1661, 1668, 1679, 1681, 1705, */
         /* 1713, 1720, 1753, 1761, 1765, 1766, 1773, 1774, 1775, 1776, */
         /* 1807, 1850, 1856, 1866, 1872, 1877, 1878, 1951, 1961, 2005, */
         /* 2032, 2073, 2095, 2097, 2104, 2160, 2188, 2193, 2214, 2216, */
         /* 2304, 2305, 2366, 2387, 2390, 2392, 2394, 2409, 2414, 2415, */
         /* 2417, 2419, 2421, 2441, 2461, 2541, 2569, 2589, 2590, 2601, */
         /* 2621, 2641, 2663, 2721, 2737, 2749, 2766, 2781, 2793, 2821, */
         /* 2822, 2827, 2834, 2845, 2865, 2929, 2969, 2973, 2974, 2975, */
         /* 2976, 3013, 3021, 3061, 3070, 3071, 3081, 3101, 3103, 3105, */
         /* 3106, 3121, 3141, 3146, 3161, 3188, 3201, 3261, 3263, 3289, */
         /* 3299, 3301, 3321, 3337, 3341, 3354, 3355, 3356, 3362, 3401, */
         /* 3407, 3436, 3441, 3471, 3473, 3481, 3493, 3494, 3495, 3501, */
         /* 3517, 3521, 3541, 3561, 3581, 3584, 3621, 3641, 3661, 3668, */
         /* 3670, 3678, 3679, 3681, 3690, 3701, 3718, 3721, 3723, 3725, */
         /* 3727, 3732, 3738, 3739, 3745, 3769, 3777, 3801, 3822, 3840, */
         /* 3881, 3921, 3941, 3981, 3984, 4005, 4006, 4017, 4018, 4019, */
         /* 4053, 4055, 4056, 4057, 4059, 4079, 4101, 4121, 4124, 4161, */
         /* 4171, 4175, 4177, 4181, 4201, 4217, 4223, 4241, 4261, 4272, */
         /* 4281, 4321, 4326, 4331, 4339, 4341, 4350, 4357, 4384, 4405, */
         /* 4458, 4469, 4482, 4495, 4500, 4521, 4541, 4556, 4560, 4561, */
         /* 4585, 4601, 4661, 4677, 4678, 4681, 4684, 4701, 4717, 4763, */
         /* 4768, 4783, 4792 */
    };
    std::sort(bad_towers.begin(), bad_towers.end());
    return bad_towers;
};
