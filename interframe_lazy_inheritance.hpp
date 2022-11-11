//
// Created by Hyunsik Yoon on 2022-07-27.
//

#ifndef VIDEOGROUP_INTERFRAME_LAZY_INHERITANCE_HPP
#define VIDEOGROUP_INTERFRAME_LAZY_INHERITANCE_HPP
#include "SL_set_trie.hpp"
#include <algorithm>
#include <chrono>

bool maximal_segment_check(State& track
        ,list<State>& answers);

void appearance_update_phase(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,SL_tracking_trie& tracks
        ,vector<chrono::nanoseconds>& interframe_time_stats
        ,vector<size_t>& memory_consumption);
void expire_phase(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,SL_tracking_trie& tracks
        ,list<State>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        ,vector<size_t>& memory_consumption);




void interframe_lazy(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,const size_t& last_frame_no
        ,vector<vector<obj*>>& appeared_groups
        ,SL_tracking_trie& tracks
        ,list<State>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption);

#endif //VIDEOGROUP_INTERFRAME_LAZY_INHERITANCE_HPP
