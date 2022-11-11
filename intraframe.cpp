//
// Created by Hyunsik Yoon on 2022-04-13.
//

#include "intraframe.hpp"

// TODO:  Inter-frame processing
//          1. Group Candidate computation
//              option 1: ellipse - seed based
//              option 2: ellipse - target grid based
//          2. Occlusion map based Candidate Pruning

pair<double,double> naive_get_ellipse_radius(const size_t& ycoord, const vector<double>& pivot_info)
{
    //long, short
    return make_pair(pivot_info[2] + pivot_info[0]*(ycoord-pivot_info[4])
            , pivot_info[3] + pivot_info[1]*(ycoord-pivot_info[4]));
}
bool naive_group_test(vector<obj*>& candidate, obj* t_obj, const vector<double>& pivot_info)
{
    //check whether t_obj can be a member of group
    for(const auto& member : candidate)
    {

        auto m_radiuses = naive_get_ellipse_radius(member->bb_center[1], pivot_info);
        auto t_radiuses = naive_get_ellipse_radius(t_obj->bb_center[1], pivot_info);

        if(!ellipse_check(member, t_obj, m_radiuses.first, m_radiuses.second)
           || !ellipse_check(t_obj, member, t_radiuses.first, t_radiuses.second))
        {
            return false;
        }
    }
    return true;
};



void candidate_computation_naive_vector(vector<obj*>& candidate
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        , vector<vector<obj*>>& maximal_groups
        , const size_t& thres_g
        , const vector<double>& pivot_info
        , vector<chrono::nanoseconds>& time_stats
        , vector<size_t>& memory_consumption)
{
    //branching
    for (; left_objs_begin != left_objs_end; ++left_objs_begin)
    {
        if(candidate.size() == 1)
        {
            vector<obj*> branch = candidate;
            branch.push_back(*left_objs_begin);

            candidate_computation_naive_vector(branch
                    , left_objs_begin + 1, left_objs_end
                    , maximal_groups, thres_g, pivot_info ,time_stats, memory_consumption);
        }else{
            if(naive_group_test(candidate, *left_objs_begin, pivot_info))
            {
                vector<obj*> branch = candidate;
                branch.push_back(*left_objs_begin);

                candidate_computation_naive_vector(branch
                        , left_objs_begin + 1, left_objs_end
                        , maximal_groups, thres_g, pivot_info, time_stats, memory_consumption);
            }
        }
    }

    if(candidate.size() >= thres_g)
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



        bool unique = true;


        size_t set_ops_per_group = 0;
        memory_consumption[9]++;
//        chrono::steady_clock::time_point MGtime = chrono::steady_clock::now();
        for(auto iter = maximal_groups.begin(); iter != maximal_groups.end();)
        {
            set_ops_per_group++;
            //find subsets of me
            if(candidate.size() > (*iter).size())
            {
//                chrono::steady_clock::time_point subset_search_time = chrono::steady_clock::now();
                if (includes(candidate.begin(), candidate.end(), (*iter).begin(), (*iter).end()
                        , [](obj* const & a, obj* const & b) -> bool { return a->id < b->id;}))
                {

                    maximal_groups.erase(iter);
                    continue;
                }
                ++iter;

//                time_stats[6] += chrono::steady_clock::now() - subset_search_time;
            } else //find superset or same as me
            {
//                chrono::steady_clock::time_point superset_search_time = chrono::steady_clock::now();
                if (includes((*iter).begin(), (*iter).end(), candidate.begin(),
                             candidate.end()
                             ,[](obj* const & a, obj* const & b) -> bool { return a->id < b->id;})) //i'm a subset or identical to the iter
                {
                    unique = false;
                    break;
                }
                ++iter;

//                time_stats[5] += chrono::steady_clock::now() - superset_search_time;

            }
        }
//        time_stats[3] += chrono::steady_clock::now() - MGtime;
        if (memory_consumption[6] > set_ops_per_group)
        {
            memory_consumption[6] = set_ops_per_group;
        }

        memory_consumption[7] += set_ops_per_group;

        if (memory_consumption[8] < set_ops_per_group)
        {
            memory_consumption[8] = set_ops_per_group;
        }


//        chrono::steady_clock::time_point insertion_time = chrono::steady_clock::now();
        if(unique)
        {
            maximal_groups.emplace_back(candidate);
        }

//        time_stats[3] += chrono::steady_clock::now() - MGtime;
    }

    return;
}



void intraframe_processing(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const vector<double>& pivot_info
        , const size_t& thres_g
        , vector<vector<obj*>>& maximal_groups
        , vector<chrono::nanoseconds>& intraframe_time_stats
        , vector<size_t>& memory_consumption)
{
    chrono::steady_clock::time_point target_grid_time_start;
    chrono::steady_clock::time_point valid_obj_time_start;
    chrono::steady_clock::time_point candidate_computation_time_start;

    //calculate num of grids for each direction
    size_t grid_row = (size_t) ceil((double)video_size.first/(double)grid_size.first);
    size_t grid_col = (size_t) ceil((double)video_size.second/(double)grid_size.second);

    size_t grid_num = 0;
    for(auto& seed_grid : a_frame_obj)
    {
        vector<size_t> target_grid;
//        target_grid_time_start = chrono::steady_clock::now();
        get_target_grid_rectangle(target_grid, grid_num, grid_row, grid_col, pivot_info, grid_size, video_size);
//        intraframe_time_stats[0] += chrono::steady_clock::now() - target_grid_time_start;

        for(auto& seed_obj : seed_grid)
        {

            valid_obj_time_start = chrono::steady_clock::now();
            //calculate a relative long,short radius of seed_obj-centered ellipse
            auto relative_radiuses = naive_get_ellipse_radius(seed_obj->bb_center[1], pivot_info);
            double relative_long_radius = relative_radiuses.first;
            double relative_short_radius = relative_radiuses.second;

            //compute the objects in target grid that located in seed_obj-centered ellipse
            vector<obj*> t_objs;
            for(auto& t_grid_num : target_grid)
            {
                //iterating every target obj except seed obj
                for(auto& candiate_obj : a_frame_obj[t_grid_num])
                {
                    if(candiate_obj->id != seed_obj->id)
                    {
                        auto t_relative_radiuses = naive_get_ellipse_radius(candiate_obj->bb_center[1], pivot_info);
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
//            intraframe_time_stats[1] += chrono::steady_clock::now() - valid_obj_time_start;



//          print_intraframe(seed_obj, t_objs);
            if(t_objs.size() < thres_g - 1) //if impossible to make thres_g - sized group
            {
                continue; //test the next seed
            }

//            candidate_computation_time_start = chrono::steady_clock::now();

            vector<obj*> candidate = {seed_obj};
                candidate_computation_naive_vector(candidate
                        , t_objs.begin(), t_objs.end()
                        ,maximal_groups, thres_g, pivot_info, intraframe_time_stats, memory_consumption);


//            intraframe_time_stats[2] += chrono::steady_clock::now() - candidate_computation_time_start;

        }

        grid_num++;
    }




//    vector<vector<size_t>*> output;
//    for(auto iter = candidate_map.rbegin(); iter != candidate_map.rend();++iter)
//    {
//        for(auto& candidate_ids : iter->second)
//        {
//            bool unique = true;
//            for(auto& entry : output)
//            {
//                if (includes((*entry).begin(), (*entry).end(), candidate_ids.begin(),
//                             candidate_ids.end())) //i'm a subset or identical to the iter
//                {
//                    unique = false;
//                    break;
//                }
//            }
//            if(unique)
//            {
//                output.push_back(&candidate_ids);
//            }
//        }
//    }


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
