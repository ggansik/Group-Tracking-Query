//
// Created by Hyunsik Yoon on 2022-07-05.
//

#include "intraframe_trie.hpp"


//void candidate_computation_proposed()
//{
//    //TODO: efficient candidate superset, subset search
//    //store the same-sized group candidates in a bucket
//    //subset check with larger group is unnecessary
//    //superset check with smaller group is unnecessary
//
//    //start from largest group bucket, erase the identical, subset of me(every bucket is a target)
//
//}
pair<double,double> prefix_get_ellipse_radius(const size_t& ycoord, const vector<double>& pivot_info)
{
    //long, short
    return make_pair(pivot_info[2] + pivot_info[0]*(ycoord-pivot_info[4])
            , pivot_info[3] + pivot_info[1]*(ycoord-pivot_info[4]));
}
bool prefix_group_test(vector<obj*>& candidate, obj* t_obj, const vector<double>& pivot_info)
{
    //check whether t_obj can be a member of group
    for(const auto& member : candidate)
    {

        auto m_radiuses = prefix_get_ellipse_radius(member->bb_center[1], pivot_info);
        auto t_radiuses = prefix_get_ellipse_radius(t_obj->bb_center[1], pivot_info);

        if(!ellipse_check(member, t_obj, m_radiuses.first, m_radiuses.second)
           || !ellipse_check(t_obj, member, t_radiuses.first, t_radiuses.second))
        {
            return false;
        }
    }
    return true;
};


void candidate_computation_no_recursive(vector<obj*>& t_objs
                                        , map<size_t, map<size_t, prefix_tree>> &maximal_group_set
                                        , const size_t &thres_g
                                        , const pair<double, double> &thres_d
                                        , vector<chrono::nanoseconds>& intraframe_time_stats)
{
    for(size_t i = thres_g; i <= t_objs.size();i++)
    {
        vector<bool> v(t_objs.size() - i, false);
        v.insert(v.end(), i, true);
        do{
            vector<size_t> candidate_ids;
            for(size_t k = 0; k < t_objs.size(); k++)
            {
                if(v[k])
                {
                    candidate_ids.push_back(t_objs[k]->id);
                }
            }
            candidate_verification(candidate_ids, maximal_group_set, thres_g, thres_d, intraframe_time_stats);
        }while(next_permutation(v.begin(), v.end()));
    }

}



void candidate_computation_recursive(/*vector<obj *> candidate
                                        , vector<size_t> candidate_ids*/
                                        vector<obj*>& candidate
                                        , vector<obj *>::iterator left_objs_begin
                                        , const vector<obj *>::iterator &left_objs_end
                                        , map<size_t, vector<vector<obj*>>> &groups
                                        , SL_detection_trie& trie
                                        , const size_t &thres_g
                                        , const vector<double> &pivot_info
                                        , vector<chrono::nanoseconds>& intraframe_time_stats)
{
    //branching
    for (; left_objs_begin != left_objs_end; ++left_objs_begin)
    {
        if (candidate.size() == 1)
        {
//            vector<obj *> branch = candidate;
//            vector<size_t> branch_ids = candidate_ids;
//            branch.push_back(*left_objs_begin);
//            branch_ids.push_back((*left_objs_begin)->id);

            vector<obj*> _candidate = candidate;
            _candidate.push_back(*left_objs_begin);

            candidate_computation_recursive(_candidate, left_objs_begin + 1, left_objs_end, groups, trie,
                                               thres_g, pivot_info,intraframe_time_stats);
        } else
        {
            if (prefix_group_test(candidate, *left_objs_begin, pivot_info))
            {
//                vector<obj *> branch = candidate;
//                vector<size_t> branch_ids = candidate_ids;
//                branch.push_back(*left_objs_begin);
//                branch_ids.push_back((*left_objs_begin)->id);
                vector<obj*> _candidate = candidate;
                _candidate.push_back(*left_objs_begin);

                candidate_computation_recursive(_candidate, left_objs_begin + 1, left_objs_end, groups, trie,
                                                    thres_g, pivot_info, intraframe_time_stats);
            }
        }
    }

    //TODO: is there any method to avoid copying the candidate_ids?
    if (candidate.size() >= thres_g)
    {
        sort(candidate.begin(), candidate.end(), [](obj* const & a, obj* const & b) -> bool { return a->id < b->id;});

//        auto matched = groups.find(candidate.size());
//        if(matched != groups.end())
//        {
//            matched->second.push_back(candidate);
//        }else
//        {
//            auto result = groups.insert(make_pair(candidate.size(), vector<vector<obj*>>()));
//            result.first->second.push_back(candidate);
//        }
        chrono::steady_clock::time_point maximal_group_computation_start = chrono::steady_clock::now();
        trie.unsorted_insert(candidate, intraframe_time_stats);
        intraframe_time_stats[3] += chrono::steady_clock::now() - maximal_group_computation_start;
    }


    return;
}


void candidate_verification(vector<size_t>& candidate_ids
        , map<size_t, map<size_t, prefix_tree>> &buckets
        , const size_t &thres_g
        , const pair<double, double> &thres_d
        , vector<chrono::nanoseconds>& intraframe_time_stats)
{

//        for(const auto& id : candidate_ids)
//        {
//            cout << id << ",";
//        }
//        cout << endl;

    //if there is no candidate in bucket


    if (buckets.size() == 0)
    {

        auto iter = buckets.insert(make_pair(candidate_ids.size(), map<size_t, prefix_tree>()));
        iter.first->second[candidate_ids[0]] = prefix_tree(candidate_ids);

        return;
    }

    bool unique = true;
    prefix_node *last_node = nullptr; //to store the most common prefix


    //get iterator which has smallest value among no smaller than candidate size
    //find superset start from longest bucket
    //buckets: map<length, <root_obj, tree>>



    for (auto iter = buckets.rbegin(); iter != buckets.rend(); ++iter)
    {
        if(iter->first >= candidate_ids.size())
        {
            //superset search in the trees which have smaller rootid than candidate_id[0]
            for(auto tree_iter = iter->second.begin(); tree_iter != iter->second.end();++tree_iter)
            {
                if(tree_iter->second.root->obj_id <= candidate_ids[0])
                {
                    if(tree_iter->second.no_pointer_contains(candidate_ids))
                    {
                        unique = false;
                        break;
                    }
                } else
                {
                    break;
                }
            }
            if(!unique)
            {
                break;
            }
        } else
        {
            break;
        }
    }


    if (unique)
    {

        //bucket[length][obj_id]
        auto iter = buckets.find(candidate_ids.size());
        if(iter != buckets.end())
        {

//              auto length_iter = buckets.find(candidate_ids.size());
            auto tree_iter = iter->second.find(candidate_ids[0]);

            if (tree_iter != iter->second.end()) //same start key is exists
            {
                tree_iter->second.add_candidate(candidate_ids);
            } else
            {
                iter->second.insert(make_pair(candidate_ids[0], prefix_tree(candidate_ids)));
            }
        } else
        {
            auto inserted_iter = buckets.insert(make_pair(candidate_ids.size(), map<size_t, prefix_tree>()));
            inserted_iter.first->second[candidate_ids[0]] = prefix_tree(candidate_ids);
            iter = inserted_iter.first; //to make iter pointing same-sized bucket
        }

        //auto iter = buckets.lower_bound(candidate_ids.size());
//            auto iter = buckets.find(candidate_ids.size());
//            if(iter != buckets.end())
//            {
//                if (iter->first == candidate_ids.size()) //same sized bucket is exists
//                {
////                    auto length_iter = buckets.find(candidate_ids.size());
//                    auto tree_iter = iter->second.find(candidate_ids[0]);
//
//                    if (tree_iter != iter->second.end()) //same start key is exists
//                    {
//                        tree_iter->second.add_candidate(candidate_ids);
//                    } else
//                    {
//                        iter->second.insert(make_pair(candidate_ids[0], prefix_tree(candidate_ids)));
//                    }
//                } else //larger bucket is exists
//                {
//                    auto inserted_iter = buckets.insert(make_pair(candidate_ids.size(), map<size_t, prefix_tree>()));
//                    inserted_iter.first->second[candidate_ids[0]] = prefix_tree(candidate_ids);
//                    iter = inserted_iter.first; //to make iter pointing same-sized bucket
//                }
//
//            } else
//            {
//                auto inserted_iter = buckets.insert(make_pair(candidate_ids.size(), map<size_t, prefix_tree>()));
//                inserted_iter.first->second[candidate_ids[0]] = prefix_tree(candidate_ids);
//                iter = inserted_iter.first; //to make iter pointing same-sized bucket
//            }

        //erase the subsets from no larger sized bucket
        for(auto bucket_iter = buckets.begin(); bucket_iter != iter; ++bucket_iter)
        {//length thres_g ~ candidate.size()-1

            //check the tries which have possibilities to contain subset of me
            for(size_t i = 0; i < candidate_ids.size() - bucket_iter->first + 1; ++i)
            {
                auto trie_iter = bucket_iter->second.find(candidate_ids[i]);

                if(trie_iter != bucket_iter->second.end())
                {
                    //if current trie is empty, remove trie
                    if(trie_iter->second.subset_update(candidate_ids, i))
                    {
                        bucket_iter->second.erase(trie_iter++);
                    }
                }
            }

            //if current length bucket is empty, remove bucket
            if(bucket_iter->second.empty())
            {
                buckets.erase(bucket_iter++);
            }
        }
    }
}




void intraframe_processing_trie(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const vector<double>& pivot_info
        , const size_t& thres_g
        , SL_detection_trie& trie
        , map<size_t, vector<vector<obj*>>>& groups
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
        get_target_grid_rectangle(target_grid, grid_num, grid_row, grid_col, pivot_info, grid_size, video_size);
        intraframe_time_stats[0] += chrono::steady_clock::now() - target_grid_time_start;
//get_target_grid(target_grid, grid_num, grid_row, grid_col,ellipse_grid_divider);
//        get_target_grid_chain(target_grid, grid_num, grid_row, grid_col);


        for (auto &seed_obj : seed_grid)
        {
            valid_obj_time_start = chrono::steady_clock::now();
            //calculate a relative long,short radius of seed_obj-centered ellipse
            auto relative_radiuses = prefix_get_ellipse_radius(seed_obj->bb_center[1], pivot_info);
            double relative_long_radius = relative_radiuses.first;
            double relative_short_radius = relative_radiuses.second;

            //compute the objects in target grid that located in seed_obj-centered ellipse
            vector<obj *> t_objs;
            for (auto &t_grid_num : target_grid)
            {
            //iterating every target obj except seed obj
                for (auto &candiate_obj : a_frame_obj[t_grid_num])
                {
                    if (candiate_obj->id != seed_obj->id)
                    {
                        auto t_relative_radiuses = prefix_get_ellipse_radius(candiate_obj->bb_center[1], pivot_info);
                        double t_relative_long_radius = t_relative_radiuses.first;
                        double t_relative_short_radius = t_relative_radiuses.second;

                        if (ellipse_check(seed_obj, candiate_obj, relative_long_radius, relative_short_radius)
                            && ellipse_check(candiate_obj, seed_obj, t_relative_long_radius, t_relative_short_radius))
                        {
                            t_objs.push_back(candiate_obj);
                        }
                    }
                }
            }

            intraframe_time_stats[1] += chrono::steady_clock::now() - valid_obj_time_start;



            //print_intraframe(seed_obj, t_objs);
            if (t_objs.size() < thres_g - 1) //if impossible to make thres_g - sized group
            {
                continue; //test the next seed
            }

            candidate_computation_time_start = chrono::steady_clock::now();

            vector<obj *> candidate = {seed_obj};
//            vector<size_t> candidate_ids = {seed_obj->id};

//            candidate_computation_naive_vector(candidate, candidate_ids
//                    , t_objs.begin(), t_objs.end()
//                    ,candidate_map, thres_g, thres_d);


            candidate_computation_recursive(/*candidate
                                            , candidate_ids*/
                                            candidate
                                            , t_objs.begin()
                                            , t_objs.end()
                                            , groups
                                            , trie
                                            , thres_g
                                            , pivot_info
                                            , intraframe_time_stats);


            intraframe_time_stats[2] += chrono::steady_clock::now() - candidate_computation_time_start;
        }

        grid_num++;
    }


//    for(auto iter = groups.rbegin(); iter != groups.rend(); ++iter)
//    {
//        for(auto& group : iter->second)
//        {
//            trie.length_sorted_insert(group, intraframe_time_stats);
//        }
//    }

//    chrono::steady_clock::time_point maximal_group_computation_start = chrono::steady_clock::now();
//    for(auto iter = groups.begin(); iter != groups.end(); ++iter)
//    {
//        for(auto& group : iter->second)
//        {
//            trie.ascending_ordered_insert(group, intraframe_time_stats);
//        }
//    }
//    intraframe_time_stats[3] += chrono::steady_clock::now() - maximal_group_computation_start;



//    cout << "------------------------------" << endl;
//    for(const auto& a_candidate : trie.maximal_groups)
//    {
//        for(const auto& id : a_candidate)
//        {
//            cout << id << ",";
//        }
//        cout << endl;
//    }

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


