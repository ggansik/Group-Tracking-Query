//
// Created by Hyunsik Yoon on 2022-07-09.
//
#include "intraframe_positional_index.hpp"

void candidate_computation_positional_index(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , positional_index& positional_index
        , const size_t& thres_g
        , const pair<double, double>& thres_d)
{
    return;
}

void intraframe_processing_positional_index(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , positional_index& positional_index
        , const size_t& method
        , vector<chrono::nanoseconds>& intraframe_time_stats)
{
    return;
}