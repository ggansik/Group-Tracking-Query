//
// Created by Hyunsik Yoon on 2022-07-31.
//

#ifndef VIDEOGROUP_INTERFRAME_HASH_HPP
#define VIDEOGROUP_INTERFRAME_HASH_HPP

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


class SSG_State : public State
{
public:
    int subset_visited;
    string intersect_visited;

    unordered_map<string, SSG_State*> parents = unordered_map<string, SSG_State*>();
    unordered_map<string, SSG_State*> children = unordered_map<string, SSG_State*>();

    SSG_State(vector<obj*>& _group
            , const size_t& _start_frame
            , const size_t& _end_frame
            , SL_trie_node<State>* _leaf_pointer, bool isPs) : State(_group, _start_frame, _end_frame, _leaf_pointer)
    {
        subset_visited = -1;
        intersect_visited = "";
    };

    void subset_update(const size_t& frame_num, const size_t& new_start_frame)
    {
        if(subset_visited == frame_num)
        {
            return;
        }
        subset_visited = frame_num;
        start_frame = min(new_start_frame, start_frame);
        end_frame = frame_num;
        for(auto& child : children)
        {
            child.second->subset_update(frame_num, start_frame);
        }
    }

    size_t get_memory_consumption()
    {
        return parents.size()*(sizeof(int) + sizeof(SSG_State*)) + children.size()*(sizeof(int) +sizeof(SSG_State*))
        + sizeof(subset_visited) + sizeof(int);
    }

};



class SSG
{
public:
    unordered_map<string, SSG_State> entire_state; //전체 state
    unordered_map<string, SSG_State*> PSList; //한 번이라도 직접 등장했던 state들

    SSG()
    {
        PSList = unordered_map<string, SSG_State*>();
        entire_state = unordered_map<string, SSG_State>();
    };

    size_t get_memory_consumption()
    {
        size_t SSG_memory_consumption = 0;

        for(auto& state: entire_state)
        {
            SSG_memory_consumption += sizeof(string) + state.second.get_memory_consumption();
        }

        return SSG_memory_consumption + PSList.size()*(sizeof(int) + sizeof(SSG_State*));
    };

};


void SSG_CM(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thred_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& current_candidate
        , SSG& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats);




void interframe_processing_SSG(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thres_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& maximal_groups
        , SSG& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption);

#endif //VIDEOGROUP_INTERFRAME_HASH_HPP
