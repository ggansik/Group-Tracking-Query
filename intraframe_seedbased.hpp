//
// Created by Hyunsik Yoon on 2022-07-27.
//

#ifndef VIDEOGROUP_INTRAFRAME_SEEDBASED_HPP
#define VIDEOGROUP_INTRAFRAME_SEEDBASED_HPP
#include "dataread.hpp"
#include "grid_utils.hpp"
#include "distance.hpp"
#include <list>

//#include "group_util.hpp"
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <set>

bool seed_group_test(vector<obj*>& candidate, obj* t_obj, const pair<double, double>& thres_d);

void candidate_computation_recursive(/*vector<obj *> candidate
                                        , vector<size_t> candidate_ids*/
        vector<obj*>& candidate
        , vector<obj *>::iterator left_objs_begin
        , const vector<obj *>::iterator &left_objs_end
        , const size_t& group_seed_cursor
        , vector<vector<obj*>>& maximal_groups
        , const size_t &thres_g
        , const pair<double, double> &thres_d
        , vector<chrono::nanoseconds>& intraframe_time_stats
        , vector<size_t>& memory_consumption);


void intraframe_processing_seedbased(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , vector<vector<obj*>>& maximal_groups
        , const vector<double>& thres_d
        , const size_t& thres_g
, vector<chrono::nanoseconds>& intraframe_time_stats
,vector<size_t>& memory_consumption);

#endif //VIDEOGROUP_INTRAFRAME_SEEDBASED_HPP
