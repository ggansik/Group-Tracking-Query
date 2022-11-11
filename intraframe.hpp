//
// Created by Hyunsik Yoon on 2022-04-13.
//

#ifndef VIDEOGROUP_INTRAFRAME_HPP
#define VIDEOGROUP_INTRAFRAME_HPP

#include <vector>
#include "dataread.hpp"
#include "grid_utils.hpp"
#include "distance.hpp"

#include <chrono>
#include <map>
#include <algorithm>
#include <list>

using namespace std;

void candidate_computation_naive_map(vector<obj*>& candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , map<string, vector<size_t>>& candidate_map
        , const size_t& thres_g
        , const vector<double>& pivot_info);


void candidate_computation_naive_vector(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , vector<vector<obj*>>& maximal_groups
        , const size_t& thres_g
        , const vector<double>& pivot_info
        , vector<chrono::nanoseconds>& time_stats);


void intraframe_processing(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const vector<double>& pivot_info
        , const size_t& thres_g
        , vector<vector<obj*>>& maximal_groups
        , vector<chrono::nanoseconds>& intraframe_time_stats
        , vector<size_t>& memory_consumption);



#endif //VIDEOGROUP_INTRAFRAME_HPP
