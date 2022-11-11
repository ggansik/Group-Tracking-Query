//
// Created by Hyunsik Yoon on 2022-08-08.
//

#ifndef VIDEOGROUP_ELLIPSE_DETERMINATION_STATISTICALLY_HPP
#define VIDEOGROUP_ELLIPSE_DETERMINATION_STATISTICALLY_HPP
#include <vector>
#include "dataread.hpp"
#include "grid_utils.hpp"
#include "distance.hpp"
#include <numeric>
#include <cmath>

#include <chrono>
#include <map>
#include <algorithm>

#include <unordered_map>


using namespace std;

pair<double,double> slope_determination(vector<vector<obj>>& object_per_frame);
pair<double,double> slope_determination_bb(vector<vector<obj>>& object_per_frame);

#endif //VIDEOGROUP_ELLIPSE_DETERMINATION_STATISTICALLY_HPP
