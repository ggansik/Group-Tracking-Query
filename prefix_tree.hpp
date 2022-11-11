//
// Created by Hyunsik Yoon on 2022-06-27.
//

#ifndef VIDEOGROUP_PREFIX_TREE_HPP
#define VIDEOGROUP_PREFIX_TREE_HPP

#include <iostream>
#include "dataread.hpp"
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
using namespace std;


class prefix_node
{

public:
    size_t obj_id;
    size_t depth;
    prefix_node* parent = nullptr;
    vector<size_t> prefix;
    map<size_t, prefix_node*> children = map<size_t, prefix_node*>();

    prefix_node(){};

    prefix_node(/*map<size_t, map<size_t, vector<prefix_node*>>>& id_node_pointer
            ,*/ vector<size_t>::iterator candidate_iter
            , vector<size_t>::iterator iter_end
            , prefix_node* _parent)
    {
        obj_id = (*candidate_iter);


//        auto iter = id_node_pointer.insert(make_pair(obj_id, map<size_t, vector<prefix_node*>>()));
//        iter.first->second[depth].push_back(this);

        if(_parent == nullptr)//root
        {
            depth = 0;
        } else
        {
            this->parent = _parent;
            depth = this->parent->depth + 1;
        }

        if(++candidate_iter != iter_end)
        {
            children[*(candidate_iter)] = new prefix_node(/*id_node_pointer,*/ candidate_iter, iter_end, this);
        }
    };

    size_t get_num_leaf(size_t height)
    {
        size_t sum = 0;
        if(depth+1 == height){
            return 1;
        } else
        {
            for(auto& child : children)
            {
                sum += child.second->get_num_leaf(height);
            }

            return sum;
        }
    }
    void print_leaf(size_t height)
    {
        if(depth+1 == height)
        {
            cout << "depth " << depth << ":" << obj_id << endl;
            return;
        } else
        {
            cout << "depth " << depth << ":" << obj_id << endl;
            for(auto& child : children)
            {
                child.second->print_leaf(height);
            }
        }
    }


    bool subtree_search(vector<size_t>& candidate_ids, size_t cursor, size_t& height)
    {
        if (height - depth >= candidate_ids.size() - cursor) //depth of children is enough?
        {
            if(obj_id == candidate_ids[cursor])
            {
                if(cursor == candidate_ids.size() - 1) //leaf 였으면
                {
                    return true;
                } else //Internal node 였으면
                {
                    for (auto &child : children)
                    {
                        if(child.second->obj_id > candidate_ids[cursor + 1])//it means there is no possibilities to contain me
                        {
                            return false;
                        }
                        if (child.second->subtree_search(candidate_ids, cursor + 1, height))
                        {
                            return true;
                        }
                    }
                }
            }else //go to lower tree
            {
                if(height == depth + 1)
                {
                    return false;
                }
                for(auto& child : children)
                {
                    if(child.second->obj_id > candidate_ids[cursor]) //it means there is no possibilities to contain me
                    {
                        return false;
                    }
                    if(child.second->subtree_search(candidate_ids, cursor, height))
                    {
                        return true;
                    }
                }
            }

//                auto iter = children.find(candidate_ids[cursor]);
//                if (iter != children.end()) //matched
//                {
//                    return iter->second->subtree_search(candidate_ids, ++cursor, height);
//                } else //go to lower tree
//                {
//                    for(auto& child : children)
//                    {
//                        return child.second->subtree_search(candidate_ids, cursor, height);
//                    }
//                }
        } else
        {
            return false;
        }

    };

    prefix_node* get_common_prefix(vector<size_t>& candidate_ids, size_t& cursor)
    {
        auto iter = children.find(candidate_ids[cursor]);
        if(iter != children.end())
        {
            return iter->second->get_common_prefix(candidate_ids, ++cursor);
        } else
        {
            return this;
        }
    };

    void id_node_pointer_eraser(map<size_t, map<size_t, vector<prefix_node*>>>& id_node_pointer, prefix_node* erasing_node)
    {
        auto id_iter = id_node_pointer.find(erasing_node->obj_id);
        auto depth_iter = id_iter->second.find(depth);

        for(size_t i = 0 ; i < depth_iter->second.size(); ++i)
        {
            if(depth_iter->second[i] == erasing_node)
            {
                depth_iter->second.erase(depth_iter->second.begin() + i);

                break;
            }
        }
//        depth_iter->second.erase(remove(depth_iter->second.begin(), depth_iter->second.end(), ), depth_iter->second.end());

    }

    bool subset_update(vector<size_t>& candidate_ids
            , size_t left_length
            , size_t& height
            )
    {
        if(height == depth + 1) //leaf
        {
            for(;left_length != 0;--left_length)
            {
                if (obj_id == candidate_ids[candidate_ids.size() - left_length])
                {
                    //remove current path
                    return true;
                }
            }
            return false;

        } else
        {
            for(;left_length != 0;--left_length)
            {
                if (obj_id == candidate_ids[candidate_ids.size() - left_length])
                {
                    for (auto iter = children.begin(); iter != children.end();)
                    {
                        if(left_length != 1)
                        {
                            if (iter->second->obj_id >
                                candidate_ids[candidate_ids.size() - left_length + 1])
                            {
                                return false;
                            }
                        }
                        if(iter->second->subset_update(candidate_ids, left_length - 1, height/*, id_node_pointer*/))
                        {
                            //id_node_pointer_eraser(id_node_pointer, iter->second);
                            delete(iter->second);
                            children.erase(iter++);
                        } else
                        {
                            ++iter;
                        }
                    }
                    if(children.empty())
                    {
                        return true;
                    } else
                    {
                        return false;
                    }
                }
            }
        }
        return false;
    };

    void get_candidate(vector<size_t>& leaf)
    {
        leaf.push_back(obj_id);

    }
};





class prefix_tree
{

public:
    prefix_node* root;
    size_t height;
    vector<prefix_node*> leaves;
//    map<size_t, map<size_t, vector<prefix_node*>>> id_node_pointer; //id, depth, pointer

    prefix_tree(){};

    prefix_tree(vector<size_t>& candidate_ids)
    {
        root = new prefix_node(/*id_node_pointer,*/ candidate_ids.begin(), candidate_ids.end(), nullptr);
        height = candidate_ids.size();

    };

//    bool contains(vector<size_t>& candidate_ids)
//    {
//        //start from shallowest depth_node
//        map<size_t, vector<prefix_node*>>* depth_pointer_map = &id_node_pointer[candidate_ids[0]];
//        for(auto iter = (*depth_pointer_map).begin(); iter != (*depth_pointer_map).end();) //depth iter
//        {
//            if (height - iter->first >= candidate_ids.size())//subtree is enough to contains me
//            {
//                for(auto& node_pointer : iter->second)
//                {
//                    size_t cursor = 1;
//                    for(auto& child : node_pointer->children)
//                    {
//                        if(child.second->subtree_search(candidate_ids, cursor, height))
//                        {
//                            return true;
//                        }
//                    }
//                }
//                ++iter;
//            }
//        }
//        return false;
//    };

    bool no_pointer_contains(vector<size_t>& candidate_ids)
    {

        size_t cursor = 1;
        if(root->obj_id != candidate_ids[0])
        {
            cursor = 0;
            if(candidate_ids.size() == height)
            {
                return false;
            }
        }
        for(auto& child : root->children)
        {
            if(child.second->obj_id > candidate_ids[cursor])
            {
                break;
            } else
            {
                if (child.second->subtree_search(candidate_ids, cursor, height))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void get_candidates(vector<vector<size_t>> leaves)
    {
        vector<size_t> leaf;
        auto current_node = root;
        leaf.push_back(root->obj_id);
        for(auto& child : current_node->children)
        {
            leaf.push_back(child.second->obj_id);
        }
    }



    bool exists(vector<size_t>& candidate_ids)//for verification of method
    {
        auto current_node = root;
        for(size_t i = 1; i < height; ++i)
        {
            auto iter = current_node->children.find(candidate_ids[i]);
            if(iter != current_node->children.end())
            {
                current_node = iter->second;
            } else
            {
                return false;
            }
        }
        return true;
    }



    void add_candidate(vector<size_t>& candidate_ids)
    {
        size_t cursor = 1;
        auto branch_node = root->get_common_prefix(candidate_ids, cursor);
//        if(cursor == candidate_ids.size())
//        {
//            return;
//        }
        //candidate size 3, cursor 3????/

        //add suffix as child of branch_node
        branch_node->children[candidate_ids[cursor]] = new prefix_node(/*id_node_pointer
                                                                        ,*/ next(candidate_ids.begin(), cursor)
                                                                        , candidate_ids.end()
                                                                        , branch_node);

    };

    bool subset_update(vector<size_t>& candidate_ids, size_t& offset)
    {
        //DFS
        for(auto iter = root->children.begin(); iter != root->children.end();)
        {
            if(iter->second->obj_id > candidate_ids[offset+1])
            {
                return false;
            } else
            {
                if (iter->second->subset_update(candidate_ids, candidate_ids.size() - offset - 1, height/*,
                                                id_node_pointer*/))
                {
                    //iter->second->id_node_pointer_eraser(id_node_pointer, iter->second);
                    delete (iter->second);
                    root->children.erase(iter++);
                } else
                {
                    ++iter;
                }
            }
        }

        if(root->children.empty())
        {


            delete (root);
            return true;
        }

        return false;
    };

    size_t get_num_leaf()
    {
        size_t sum = 0;
        for(auto iter = root->children.begin(); iter != root->children.end(); ++iter)
        {
            sum += iter->second->get_num_leaf(height);
        }
        return sum;
    }

    void print_leaf()
    {

        cout << "depth: " << root->depth << ":" <<root->obj_id << endl;
        for(auto iter = root->children.begin(); iter != root->children.end(); ++iter)
        {
            iter->second->print_leaf(height);
        }
    }


};
//
//class prefix_tree_bucket
//{
//public:
//    map<size_t, prefix_tree*>>
//};
//
//class obj_length_tree
//{
//public:
//    map<size_t, map<size_t, vector<prefix_node*>>> pointer_bucket;
//    vector<prefix_tree> trees;
//    size_t thres_g;
//
//    obj_length_tree(vector<vector<obj*>>& frame_obj_ids, size_t _thres_g)
//    {
//        for(auto& grid : frame_obj_ids)
//        {
//            for(auto& obj : grid)
//            {
//                pointer_bucket[obj->id] = map<size_t, vector<prefix_node*>>();
//            }
//        }
//        trees = vector<prefix_tree>();
//        thres_g = _thres_g;
//    }
//
//
//
//    void add_candidate(vector<size_t>& candidate_ids)
//    {
//        auto p_bucket_iter = pointer_bucket[candidate_ids[0]].find(candidate_ids.size());
//        if(p_bucket_iter != pointer_bucket[candidate_ids[0]].end())
//        {
//
//        } else
//        {
//            pointer_bucket[candidate_ids[0]][candidate_ids.size()] = vector<prefix_node*>();
//
//        }
//    }
//



#endif //VIDEOGROUP_PREFIX_TREE_HPP
