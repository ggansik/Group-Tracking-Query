//
// Created by Hyunsik Yoon on 2022-06-26.
//

#ifndef VIDEOGROUP_EVALUATION_H
#define VIDEOGROUP_EVALUATION_H

#include "preprocessing.hpp"
#include "grid_utils.hpp"
#include "intraframe.hpp"
#include "intraframe_trie.hpp"
#include "intraframe_buckets.hpp"
#include "prefix_tree.hpp"
#include "candidate_computation_test.hpp"
#include "set_trie.hpp"
#include "SL_set_trie.hpp"
#include "intraframe_seedbased.hpp"
#include "interframe_lazy_inheritance.hpp"
#include "interframe_hash.hpp"
#include "interframe_lazy_notrie.hpp"

#include <unordered_set>

//#include "intraframe_positional_index.hpp"

#include "interframe.hpp"
#include "dataread.hpp"

#include <chrono>
#include <vector>


void method(vector<vector<obj>>& object_per_frame
        , const size_t& grid_method
        , const size_t& occlusion_method
        , const size_t& intraframe_method
        , const pair<size_t, size_t>& video_size
        , const vector<double>& pivot_info
        , const size_t& thres_g
        , const size_t& thres_f
        , const size_t& thres_o
        , const double& ellipse_grid_divider
        , list<State>& answers
        , chrono::nanoseconds& intraframe_time
        , chrono::nanoseconds& interframe_time
        , chrono::nanoseconds& preprocessing_time
        , vector<chrono::nanoseconds>& intraframe_time_stats
        , vector<chrono::nanoseconds>& intraframe_trie_stats
        , vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& interframe_num_states
        , vector<size_t>& memory_consumption);




#endif //VIDEOGROUP_EVALUATION_H
