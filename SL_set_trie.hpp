//
// Created by Hyunsik Yoon on 2022-07-25.
//

#ifndef VIDEOGROUP_SL_SET_TRIE_HPP
#define VIDEOGROUP_SL_SET_TRIE_HPP
#include <iostream>
#include "dataread.hpp"
#include "distance.hpp"
#include <algorithm>
#include <fstream>
#include <vector>
#include <list>
#include <sstream>
#include <map>
#include <unordered_map>
#include <cmath>
#include <chrono>

using namespace std;

//bool group_test(vector<obj*>& candidate, obj* t_obj, const pair<double, double>& thres_d)
//{
//    //check whether t_obj can be a member of group
//    for(const auto& member : candidate)
//    {
//        double m_relative_long_radius = thres_d.first * (double)(member->bb_center[1]);
//        double m_relative_short_radius = m_relative_long_radius * thres_d.second;
//
//        double t_relative_long_radius = thres_d.first * (double)(t_obj->bb_center[1]);
//        double t_relative_short_radius = t_relative_long_radius * thres_d.second;
//
//        if(!ellipse_check(member, t_obj, m_relative_long_radius, m_relative_short_radius)
//           || !ellipse_check(t_obj, member, t_relative_long_radius, t_relative_short_radius))
//        {
//            return false;
//        }
//    }
//    return true;
//};

class State;


template<typename T>
class SL_trie_node
{
public:
    SL_trie_node* parent;
    size_t object_id;
    size_t depth;
    unordered_map<size_t, SL_trie_node*> children;

    vector<size_t> L;
    obj* S;

    bool is_leaf;
    size_t entry_idx;
    State* entry;


    SL_trie_node()
    {
        parent = nullptr;
        object_id = 999999999;
        depth = 9999999999;
        children = unordered_map<size_t, SL_trie_node*>();
        is_leaf = false;
        entry_idx = -1;
        L = {0, 0};
        entry = nullptr;
    };

    bool destroy()
    {

    }

    size_t get_memory_consumption()
    {
        size_t child_memory_consumption = 0;
        size_t children_variable_memory_consumption = 0;
        for(auto& child : children)
        {
            child_memory_consumption += child.second->get_memory_consumption();
        }

        return child_memory_consumption
        + sizeof(parent)
        + sizeof(object_id)
        + sizeof(depth)
        + children.size() * (sizeof(size_t) + sizeof(SL_trie_node*))
        + sizeof(is_leaf)
        + sizeof(entry);
    }


    bool trie_group_test(obj*& candidate, obj*& t_obj, const pair<double, double>& thres_d)
    {
        //check whether t_obj can be a member of group

        double m_relative_long_radius = thres_d.first * (double)(candidate->bb_center[1]);
        double m_relative_short_radius = m_relative_long_radius * thres_d.second;

        double t_relative_long_radius = thres_d.first * (double)(t_obj->bb_center[1]);
        double t_relative_short_radius = t_relative_long_radius * thres_d.second;


        if(t_obj->bb_center[1] > candidate->bb_center[1])
        {
            if(t_obj->bb_center[0] > candidate->bb_center[0])
            {
                if(t_obj->bb_center[0] - candidate->bb_center[0] <= t_relative_long_radius
                   && t_obj->bb_center[1] - candidate->bb_center[1] <= t_relative_short_radius)
                {
                    return true;
                }
            } else
            {
                if(candidate->bb_center[0] - t_obj->bb_center[0] <= t_relative_long_radius
                   && t_obj->bb_center[1] - candidate->bb_center[1] <= t_relative_short_radius)
                {
                    return true;
                }
            }
        } else
        {
            if(t_obj->bb_center[0] > candidate->bb_center[0])
            {
                if(t_obj->bb_center[0] - candidate->bb_center[0] <= m_relative_long_radius
                   && candidate->bb_center[1] - t_obj->bb_center[1] <= m_relative_short_radius)
                {
                    return true;
                }
            } else
            {
                if(candidate->bb_center[0] - t_obj->bb_center[0] <= m_relative_long_radius
                   && candidate->bb_center[1] - t_obj->bb_center[1] <= m_relative_short_radius)
                {
                    return true;
                }
            }
        }
        return false;



//        if(!ellipse_check(candidate, t_obj, m_relative_long_radius, m_relative_short_radius)
//           || !ellipse_check(t_obj, candidate, t_relative_long_radius, t_relative_short_radius))
//        {
//            return false;
//        }
//
//        return true;
    };

    bool detection_superset_existence(vector<obj*>& group, const size_t cursor, const pair<double, double>& thres_d)
    {

        if(object_id == group[cursor]->id)
        {
            if(cursor == group.size()-1) //마지막것까지 문제 없이 봤으면
            {
                return true;
            } else
            {
                if(this->children.empty()) //볼게 남았는데 자식노드가 없으면
                {
                    return false;
                }else //볼것도 남았고 아직 leaf도 아니면
                {
                    for (auto iter = children.begin(); iter != children.end(); ++iter)
                    {
                        if (iter->first <= group[cursor + 1]->id)
                        {
                            //go lower
                            if (iter->second->detection_superset_existence(group, cursor + 1,thres_d) == true)
                            {
                                return true;
                            }
                        }
                    }
                    //자식노드 하위의 subtree에서도 못찾으면
                    return false;
                }
            }

        } else //현재노드가 매칭이 안되면
        {
            if(children.empty()) //볼게 남았는데 자식노드가 없으면
            {
                return false;
            } else
            {
                for(auto iter = children.begin(); iter != children.end(); ++iter)
                {

                    if(iter->first <= group[cursor]->id)
                    {
//                        if(iter->second->depth == 0)
//                        {
//                            // group[0], this.S proximity 판단;
//                            if(!trie_group_test(group[0],iter->second->S, thres_d))
//                            {
//                                continue;
//                            }
//                        }

                        if(iter->second->depth == 0)
                        {
                            // group[0], this.L length 판단 subtree가 포함하는 최대길이보다 내가 더 길면
                            if(iter->second->L[0] < group.size())
                            {
                                continue;
                            }
                        }


                        //go lower
                        if(iter->second->detection_superset_existence(group, cursor, thres_d))
                        {
                            return true;
                        }
                    }
                }
                //자식노드 하위의 subtree에서도 못찾으면
                return false;
            }

        }
    }

    bool detection_remove_all_subsets(vector<obj*>::iterator iter
                                    , vector<obj*>& group
                                    , vector<vector<obj*>*>& maximal_groups
                                    , const pair<double, double>& thres_d)
    {
        for(;iter != group.end(); ++iter)
        {
            if(children.empty()) //real leaf before end of group iter
            {
                return true;
            } else
            {
                auto key_value = children.find((*iter)->id);
                if (key_value != children.end())
                {
                    //여기 들어오면 이미 children에 도달한 것
                    if(key_value->second->is_leaf)
                    {
                        key_value->second->is_leaf = false;

                        //remove from maximal_groups of the trie
                        if (maximal_groups.size() > 1)
                        {
                            iter_swap(maximal_groups.begin() + key_value->second->entry_idx, maximal_groups.end() - 1);
                            maximal_groups.pop_back();
                        }
                        else
                        {
                            maximal_groups.clear();
                        }

                    }
                    if(key_value->second->detection_remove_all_subsets(iter + 1, group, maximal_groups, thres_d))
                    {
                        children.erase(key_value->first);
                    }
                }
            }
        }
        if(children.empty()) //도달했던 노드들은 다 이미 leaf가 아니게 됨
        {
            return true;
        } else
        {
            return false;
        }
    }

    SL_trie_node<T>* detection_insert_root(vector<obj*>& group, const size_t& _entry_idx)
    {
        SL_trie_node<T>* current_node = this; //root

        size_t insert_cursor = 0;
        for(size_t i = 0; i < group.size(); i++)
        {
            auto iter = current_node->children.find(group[i]->id);

            if(iter != current_node->children.end())
            {
                if(i == 0) //update L
                {
                    if(iter.first->second->L[0] > group.size())
                    {
                        iter.first->second->L[0] = group.size();
                        iter.first->second->L[1] = 1;
                    } else if (iter.first->second->L[0] == group.size())
                    {
                        iter.first->second->L[1]++;
                    }
                }
                current_node = iter->second;
            } else
            {

                //여기서부터 삽입
                insert_cursor = i;
                break;
            }
        }

        for(;insert_cursor < group.size();insert_cursor++)
        {

            auto result = current_node->children.insert(make_pair(group[insert_cursor], new SL_trie_node<vector<obj*>>));

            if(insert_cursor == 0)
            {
                current_node->S = group[0];

                if(result.first->second->L[0] > group.size())
                {
                    result.first->second->L[0] = group.size();
                    result.first->second->L[1] = 1;
                } else if (result.first->second->L[0] == group.size())
                {
                    result.first->second->L[1]++;
                }
            }

            result.first->second->object_id = group[insert_cursor]->id;
            result.first->second->depth = insert_cursor;
            result.first->second->parent = current_node;

            current_node = result->first->second;
        }

        current_node->entry_idx = _entry_idx;
        current_node->is_leaf = true;

        return current_node;
    }

    SL_trie_node<T>* detection_insert_here(const size_t& cursor, vector<obj*>& group, const size_t& _entry_idx)
    {//use this with exists
        if(cursor < group.size())
        {

            auto iter = this->children.insert(make_pair(group[cursor]->id, new SL_trie_node<T>));

            iter.first->second->object_id = group[cursor]->id;
            iter.first->second->depth = cursor;
            iter.first->second->parent = this;
            return iter.first->second->detection_insert_here(cursor+1, group, _entry_idx);

        } else
        {//this node is a leaf node
            this->entry_idx = _entry_idx; //link the entry
            this->is_leaf = true; //set this node as a leaf

            return this;
        }
    }

    SL_trie_node<T>* tracking_insert_here(const size_t& cursor
                                        , vector<obj*>& group
                                        , State* _entry)
    {

        if(cursor < group.size())
        {
            auto iter = this->children.insert(make_pair(group[cursor]->id, new SL_trie_node<T>));

            iter.first->second->object_id = group[cursor]->id;
            iter.first->second->parent = this;
            iter.first->second->depth = cursor;
            return iter.first->second->tracking_insert_here(cursor + 1, group, _entry);

        } else if(cursor >= group.size())
        {//this node is a leaf node
            this->entry = _entry; //link the entry -> only in tracking
            this->is_leaf = true; //set this node as a leaf

            return this;
        }
    }

    void tracking_subset_update(vector<obj*>::iterator iter
            , vector<obj*>& group
            , const size_t& frame_number)
    {
//        if(frame_number == 51)
//        {
//            cout << 'hello' << endl;
//        }
        for(;iter != group.end(); ++iter)
        {
            if(children.empty()) //real leaf before end of group iter
            {
                return;
            } else
            {
                auto key_value = children.find((*iter)->id);
                if (key_value != children.end())
                {
                    //여기 들어오면 이미 children에 도달한 것
                    if(key_value->second->is_leaf)
                    {
                        key_value->second->entry->end_frame = frame_number;
                    }

                    key_value->second->tracking_subset_update(next(iter), group, frame_number);
                }
            }
        }
    }

    void tracking_traverse_remove_node(vector<obj*>& group)
    {
        SL_trie_node<T>* current_node = this; //root

        vector<unordered_map<size_t, SL_trie_node<State>*>::iterator> path;

        for(size_t i = 0; i < group.size(); i++)
        {
            auto iter = current_node->children.find(group[i]->id);

            if(iter != current_node->children.end())
            {
                path.emplace_back(iter);
                current_node = iter->second;
            }
        }



        for(auto iter = path.rbegin(); iter != path.rend(); ++iter)
        {
//            cout << (*iter)->first << endl;

//            cout << (*iter)->first <<endl;
            if(current_node->children.empty())
            {
                auto parent_ptr = current_node->parent;
                delete ((*iter)->second);
                parent_ptr->children.erase((*iter));

                if(!parent_ptr->children.empty() || parent_ptr->is_leaf)
                {//children이 empty가 아니거나(다른 branch에 leaf가 존재함)
                    //children이 Empty여도 leaf이면 더이상 지우지 않음
                    break;
                }else //parent가 leaf도 아니고 children이 empty도 아니면
                {
                    current_node = parent_ptr;
                }
            }else
            {
                current_node->is_leaf = false;
            }


        }
    }
    ~SL_trie_node(){};
};







template<typename T>
class SL_trie
{
public:
    SL_trie_node<T>* root;

    SL_trie()
    {
        //initialize the trie with the group
        root = new SL_trie_node<T>();
    };

    pair<size_t, SL_trie_node<T>*> exists(vector<obj*>& group)
    { //both group detection, state
        SL_trie_node<T>* current_node = root;

        for(size_t i = 0; i < group.size(); i++)
        {
            auto iter = current_node->children.find(group[i]->id);

            if(iter != current_node->children.end())
            {
                current_node = iter->second;
            } else
            {
                return make_pair(i, current_node);
            }
        }
        if(current_node->is_leaf)
            return make_pair(group.size(), current_node);
        else
            return make_pair(group.size()+1, current_node); //founded, but it is not a leaf
    }


    pair<size_t, SL_trie_node<T>*> exists(vector<size_t>& group)
    { //both group detection, state
        SL_trie_node<T> *current_node = root;

        for (size_t i = 0; i < group.size(); i++)
        {
            auto iter = current_node->children.find(group[i]);

            if (iter != current_node->children.end())
            {
                current_node = iter->second;
            } else
            {
                return make_pair(i, current_node);
            }
        }
        return make_pair(group.size(), current_node);
    }

};


class SL_detection_trie : public SL_trie<vector<obj*>*>
{

public:
    vector<vector<obj*>*> maximal_groups; //input for the tracking phase
    pair<double, double> thres_d;

    SL_detection_trie(const pair<double, double> _thres_d) : SL_trie()
    {
        maximal_groups = vector<vector<obj*>*>();
        thres_d = _thres_d;
    };

    void unsorted_insert(vector<obj*>& group, vector<chrono::nanoseconds>& time_stats)
    {
//        for(auto& obj : group)
//        {
//            cout << obj->id << ", ";
//        }
//        cout << endl;
        chrono::steady_clock::time_point existence_check_time = chrono::steady_clock::now();
//        auto result = exists(group);
        time_stats[4] += chrono::steady_clock::now() - existence_check_time;

        size_t cursor = 0;
        chrono::steady_clock::time_point superset_search_time = chrono::steady_clock::now();
        auto supersetbool = root->detection_superset_existence(group, cursor, thres_d);
        time_stats[5] += chrono::steady_clock::now() - superset_search_time;

        if(!supersetbool) //there is no superset
        {
            chrono::steady_clock::time_point subset_check = chrono::steady_clock::now();
            root->detection_remove_all_subsets(group.begin(), group, maximal_groups, thres_d);
            time_stats[6] += chrono::steady_clock::now() - subset_check;

            chrono::steady_clock::time_point insertion_time = chrono::steady_clock::now();
            maximal_groups.push_back(&group);
            root->detection_insert_here(0, group, maximal_groups.size()-1);
            time_stats[7] += chrono::steady_clock::now() - insertion_time;
        }
    }


    void ascending_ordered_insert(vector<obj*>& group, vector<chrono::nanoseconds>& time_stats)
    {
        chrono::steady_clock::time_point existence_check_time = chrono::steady_clock::now();
        auto result = exists(group);
        time_stats[4] += chrono::steady_clock::now() - existence_check_time;

        if(result.first < group.size()) // not exists
        {

            chrono::steady_clock::time_point subset_check = chrono::steady_clock::now();
            root->detection_remove_all_subsets(group.begin(), group, maximal_groups, thres_d);
            time_stats[6] += chrono::steady_clock::now() - subset_check;

            chrono::steady_clock::time_point insertion_time = chrono::steady_clock::now();
            //insert group from last visited node in exists method
            //add the entry and link the idx of entry to leaf node
            maximal_groups.push_back(&group);
            result.second->detection_insert_here(result.first, group, maximal_groups.size()-1);
            time_stats[7] += chrono::steady_clock::now() - insertion_time;
        }
    }

    void descending_ordered_insert(vector<obj*>& group, vector<chrono::nanoseconds>& time_stats)
    {
        chrono::steady_clock::time_point existence_check_time = chrono::steady_clock::now();
        auto result = exists(group);
        time_stats[4] += chrono::steady_clock::now() - existence_check_time;

        if(result.first < group.size()) // not exists
        {

            size_t cursor = 0;
            chrono::steady_clock::time_point superset_search_time = chrono::steady_clock::now();
            auto supersetbool = root->detection_superset_existence(group, cursor, thres_d);
            time_stats[5] += chrono::steady_clock::now() - superset_search_time;

            if(!supersetbool) //there is no superset
            {
                chrono::steady_clock::time_point insertion_time = chrono::steady_clock::now();
                //insert group from last visited node in exists method
                //add the entry and link the idx of entry to leaf node
                maximal_groups.push_back(&group);
                result.second->detection_insert_here(result.first, group, maximal_groups.size()-1);
                time_stats[7] += chrono::steady_clock::now() - insertion_time;
            }
        }
    }
};

class State
{
public:
    vector<obj*> group;
    size_t start_frame;
    size_t end_frame;
    SL_trie_node<State>* leaf_pointer;

    State(vector<obj*>& _group
            , const size_t& _start_frame
            , const size_t& _end_frame
            , SL_trie_node<State>* _leaf_pointer)
    {
        group = _group;
        start_frame = _start_frame;
        end_frame = _end_frame;
        leaf_pointer = _leaf_pointer;
    }


    size_t get_memory_consumption()
    {
        return group.size()*sizeof(obj*) + sizeof(start_frame) + sizeof(end_frame);
    }
};



class SL_tracking_trie : public SL_trie<State>
{
public:
    list<State> states; //list of states group, start frame, end frame

    SL_tracking_trie()
    {

    };

    list<State>::iterator lazy_expire(list<State>::iterator& state)
    {
        //TODO
        //remove from the trie,
        root->tracking_traverse_remove_node(state->group);
        //remove from the states
        return states.erase(state);
    }

    bool lazy_subset_update(vector<obj*>& group, const size_t& current_frame)
    {
        //update the appearance of the subsets of the group
        //TODO
        root->tracking_subset_update(group.begin(), group, current_frame);
    }

    void lazy_add_new_track(vector<obj*>& group
            , pair<size_t, size_t>& lifetime
            , pair<size_t, SL_trie_node<State>*>& exists_result)
    {
        //TODO

        states.emplace_back(State(group, lifetime.first, lifetime.second, nullptr));

        states.back().leaf_pointer =
                exists_result.second->tracking_insert_here(exists_result.first, group, &states.back());
    }

    pair<size_t,size_t> get_memory_consumption()
    {
        size_t state_memory_consumption = 0;
        size_t trie_memory_consumption = 0;

        for(auto& state : states)
        {
            state_memory_consumption += state.get_memory_consumption();
        }

        trie_memory_consumption = root->get_memory_consumption();


        return make_pair(state_memory_consumption,trie_memory_consumption);
    }



//    void lazy_add_new_track(vector<obj*>& group
//            , pair<size_t, size_t>& lifetime)
//    {
//        //TODO
//        states.emplace_back(State(group, lifetime.first, lifetime.second, nullptr));
//        root->tracking_insert_here(0, group, &states.back());
//    }
};
#endif //VIDEOGROUP_SL_SET_TRIE_HPP
