//
// Created by Hyunsik Yoon on 2022-07-13.
//

#ifndef VIDEOGROUP_CANDIDATE_COMPUTATION_TEST_HPP
#define VIDEOGROUP_CANDIDATE_COMPUTATION_TEST_HPP
#include <vector>
#include "dataread.hpp"
#include <chrono>
#include "distance.hpp"
#include <algorithm>
#include "grid_utils.hpp"
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>


void candidate_computation_no_recursive(vector<obj*>& t_objs
        , vector<vector<size_t>>& candidate_map
        , const size_t &thres_g
        , const pair<double, double> &thres_d
        , vector<chrono::nanoseconds>& intraframe_time_stats);

void candidate_computation_recursive(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        ,  map<size_t, vector<vector<size_t>>>& candidate_map
        , const size_t& thres_g
        , const pair<double, double>& thres_d);



void candidate_computation_test(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , map<size_t, vector<vector<size_t>>>& candidate_map
        , vector<chrono::nanoseconds>& intraframe_time_stats);

#endif //VIDEOGROUP_CANDIDATE_COMPUTATION_TEST_HPP
