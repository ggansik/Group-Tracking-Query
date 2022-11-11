//
// Created by Hyunsik Yoon on 2022-04-14.
//

#ifndef VIDEOGROUP_INTERFRAME_HPP
#define VIDEOGROUP_INTERFRAME_HPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <chrono>
#include "dataread.hpp"
#include "SL_set_trie.hpp"
#include "interframe_lazy_inheritance.hpp"


using namespace std;



void MFS_intersection(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thred_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& current_candidate
        , map<vector<obj*>,State>& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats);


void interframe_processing(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thres_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& maximal_groups
        , map<vector<obj*>,State>& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption);

void print_state(pair<const vector<size_t> , set<size_t>>& a_state);
void print_group_state(map<vector<size_t>, set<size_t>>& group_state);


#endif //VIDEOGROUP_INTERFRAME_HPP
