#ifndef VIDEOGROUP_INTRAFRAME_TRIE_HPP
#define VIDEOGROUP_INTRAFRAME_TRIE_HPP


#include "dataread.hpp"
#include "grid_utils.hpp"
#include "distance.hpp"
#include "prefix_tree.hpp"
#include "set_trie.hpp"
//#include "group_util.hpp"
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <set>
#include "SL_set_trie.hpp"

using namespace std;




void candidate_computation_recursive(vector<obj *> candidate
        , vector<size_t> candidate_ids
        , vector<obj *>::iterator left_objs_begin
        , const vector<obj *>::iterator &left_objs_end
        , map<size_t, vector<vector<obj*>>> &groups
        , const size_t &thres_g
        , const pair<double, double> &thres_d
        , vector<chrono::nanoseconds>& intraframe_time_stats);


//void intraframe_processing_multilevel(vector<grid>& a_frame_obj
//        , const pair<size_t, size_t>& video_size
//        , const pair<size_t,size_t>& grid_size
//        , const pair<double, double>& thres_d
//        , const size_t& thres_g
//        , vector<vector<size_t>>& candidate_map);



void candidate_verification(vector<size_t>& candidate_ids
        , map<size_t, map<size_t, prefix_tree>> &buckets
        , const size_t &thres_g
        , const pair<double, double> &thres_d
        , vector<chrono::nanoseconds>& intraframe_time_stats);



void intraframe_processing_trie(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , SL_detection_trie& trie
        , map<size_t, vector<vector<obj*>>>& groups
        , vector<chrono::nanoseconds>& intraframe_time_stats);

#endif //VIDEOGROUP_INTRAFRAME_TRIE_HPP