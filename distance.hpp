//
// Created by Hyunsik Yoon on 2022-05-10.
//

#ifndef VIDEOGROUP_DISTANCE_HPP
#define VIDEOGROUP_DISTANCE_HPP
#include "dataread.hpp"
#include "cmath"
bool ellipse_check(const obj* seed_obj
        , const obj* t_obj
        , const double& relative_long_radius
        , const double& relative_short_radius);


//vector<double> ellipse_info = {slope_ret.first
//        , slope_ret.second
//        , pivot_long_rad
//        , pivot_short_rad
//        , pivot_y_coord};

#endif //VIDEOGROUP_DISTANCE_HPP
