#ifndef VIDEOGROUP_INTRAFRAME_PROPOSED_HPP
#define VIDEOGROUP_INTRAFRAME_PROPOSED_HPP

#include <vector>
#include "dataread.hpp"
#include "grid_utils.hpp"
#include "distance.hpp"
#include "prefix_tree.hpp"
#include "group_utils.hpp"
#include <chrono>
#include <map>
#include <algorithm>
#include <set>

using namespace std;


void candidate_computation_prefix_bucket(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , vector<vector<prefix_tree_map>>& buckets
        , map<size_t,size_t>& bucket_cand_length
        , const size_t& thres_g
        , const pair<double, double>& thres_d);

//void intraframe_processing_multilevel(vector<grid>& a_frame_obj
//        , const pair<size_t, size_t>& video_size
//        , const pair<size_t,size_t>& grid_size
//        , const pair<double, double>& thres_d
//        , const size_t& thres_g
//        , vector<vector<size_t>>& candidate_map);


void candidate_computation_bucket_only(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , vector<vector<size_t>>& candidate_map
        , vector<vector<vector<size_t>>>& intra_candidate_bucket
        , map<size_t,size_t>& bucket_cand_length
        , const size_t& thres_g
        , const pair<double, double>& thres_d);


void intraframe_processing_proposed(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , vector<vector<size_t>>& candidate_map
        , vector<vector<prefix_tree_map>>& intra_candidate_bucket
        , map<size_t,size_t>& bucket_cand_length
        , const size_t& intraframe_method
        , vector<chrono::nanoseconds>& intraframe_time_stats);

#endif VIDEOGROUP_INTRAFRAME_PROPOSED_HPP