//
// Created by Hyunsik Yoon on 2022-07-05.
//

#include "intraframe_proposed.hpp"

bool group_test(vector<obj *> &candidate, obj *t_obj, const pair<double, double> &thres_d)
{
    //check whether t_obj can be a member of group
    for (const auto &member : candidate)
    {
        double m_relative_long_radius = thres_d.first * (double) (member->bb_center[1]);
        double m_relative_short_radius = m_relative_long_radius * thres_d.second;

        double t_relative_long_radius = thres_d.first * (double) (t_obj->bb_center[1]);
        double t_relative_short_radius = t_relative_long_radius * thres_d.second;

        if (!ellipse_check(member, t_obj, m_relative_long_radius, m_relative_short_radius)
            || !ellipse_check(t_obj, member, t_relative_long_radius, t_relative_short_radius))
        {
            return false;
        }
    }
    return true;
}

void candidate_computation_proposed()
{
    //TODO: efficient candidate superset, subset search
    //store the same-sized group candidates in a bucket
    //subset check with larger group is unnecessary
    //superset check with smaller group is unnecessary

    //start from largest group bucket, erase the identical, subset of me(every bucket is a target)

}

void candidate_computation_prefix_bucket(vector<obj *> candidate, vector<size_t> candidate_ids,
                                         vector<obj *>::iterator left_objs_begin,
                                         const vector<obj *>::iterator &left_objs_end,
                                         vector<vector<prefix_tree_map>> &buckets,
                                         map<size_t, size_t> &bucket_cand_length, const size_t &thres_g,
                                         const pair<double, double> &thres_d)
{
    //branching
    for (; left_objs_begin != left_objs_end; ++left_objs_begin)
    {
        if (candidate.size() == 1)
        {
            vector<obj *> branch = candidate;
            vector<size_t> branch_ids = candidate_ids;
            branch.push_back(*left_objs_begin);
            branch_ids.push_back((*left_objs_begin)->id);

            candidate_computation_prefix_vector(branch, branch_ids, left_objs_begin + 1, left_objs_end, buckets,
                                                bucket_cand_length, thres_g, thres_d);
        } else
        {
            if (group_test(candidate, *left_objs_begin, thres_d))
            {
                vector<obj *> branch = candidate;
                vector<size_t> branch_ids = candidate_ids;
                branch.push_back(*left_objs_begin);
                branch_ids.push_back((*left_objs_begin)->id);

                candidate_computation_prefix_vector(branch, branch_ids, left_objs_begin + 1, left_objs_end, buckets,
                                                    bucket_cand_length, thres_g, thres_d);
            }
        }
    }

    if (candidate.size() >= thres_g)
    {
        sort(candidate_ids.begin(), candidate_ids.end());

//        for(const auto& id : candidate_ids)
//        {
//            cout << id << ",";
//        }
//        cout << endl;

        //if there is any candidate in bucket
        if (buckets.size() == 0)
        {
            bucket_cand_length[candidate.size()] = 0;
            buckets.push_back(vector<prefix_tree_map>());
            buckets.back().push_back(prefix_tree_map(candidate_ids.begin(), candidate_ids.end()));

            return;
        }

        bool unique = true;
        prefix_node_map *last_node = nullptr; //to store the most common prefix

        //get iterator which has smallest value over no smaller than candidate size
        //find superset start from longest bucket
        for (auto iter = bucket_cand_length.rbegin(); iter != bucket_cand_length.rend(); ++iter)
        {
            //superset or candidate can be already exists when the candidate_id is not longer than candidate in bucket
            if (iter->first >= candidate_ids.size())
            {
                //bucket iteration
                for (auto &tree : buckets[iter->second]) //TODO: make a tree hashmap with root key
                {
                    if (tree.traverse(candidate_ids, last_node))
                    {
                        unique = false;

                        break;
                    }
                }
                if (!unique)
                {
                    break;
                }

            } else
            {
                break;
            }
        }

        auto entry = bucket_cand_length.rbegin();
        for (; entry != bucket_cand_length.rend(); ++entry)
        {
            if (entry->first <= candidate.size())
            {
                break;
            }
        }

        if (unique)
        {
            if (entry == bucket_cand_length.rend()) // candidate.size is smaller than the smallest bucket
            {
                bucket_cand_length[candidate.size()] = buckets.size();
                buckets.push_back(vector<prefix_tree_map>());
                buckets.back().push_back(prefix_tree_map(candidate_ids.begin(), candidate_ids.end()));
            } else
            {
                if (entry->first == candidate.size()) //same-sized bucket is exists
                {
                    for (auto &tree : buckets[entry->second]) //TODO: make tree hashmap with start key
                    {
                        if (tree.root->obj_id == candidate_ids[0])
                        {
                            tree.add_candidate(candidate_ids, last_node);
                            break;
                        }
                    }
                    //TODO: exception handling when the case of matched tree
                    if (++entry == bucket_cand_length.rend())
                    {
                        return;
                    }
                } else //add new bucket
                {
                    bucket_cand_length[candidate.size()] = buckets.size();
                    buckets.push_back(vector<prefix_tree>());
                    buckets.back().push_back(prefix_tree(candidate_ids));

                }
                //erase the subsets from no larger sized bucket
                for (; entry != bucket_cand_length.rend(); ++entry)
                {
                    if (entry->first < candidate_ids.size())
                    {
                        for (auto iter = buckets[entry->second].begin(); iter != buckets[entry->second].end();)
                        {
                            bool removed = iter->traverse_remove(candidate_ids, candidate_ids.size());
                            if (!removed)
                            {
                                ++iter;
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}

void candidate_computation_bucket_only(vector<obj *> candidate, vector<size_t> candidate_ids,
                                       vector<obj *>::iterator left_objs_begin,
                                       const vector<obj *>::iterator &left_objs_end,
                                       vector<vector<size_t>> &candidate_map, vector<vector<vector<size_t>>> &buckets,
                                       map<size_t, size_t> &bucket_cand_length, const size_t &thres_g,
                                       const pair<double, double> &thres_d)
{
//branching
    for (; left_objs_begin != left_objs_end; ++left_objs_begin)
    {
        if (candidate.size() == 1)
        {
            vector<obj *> branch = candidate;
            vector<size_t> branch_ids = candidate_ids;
            branch.push_back(*left_objs_begin);
            branch_ids.push_back((*left_objs_begin)->id);

            candidate_computation_test_vector(branch, branch_ids, left_objs_begin + 1, left_objs_end, candidate_map,
                                              buckets, bucket_cand_length, thres_g, thres_d);

        } else
        {
            if (group_test(candidate, *left_objs_begin, thres_d))
            {
                vector<obj *> branch = candidate;
                vector<size_t> branch_ids = candidate_ids;
                branch.push_back(*left_objs_begin);
                branch_ids.push_back((*left_objs_begin)->id);

                candidate_computation_test_vector(branch, branch_ids, left_objs_begin + 1, left_objs_end, candidate_map,
                                                  buckets, bucket_cand_length, thres_g, thres_d);
            }
        }
    }

    if (candidate.size() >= thres_g)
    {
        sort(candidate_ids.begin(), candidate_ids.end());

        //        for(const auto& id : candidate_ids)
        //        {
        //            cout << id << ",";
        //        }
        //        cout << endl;

        //if there is any candidate in bucket
        if (buckets.size() == 0)
        {
            bucket_cand_length[candidate.size()] = 0;
            buckets.push_back(vector<vector<size_t>>());
            buckets.back().push_back(candidate_ids);

            return;
        }

        bool unique = true;


        //        auto entry = bucket_cand_length.find(candidate.size());


        //get iterator which has smallest value over no smaller than candidate size

        //find superset start from longest bucket
        for (auto iter = bucket_cand_length.rbegin(); iter != bucket_cand_length.rend(); ++iter)
        {
            //superset or candidate can be already exists when the candidate_id is no longer than candidate in bucket
            if (iter->first >= candidate_ids.size())
            {
                //bucket iteration
                for (auto &t_candidate : buckets[iter->second])
                {
                    //TODO: make a prefix tree for each bucket

                    if (includes(t_candidate.begin(), t_candidate.end(), candidate_ids.begin(), candidate_ids.end()))
                    {
                        unique = false;
                        break;
                    }
                }
                if (!unique)
                {
                    break;
                }
            } else
            {
                break;
            }
        }

        auto entry = bucket_cand_length.rbegin();
        for (; entry != bucket_cand_length.rend(); ++entry)
        {
            if (entry->first <= candidate.size())
            {
                break;
            }
        }

        if (unique)
        {
            if (entry == bucket_cand_length.rend()) // candidate.size is smaller than the smallest bucket
            {
                bucket_cand_length[candidate.size()] = buckets.size();
                buckets.push_back(vector<vector<size_t>>());
                buckets.back().push_back(candidate_ids);
            } else
            {
                if (entry->first == candidate.size()) //same-sized bucket is exists
                {
                    buckets[entry->second].push_back(candidate_ids);
                    if (++entry == bucket_cand_length.rend())
                    {
                        return;
                    }
                } else //add new bucket
                {
                    bucket_cand_length[candidate.size()] = buckets.size();
                    buckets.push_back(vector<vector<size_t>>());
                    buckets.back().push_back(candidate_ids);
                }

                //erase the subsets from no larger sized bucket
                for (; entry != bucket_cand_length.rend(); ++entry)
                {
                    if (entry->first < candidate_ids.size())
                    {
                        for (auto iter = buckets[entry->second].begin(); iter != buckets[entry->second].end();)
                        {
                            if (includes(candidate_ids.begin(), candidate_ids.end(), (*iter).begin(), (*iter).end()))
                            {
                                buckets[entry->second].erase(iter);
                            } else
                            {
                                ++iter;
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}




void intraframe_processing_proposed(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , vector<vector<size_t>>& candidate_map
        , vector<vector<prefix_tree_map>>& intra_candidate_bucket
        , map<size_t,size_t>& bucket_cand_length
        , const size_t& method
        , vector<chrono::nanoseconds>& intraframe_time_stats)
{
    chrono::steady_clock::time_point target_grid_time_start;
    chrono::steady_clock::time_point valid_obj_time_start;
    chrono::steady_clock::time_point candidate_computation_time_start;

//calculate num of grids for each direction
    size_t grid_row = (size_t) ceil((double) video_size.first / (double) grid_size.first);
    size_t grid_col = (size_t) ceil((double) video_size.second / (double) grid_size.second);

    size_t grid_num = 0;
    for (auto &seed_grid : a_frame_obj)
    {
        vector<size_t> target_grid;
        target_grid_time_start = chrono::steady_clock::now();
        get_target_grid_rectangle(target_grid, grid_num, grid_row, grid_col, thres_d, grid_size, video_size);
        intraframe_time_stats[0] += chrono::steady_clock::now() - target_grid_time_start;
//get_target_grid(target_grid, grid_num, grid_row, grid_col,ellipse_grid_divider);
//        get_target_grid_chain(target_grid, grid_num, grid_row, grid_col);


        for (auto &seed_obj : seed_grid)
        {

            valid_obj_time_start = chrono::steady_clock::now();
//calculate a relative long,short radius of seed_obj-centered ellipse
            double relative_long_radius = thres_d.first * (double) seed_obj->bb_center[1];
            double relative_short_radius = relative_long_radius * thres_d.second;

//compute the objects in target grid that located in seed_obj-centered ellipse
            vector<obj *> t_objs;
            for (auto &t_grid_num : target_grid)
            {
//iterating every target obj except seed obj
                for (auto &candiate_obj : a_frame_obj[t_grid_num])
                {
                    if (candiate_obj->id != seed_obj->id)
                    {
                        double t_relative_long_radius = thres_d.first * (double) candiate_obj->bb_center[1];
                        double t_relative_short_radius = t_relative_long_radius * thres_d.second;

                        if (ellipse_check(seed_obj, candiate_obj, relative_long_radius, relative_short_radius)
                            && ellipse_check(candiate_obj, seed_obj, t_relative_long_radius, t_relative_short_radius))
                        {
                            t_objs.push_back(candiate_obj);
                        }
                    }
                }
            }
            intraframe_time_stats[1] += chrono::steady_clock::now() - valid_obj_time_start;



//          print_intraframe(seed_obj, t_objs);
            if (t_objs.size() < thres_g - 1) //if impossible to make thres_g - sized group
            {
                continue; //test the next seed
            }

            candidate_computation_time_start = chrono::steady_clock::now();

            vector<obj *> candidate = {seed_obj};
            vector<size_t> candidate_ids = {seed_obj->id};
//            candidate_computation_naive_vector(candidate, candidate_ids
//                    , t_objs.begin(), t_objs.end()
//                    ,candidate_map, thres_g, thres_d);
            if (method == 1)
            {
//                candidate_computation_test_vector(candidate, candidate_ids
//                        , t_objs.begin(), t_objs.end()
//                        ,candidate_map, intra_candidate_bucket, bucket_cand_length,  thres_g, thres_d);
            } else if (method == 2) //prefix_tree with bucket
            {
//                candidate_computation_prefix_vector(candidate, candidate_ids
//                        , t_objs.begin(), t_objs.end()
//                        ,intra_candidate_bucket, bucket_cand_length,  thres_g, thres_d);
            } else
            {
                return;
            }

            intraframe_time_stats[2] += chrono::steady_clock::now() - candidate_computation_time_start;
        }

        grid_num++;
    }

//    for(const auto& a_candidate : candidate_map)
//    {
//        for(const auto& id : a_candidate)
//        {
//            cout << id << ",";
//        }
//        cout << endl;
//    }
//
//    print_group_candidates(candidate_map);
}

/*
void intraframe_processing_multilevel(vector<grid>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const pair<double, double>& thres_d
        , const size_t& thres_g
        , vector<vector<size_t>>& candidate_map)
{
    //calculate num of grids for each direction
    size_t grid_row = (size_t) ceil((double)video_size.first/(double)grid_size.first);
    size_t grid_col = (size_t) ceil((double)video_size.second/(double)grid_size.second);

    size_t grid_num = 0;
    for(auto& seed_grid : a_frame_obj)
    {
        vector<size_t> target_grid;
        get_target_grid(target_grid, grid_num, grid_row, grid_col);

        for(auto seed_obj : seed_grid.objects)
        {
            //calculate a relative long,short radius of seed_obj-centered ellipse
            double relative_long_radius = thres_d.first * (double)seed_obj->bb_width;
            double relative_short_radius = relative_long_radius * thres_d.second;

            //compute the objects in target grid that located in seed_obj-centered ellipse
            vector<obj*> t_objs;
            for(auto& t_grid_num : target_grid)
            {
                //iterating every target obj except seed obj
                for(auto& candiate_obj : a_frame_obj[t_grid_num].objects)
                {
                    if(candiate_obj->id != seed_obj->id)
                    {
                        double t_relative_long_radius = thres_d.first * (double)candiate_obj->bb_width;
                        double t_relative_short_radius = t_relative_long_radius * thres_d.second;

                        if (ellipse_check(seed_obj, candiate_obj, relative_long_radius, relative_short_radius)
                            && ellipse_check(candiate_obj, seed_obj, t_relative_long_radius, t_relative_short_radius))
                        {
                            t_objs.push_back(candiate_obj);
                        }
                    }
                }
            }



//            print_intraframe(seed_obj, t_objs);
            if(t_objs.size() < thres_g - 1) //if impossible to make thres_g - sized group
            {
                continue; //test the next seed
            }

            vector<obj*> candidate = {&seed_obj};
            vector<size_t> candidate_ids = {seed_obj.id};
            candidate_computation_naive_vector(candidate, candidate_ids
                    , t_objs.begin(), t_objs.end()
                    ,candidate_map, thres_g, thres_d);

        }
        grid_num++;
    }
}
 */


