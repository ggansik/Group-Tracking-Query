//
// Created by Hyunsik Yoon on 2022-08-15.
//

#ifndef VIDEOGROUP_INTERFRAME_LAZY_NOTRIE_HPP
#define VIDEOGROUP_INTERFRAME_LAZY_NOTRIE_HPP
#include "SL_set_trie.hpp"
#include <algorithm>
#include <chrono>

class notrie_state
{
public:
    vector<obj*> group;
    size_t start_frame;
    size_t end_frame;

    notrie_state(vector<obj*>& _group
    , const size_t& _start_frame
    , const size_t& _end_frame)
    {
        group = _group;
        start_frame = _start_frame;
        end_frame = _end_frame;
    };

};





bool maximal_segment_check(State& track
        ,list<State>& answers);

void appearance_update_phase_notrie(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,unordered_map<string, notrie_state>& tracks
        ,vector<chrono::nanoseconds>& interframe_time_stats
        ,vector<size_t>& memory_consumption);

void expire_phase_notrie(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,unordered_map<string, notrie_state>& tracks
        ,list<State>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        ,vector<size_t>& memory_consumption);



void interframe_lazy_notrie(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,unordered_map<string, notrie_state>& tracks
        ,list<notrie_state>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption);
#endif //VIDEOGROUP_INTERFRAME_LAZY_NOTRIE_HPP
