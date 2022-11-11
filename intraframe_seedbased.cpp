//
// Created by Hyunsik Yoon on 2022-07-27.
//
#include "intraframe_seedbased.hpp"


bool obj_id_includes(vector<size_t>::iterator first1, vector<size_t>::iterator last1,
                     vector<obj*>::iterator first2,  vector<obj*>::iterator last2)
{
    for (; first2 != last2; ++first1)
    {
        if (first1 == last1 || (*first2)->id < (*first1))
            return false;
        if (!(*first1, (*first2)->id))
            ++first2;
    }
    return true;
}
pair<double,double> seed_get_ellipse_radius(const size_t& ycoord, const vector<double>& pivot_info)
{
    //long, short
    return make_pair(pivot_info[2] + pivot_info[0]*(ycoord-pivot_info[4])
            , pivot_info[3] + pivot_info[1]*(ycoord-pivot_info[4]));
}
bool seed_group_test(vector<obj*>& candidate, obj* t_obj, const vector<double>& pivot_info)
{
    //check whether t_obj can be a member of group
    for(const auto& member : candidate)
    {

        auto m_radiuses = seed_get_ellipse_radius(member->bb_center[1], pivot_info);
        auto t_radiuses = seed_get_ellipse_radius(t_obj->bb_center[1], pivot_info);

        if(!ellipse_check(member, t_obj, m_radiuses.first, m_radiuses.second)
           || !ellipse_check(t_obj, member, t_radiuses.first, t_radiuses.second))
        {
            return false;
        }
    }
    return true;
};

void candidate_computation_recursive(/*vector<obj *> candidate
                                        , vector<size_t> candidate_ids*/
        vector<obj*>& candidate
        , vector<obj *>::iterator left_objs_begin
        , const vector<obj *>::iterator &left_objs_end
        , const size_t& grid_group_cursor
        , vector<vector<obj*>>& maximal_groups
        , const size_t groups_current_seed_idx
        , const size_t &thres_g
        , const vector<double>& pivot_info
        , vector<chrono::nanoseconds>& time_stats
        ,vector<size_t>& memory_consumption)
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
            _candidate.emplace_back(*left_objs_begin);

            candidate_computation_recursive(_candidate, left_objs_begin + 1, left_objs_end, grid_group_cursor
                                            , maximal_groups,groups_current_seed_idx, thres_g, pivot_info,time_stats,memory_consumption);

        } else
        {
            if (seed_group_test(candidate, *left_objs_begin, pivot_info))
            {
//                vector<obj *> branch = candidate;
//                vector<size_t> branch_ids = candidate_ids;
//                branch.push_back(*left_objs_begin);
//                branch_ids.push_back((*left_objs_begin)->id);
                vector<obj*> _candidate = candidate;
                _candidate.emplace_back(*left_objs_begin);

                candidate_computation_recursive(_candidate, left_objs_begin + 1, left_objs_end, grid_group_cursor
                                                , maximal_groups, groups_current_seed_idx, thres_g, pivot_info, time_stats,memory_consumption);
            } else
            {
                continue;
            }
        }
    }



    if (candidate.size() >= thres_g)
    {

//        chrono::steady_clock::time_point group_sort_time = chrono::steady_clock::now();
        sort(candidate.begin(), candidate.end(), [](obj* const & a, obj* const & b) -> bool { return a->id < b->id;});
//        time_stats[3] += chrono::steady_clock::now() - group_sort_time;

        size_t recursion_memory = 0;

        for(size_t i = 0; i < candidate.size(); i++)
        {
            recursion_memory += i*sizeof(obj*);
        }

        if(memory_consumption[0] < recursion_memory)
        {
            memory_consumption[0] = recursion_memory;
        }

//        chrono::steady_clock::time_point superset_search_time = chrono::steady_clock::now();
        bool unique = true;


//        chrono::steady_clock::time_point mg_candidate_set_update = chrono::steady_clock::now();
        /* code for object-local */
        size_t set_ops_per_group = 0;
        memory_consumption[9]++;
        if(groups_current_seed_idx != maximal_groups.size())
        {

            for (size_t current = groups_current_seed_idx; current < maximal_groups.size(); current++)
            {
                set_ops_per_group++;
                if(maximal_groups[current].size()>=candidate.size())
                {
                    if (includes((maximal_groups[current]).begin(), (maximal_groups[current]).end(), candidate.begin(),
                                 candidate.end(), [](obj *const &a, obj *const &b) -> bool
                                 { return a->id < b->id; })) //i'm a subset or identical to the iter
                    {
                        unique = false;
                        break;
                    }
                }
            }
        }
        /* code for object-local end */


        /* code for grid-local */
        if(unique)
        {
            if (grid_group_cursor != maximal_groups.size())
            {
                for (size_t current = grid_group_cursor; current < groups_current_seed_idx; current++)
                {
                    set_ops_per_group++;
                    if (maximal_groups[current].size() >= candidate.size())
                    {
                        if (includes((maximal_groups[current]).begin(), (maximal_groups[current]).end(),
                                     candidate.begin(),
                                     candidate.end(), [](obj *const &a, obj *const &b) -> bool
                                     { return a->id < b->id; })) //i'm a subset or identical to the iter
                        {
                            unique = false;
                            break;
                        }
                    }
                }
            }
        }
        /* code for grid-local end*/

        /* code for global-MG */
        if(unique)
        {
            for (size_t current = 0; current < grid_group_cursor; current++)
            {
                set_ops_per_group++;
                if(maximal_groups[current].size()>=candidate.size())
                {
                    if (includes((maximal_groups[current]).begin(), (maximal_groups[current]).end(), candidate.begin(), candidate.end(),
                                 [](obj *const &a, obj *const &b) -> bool
                                 { return a->id < b->id; })) //i'm a subset or identical to the iter
                    {
                        unique = false;
                        break;
                    }
                }
            }
        }
        /* code for global-MG end*/
        if (memory_consumption[6] > set_ops_per_group)
        {
            memory_consumption[6] = set_ops_per_group;
        }

        memory_consumption[7] += set_ops_per_group;

        if (memory_consumption[8] < set_ops_per_group)
        {
            memory_consumption[8] = set_ops_per_group;
        }

//        time_stats[3] += chrono::steady_clock::now() - superset_search_time;


//        chrono::steady_clock::time_point insertion_time = chrono::steady_clock::now();
        if(unique)
        {

            maximal_groups.emplace_back(candidate);
//            for(const auto& id : candidate)
//            {
//                cout << id->id << ",";
//            }
//            cout << endl;
        }
//        time_stats[3] += chrono::steady_clock::now() - mg_candidate_set_update;
//        time_stats[7] += chrono::steady_clock::now() - insertion_time;
    }
    return;
}



void intraframe_processing_seedbased(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , vector<vector<obj*>>& maximal_groups
        , const vector<double>& pivot_info
        , const size_t& thres_g
        , vector<chrono::nanoseconds>& intraframe_time_stats
        , vector<size_t>& memory_consumption)
{
    chrono::steady_clock::time_point target_grid_time_start;
    chrono::steady_clock::time_point valid_obj_time_start;
    chrono::steady_clock::time_point candidate_computation_time_start;

//calculate num of grids for each direction
    size_t grid_row = (size_t) ceil((double) video_size.first / (double) grid_size.first);
    size_t grid_col = (size_t) ceil((double) video_size.second / (double) grid_size.second);
//    size_t grid_row = grid_size.first;
//    size_t grid_col = (size_t) ceil((double) video_size.second/((double) video_size.first / (double) grid_size.first));
    size_t grid_num = 0;
    for (auto &seed_grid : a_frame_obj)
    {
        vector<size_t> target_grid;
//        target_grid_time_start = chrono::steady_clock::now();
        get_target_grid_rectangle(target_grid, grid_num, grid_row, grid_col, pivot_info, grid_size, video_size);
//        intraframe_time_stats[0] += chrono::steady_clock::now() - target_grid_time_start;
        //get_target_grid(target_grid, grid_num, grid_row, grid_col,ellipse_grid_divider);
        //        get_target_grid_chain(target_grid, grid_num, grid_row, grid_col);


        size_t grid_group_cursor = maximal_groups.size();
        for (auto seed_iter = seed_grid.begin(); seed_iter != seed_grid.end(); )
        {
//            valid_obj_time_start = chrono::steady_clock::now();
            //calculate a relative long,short radius of seed_obj-centered ellipse
            auto relative_radiuses = seed_get_ellipse_radius((*seed_iter)->bb_center[1], pivot_info);
            double relative_long_radius = relative_radiuses.first;
            double relative_short_radius = relative_radiuses.second;

            //compute the objects in target grid that located in seed_obj-centered ellipse
            vector<obj *> t_objs;
            for (const auto &t_grid_num : target_grid)
            {
            //iterating every target obj except seed obj
                for (const auto &candiate_obj : a_frame_obj[t_grid_num])
                {
                    if (candiate_obj->id != (*seed_iter)->id)
                    {
                        auto t_relative_radiuses = seed_get_ellipse_radius(candiate_obj->bb_center[1], pivot_info);
                        double t_relative_long_radius = t_relative_radiuses.first;
                        double t_relative_short_radius = t_relative_radiuses.second;

                        if (ellipse_check((*seed_iter), candiate_obj, relative_long_radius, relative_short_radius)
                            && ellipse_check(candiate_obj, (*seed_iter), t_relative_long_radius, t_relative_short_radius))
                        {
                            t_objs.push_back(candiate_obj);
                        }
                    }
                }
            }

//            intraframe_time_stats[1] += chrono::steady_clock::now() - valid_obj_time_start;


            //print_intraframe(seed_obj, t_objs);
//            cerr << "hello" << endl;
            if (t_objs.size() < thres_g - 1) //if impossible to make thres_g - sized group
            {
//                cerr << t_objs.size() << endl;

                seed_grid.erase(seed_iter);
                continue; //test the next seed
            } else
            {
//                cerr << "a seed " << endl;

//                candidate_computation_time_start = chrono::steady_clock::now();
//                cerr << "a seed " << endl;

                vector<obj *> candidate = {(*seed_iter)};
                //            vector<size_t> candidate_ids = {seed_obj->id};

                //            candidate_computation_naive_vector(candidate, candidate_ids
                //                    , t_objs.begin(), t_objs.end()
                //                    ,candidate_map, thres_g, thres_d);
//                cerr << "a seed " << endl;

                candidate_computation_recursive(/*candidate
                                            , candidate_ids*/
                        candidate, t_objs.begin(), t_objs.end(), grid_group_cursor,maximal_groups, maximal_groups.size(), thres_g, pivot_info,
                        intraframe_time_stats, memory_consumption);

//                cerr << "a seed end" << endl;
//                intraframe_time_stats[2] += chrono::steady_clock::now() - candidate_computation_time_start;

                seed_grid.erase(seed_iter);
            }
//            cout << "next seed -------------------------------------------------------------------------" << endl;
        }

        grid_num++;
    }
}