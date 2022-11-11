//
// Created by Hyunsik Yoon on 2022-05-10.
//

#include "distance.hpp"

bool ellipse_check(const obj* seed_obj
        , const obj* t_obj
        , const double& relative_long_radius
        , const double& relative_short_radius)
{
    //if summation of distance from each focus of the seed_obj-centered ellipse to
    //t_obj is smaller than 2*relative_long_radius means ellipse including t_obj

    double focus_offset = sqrt(pow(relative_long_radius, 2) - pow(relative_short_radius, 2));
    double right_focus_x = seed_obj->bb_center[0] + focus_offset;
    double left_focus_x = seed_obj->bb_center[0] - focus_offset; //although < 0, it doesn't matter.
    double focus_y = seed_obj->bb_center[1];

    double right_focus_distance = sqrt(pow(right_focus_x - t_obj->bb_center[0], 2) + pow(focus_y - t_obj->bb_center[1], 2));
    double left_focus_distance = sqrt(pow(left_focus_x - t_obj->bb_center[0], 2) + pow(focus_y - t_obj->bb_center[1], 2));

    if(right_focus_distance + left_focus_distance <= relative_long_radius*2)
    {
        return true;
    } else
    {
        return false;
    }
}