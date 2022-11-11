//
// Created by Hyunsik Yoon on 2022-07-22.
//

#ifndef VIDEOGROUP_SET_TRIE_HPP
#define VIDEOGROUP_SET_TRIE_HPP

#include <iostream>
#include "dataread.hpp"
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <chrono>

using namespace std;

template<typename T>
class trie_node
{
public:
    trie_node* parent;
    size_t object_id;
    size_t depth;
    map<size_t, trie_node*> children;

    bool is_leaf;
    T entry;


    trie_node()
    {
        parent = nullptr;
        object_id = 9999999999;
        depth = 9999999999;
        children = map<size_t, trie_node*>();
        is_leaf = false;
    };

    bool destroy()
    {

    }


    bool superset_search(vector<obj*>& group, const size_t cursor)
    {


        if(object_id == group[cursor]->id)
        {
            if(cursor == group.size()-1) //마지막것까지 문제 없이 봤으면
            {
                return true;
            } else
            {
                if(this->is_leaf) //볼게 남았는데 Leaf까지 온거면
                {
                    return false;
                }else //볼것도 남았고 아직 leaf도 아니면
                {
                    for (auto iter = children.begin(); iter != children.end(); ++iter)
                    {
                        if (iter->first <= group[cursor + 1]->id)
                        {
                            //go lower
                            if (iter->second->superset_search(group, cursor + 1) == true)
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
                        //go lower
                        if(iter->second->superset_search(group, cursor) == true)
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

    trie_node<T>* detection_insert_here(vector<obj*>::iterator left_objs, vector<obj*>& group)
    {

        if(left_objs != group.end())
        {
            auto iter = this->children.insert(make_pair((*left_objs)->id, new trie_node<T>));

            iter.first->second->object_id = (*left_objs)->id;
            iter.first->second->parent = this;
            return iter.first->second->detection_insert_here(++left_objs, group);

        } else
        {//this node is a leaf node
//            this->entry = &group; //link the entry -> only in tracking
            this->is_leaf = true; //set this node as a leaf

            return this;
        }
    }

    trie_node<T>* tracking_insert_here(vector<obj*>::iterator left_objs, vector<obj*>& group)
    {

        if(left_objs != group.end())
        {
            auto iter = this->children.insert(make_pair((*left_objs)->id, new trie_node<T>));

            iter.first->second->object_id = (*left_objs)->id;
            iter.first->second->parent = this;
            return iter.first->second->tracking_insert_here(++left_objs, group);

        } else
        {//this node is a leaf node
            this->entry = &group; //link the entry -> only in tracking
            this->is_leaf = true; //set this node as a leaf

            return this;
        }
    }

    ~trie_node(){};
};


template<typename T>
class trie
{
public:
    trie_node<T>* root;

    trie()
    {
        //initialize the trie with the group
        root = new trie_node<T>();
    };

    pair<size_t, trie_node<T>*> exists(vector<obj*>& group)
    { //both group detection, state
        trie_node<T>* current_node = root;

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
        return make_pair(group.size(), current_node);
    }

    pair<size_t, trie_node<T>*> exists(vector<size_t>& group)
    { //both group detection, state
        trie_node<T>* current_node = root;

        for(size_t i = 0; i < group.size(); i++)
        {
            auto iter = current_node->children.find(group[i]);

            if(iter != current_node->children.end())
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


class detection_trie : public trie<vector<obj*>* >
{
public:
    vector<vector<obj*>*> maximal_groups; //input for the tracking phase

    detection_trie() : trie()
    {
        maximal_groups = vector<vector<obj*>*>();
    };

    bool length_sorted_insert(vector<obj*>& group, vector<chrono::nanoseconds>& time_stats)
    {
//        size_t cursor = 0;
//        if(!root->superset_search(group,cursor))
//        {
//            root->insert_here(group.begin(), group);
//            maximal_groups.push_back(&group);
//            return true;
//        } else
//        {
//            return false;
//        }

        chrono::steady_clock::time_point existence_check_time = chrono::steady_clock::now();
        auto result = exists(group);
        time_stats[4] += chrono::steady_clock::now() - existence_check_time;

        if(result.first < group.size()) // not exists
        {
            //insert the new maximal group
//            if(group[0] == 219 && group[1] == 220 && group[2] == 221 && group[3] == 223)
//            {
//                cout <<" i'm in!" << endl;
//            }
            size_t cursor = 0;
            chrono::steady_clock::time_point superset_search_time = chrono::steady_clock::now();
            auto supersetbool = root->superset_search(group, cursor);
            time_stats[5] += chrono::steady_clock::now() - superset_search_time;
            if(!supersetbool) //there is no superset
            {
                chrono::steady_clock::time_point insertion_time = chrono::steady_clock::now();
                //insert group from last visited node in exists method
                result.second->detection_insert_here(next(group.begin(), result.first), group);
                //link the group pointer
                maximal_groups.push_back(&group);
                time_stats[6] += chrono::steady_clock::now() - insertion_time;

                return true;
            } else
            {
                return false;
            }
        }else
        {
            return false;
        }

    }
};

/*
class tracking_trie : public trie<pair<vector<obj*>*, pair<size_t, size_t>>* >
{
    vector<pair<vector<size_t>*, pair<size_t, size_t>>> states; //list of states group, start frame, end frame

    tracking_trie()
    {

    };

    tracking_trie(map<size_t, vector<vector<size_t>*>>& maximal_groups,const size_t& current_frame)
    {
        for(auto& bucket : maximal_groups)
        {
            for(auto& group : bucket.second)
            {
                states.push_back(make_pair(group, make_pair(current_frame, current_frame)));
            }
        }
    }

    //TODO: does length-ordered maximal group is needed for tracking step?
    bool lazy_update_phase(map<size_t, vector<vector<size_t>*>>& maximal_groups, const size_t& current_frame) //length-ordered maximal groups
    {
        for(auto& bucket : maximal_groups)
        {
            for(auto& group : bucket.second)
            {
                //insert and subset appearance update
                auto leaf_pointer = exists();
                if(exists())
                {//appearance update for me and subset
                    leaf_pointer->entry->second.second = current_frame;
                    lazy_subset_update(group,current_frame);

                }else
                {//insert and appearance update for me and subset
                    //TODO: reuse the leaf_pointer when it is possible
                    states.push_back(make_pair(group, make_pair(current_frame,current_frame))); //new state
                    trie_insert(group, &states.back()); //insert group into the trie and link the state
                    lazy_subset_update(group, current_frame);
                }
            }
        }

    }

    bool lazy_expire_phase()
    { //intersection and expire
        //TODO
    }

    bool lazy_subset_update(vector<size_t>* group, const size_t& current_frame)
    {
        //update the appearance of the subsets of the group
        //TODO
    }

    trie_node<pair<vector<size_t>*, pair<size_t, size_t>>* >*
            trie_insert(vector<size_t>* group,  pair<vector<size_t>*, pair<size_t, size_t>>* state)
    {
        //TODO
    }
};
 */
#endif //VIDEOGROUP_SET_TRIE_HPP
