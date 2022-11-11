//
// Created by Hyunsik Yoon on 2022-07-06.
//

#ifndef VIDEOGROUP_INTRAFRAME_POSITIONAL_INDEX_HPP
#define VIDEOGROUP_INTRAFRAME_POSITIONAL_INDEX_HPP

#include <vector>
#include "dataread.hpp"
#include "grid_utils.hpp"
#include "distance.hpp"
#include "proximity.hpp"

#include <chrono>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <queue>

class bucket_entry
{
public:
    vector<size_t>* candidate;
    size_t position;

    bucket_entry(vector<size_t>* _candidate, size_t& _position)
    {
        candidate = _candidate;
        position = _position;
    }

    bool operator<(const bucket_entry& e) const
    {
        return this->position > e.position;
    }

};


class buckets
{
public:
    map<size_t, priority_queue<bucket_entry>> bucket_list;
//    map<size_t, size_t> length_index_mapping;

    buckets()
    {
        bucket_list = map<size_t, priority_queue<bucket_entry>>();
//        length_index_mapping = map<size_t,size_t>();
    }

    void allocate_and_insert(vector<size_t>& candidate_ids, size_t& pos)
    {
        auto iter = bucket_list.insert(make_pair(candidate_ids.size(), priority_queue<bucket_entry>()));
        iter.first->second.push(bucket_entry(&candidate_ids, pos));
    }

//    bool existence_check(vector<size_t>& candidate_ids)
//    {
//        for(auto iter = bucket_list.begin(); iter != bucket_list.end(); ++iter)
//        {
//            if(iter->second.top().candidate.size() >= candidate_ids.size())
//            {
////                for(auto entry : iter->second)
////                {
////
////                }
//            }else
//            {
//                break;
//            }
//        }
//        return false
//    }

    map<size_t, priority_queue<bucket_entry>>::iterator find(const size_t& length)
    {
        return bucket_list.find(length);
    }

    map<size_t, priority_queue<bucket_entry>>::iterator end()
    {
        return bucket_list.end();
    }


};

class positional_index
{
public:
    map<size_t, buckets> index;
    vector<vector<size_t>> candidate_list;
    size_t thres_g;

    positional_index(vector<vector<obj*>>& frame_obj_ids, size_t _thres_g)
    {
        for(auto& grid : frame_obj_ids)
        {
            for(auto& obj : grid)
            {
                index[obj->id] = buckets();
            }
        }

        candidate_list = vector<vector<size_t>>();
        thres_g = _thres_g;
    }
    positional_index();

    void insert_and_update(vector<size_t>& candidate_ids)
    {
        candidate_list.push_back(candidate_ids);
        for(size_t pos = 0; pos < candidate_ids.size(); ++pos)
        {
            //if id-length mapping is exists
            auto bucket_index = index[candidate_ids[pos]].find(candidate_ids.size());
            if(bucket_index != index[candidate_ids[pos]].end()) //bucket of current length is exists
            {
                bucket_index->second.push(bucket_entry(&(candidate_list.back()), pos));
            } else
            {
                //add new bucket and insert entry
                index[candidate_ids[pos]].allocate_and_insert(candidate_list.back(), pos);

            }

        }

        //erase subsets
        //create all of the possible subsets start from longest one
        //TODO: how to mitigate the double-check overhead?
//        for(int length = candidate_ids.size(); length >= thres_g; --length)
//        {
//            //(12345, thres_g = 2) -> 1, 2, 3, 4 index search bucket(4,3,2) bucket(4,3,2) bucket(3,2) bucket(2)
//
//
//        }
    }

    bool existence_check(vector<size_t>& candidate_ids)
    {
        return false;
//        return index[candidate_ids[0]].existence_check(candidate_ids);
    }

    size_t size()
    {
        return index.size();
    }
};


void candidate_computation_positional_index(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , positional_index& positional_index
        , const size_t& thres_g
        , const pair<double, double>& thres_d);

void intraframe_processing_positional_index(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , positional_index& positional_index
        , const size_t& method
        , vector<chrono::nanoseconds>& intraframe_time_stats);

#endif //VIDEOGROUP_INTRAFRAME_POSITIONAL_INDEX_HPP
