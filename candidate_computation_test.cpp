//
// Created by Hyunsik Yoon on 2022-07-13.
//

#include "candidate_computation_test.hpp"

pair<double,double> test_get_ellipse_radius(const size_t& ycoord, const vector<double>& pivot_info)
{
    //long, short
    return make_pair(pivot_info[2] + pivot_info[0]*(ycoord-pivot_info[4])
            , pivot_info[3] + pivot_info[1]*(ycoord-pivot_info[4]));
}

bool test_group_test(vector<obj*>& candidate, obj* t_obj, const vector<double>& pivot_info)
{
    //check whether t_obj can be a member of group
    for(const auto& member : candidate)
    {

        auto m_radiuses = test_get_ellipse_radius(member->bb_center[1], pivot_info);
        auto t_radiuses = test_get_ellipse_radius(t_obj->bb_center[1], pivot_info);

        if(!ellipse_check(member, t_obj, m_radiuses.first, m_radiuses.second)
           || !ellipse_check(t_obj, member, t_radiuses.first, t_radiuses.second))
        {
            return false;
        }
    }
    return true;
};



void candidate_computation_no_recursive(vector<obj*>& t_objs
        , vector<vector<size_t>>& candidate_map
        , const size_t &thres_g
        , const vector<double> &pivot_info
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
        }while(next_permutation(v.begin(), v.end()));
    }

}




void candidate_computation_recursive(vector<obj*> candidate
        , vector<size_t> candidate_ids
        , vector<obj*>::iterator left_objs_begin
        , const vector<obj*>::iterator& left_objs_end
        ,   map<size_t, vector<vector<size_t>>>& candidate_map
        , const size_t& thres_g
        , const vector<double>& pivot_info)
{
    //branching
    for (; left_objs_begin != left_objs_end; ++left_objs_begin)
    {
        if(candidate.size() == 1)
        {
            vector<obj*> branch = candidate;
            vector<size_t> branch_ids = candidate_ids;
            branch.push_back(*left_objs_begin);
            branch_ids.push_back((*left_objs_begin)->id);

            candidate_computation_recursive(branch, branch_ids
                    , left_objs_begin + 1, left_objs_end
                    , candidate_map, thres_g, pivot_info);
        }else{
            if(test_group_test(candidate, *left_objs_begin, pivot_info))
            {
                vector<obj*> branch = candidate;
                vector<size_t> branch_ids = candidate_ids;
                branch.push_back(*left_objs_begin);
                branch_ids.push_back((*left_objs_begin)->id);

                candidate_computation_recursive(branch, branch_ids
                        , left_objs_begin + 1, left_objs_end
                        , candidate_map, thres_g, pivot_info);
            }
        }
    }




    if(candidate.size() >= thres_g)
    {
        sort(candidate_ids.begin(), candidate_ids.end());

        auto iter = candidate_map.find(candidate_ids.size());
        if(iter != candidate_map.end())
        {
            iter->second.push_back(candidate_ids);

//            iter->second.second = unordered_set<size_t>();
//            for(auto& object : candidate)
//            {
//                size_t grid_num = 0;
//                if(object->bb_center[0] > 990)
//                    if(object->bb_center[1] > 540)
//                        grid_num = 3;
//                    else
//                        grid_num = 1;
//                else
//                    if(object->bb_center[1] > 540)
//                        grid_num = 1;
//                iter->second.second.insert(grid_num);
//            }
        } else
        {
            auto iter2 = candidate_map.insert(make_pair(candidate_ids.size()
                    , vector<vector<size_t>>()));
            iter2.first->second.push_back(candidate_ids);
//            for(auto& object : candidate)
//            {
//                size_t grid_num = 0;
//                if(object->bb_center[0] > 990)
//                    if(object->bb_center[1] > 540)
//                        grid_num = 3;
//                    else
//                        grid_num = 1;
//                else
//                if(object->bb_center[1] > 540)
//                    grid_num = 1;
//                iter2.first->second.second.insert(grid_num);
//            }
        }

//        for(const auto& id : candidate_ids)
//        {
//            cout << id << ",";
//        }
//        cout << endl;
    }
    return;
}


void candidate_computation_test(vector<vector<obj*>>& a_frame_obj
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& ellipse_grid_divider
        , const vector<double>& pivot_info
        , const size_t& thres_g
        ,   map<size_t, vector<vector<size_t>>>& candidate_map
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



//        cout << "grid_num: " << grid_num << endl;
        for (auto &seed_obj : seed_grid)
        {
//            cout << "seed_num: " << seed_obj->id <<endl;
            valid_obj_time_start = chrono::steady_clock::now();
            //calculate a relative long,short radius of seed_obj-centered ellipse
            auto relative_radiuses = test_get_ellipse_radius(seed_obj->bb_center[1], pivot_info);
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
                        auto t_relative_radiuses = test_get_ellipse_radius(candiate_obj->bb_center[1], pivot_info);
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
            vector<size_t> candidate_ids = {seed_obj->id};
//            candidate_computation_naive_vector(candidate, candidate_ids
//                    , t_objs.begin(), t_objs.end()
//                    ,candidate_map, thres_g, thres_d);

            candidate_computation_recursive(candidate
                    , candidate_ids
                    , t_objs.begin()
                    , t_objs.end()
                    , candidate_map
                    , thres_g
                    , pivot_info);


            intraframe_time_stats[2] += chrono::steady_clock::now() - candidate_computation_time_start;
        }

        grid_num++;
    }

//    for(auto iter = candidate_map.begin(); iter != candidate_map.end();)
//    {
//        iter->
//        //find subsets of me
//        if(candidate_ids.size() > (*iter).size())
//        {
//
////                vector<size_t> output(10);
////                auto it = set_difference((*iter).begin(), (*iter).end()
////                ,candidate_ids.begin(), candidate_ids.end()
////                ,output.begin());
////
////                output.resize(it-output.begin())
//
//            if(includes(candidate_ids.begin(), candidate_ids.end()
//                    , (*iter).begin(), (*iter).end())) //iter is my subset
//            {
//                candidate_map.erase(iter);
//                continue;
//            }
//            ++iter;
//        } else //find superset or same as me
//        {
////                vector<size_t> output(10);
////                auto it = set_difference(candidate_ids.begin(), candidate_ids.end()
////                        ,(*iter).begin(), (*iter).end()
////                        ,output.begin());
//
////                output.resize(it-output.begin());
//
//            if (includes((*iter).begin(), (*iter).end(), candidate_ids.begin(),
//                         candidate_ids.end())) //i'm a subset or identical to the iter
//            {
//                unique = false;
//                break;
//            }
//            ++iter;
//        }
//    }
//    if(unique)
//    {
//        candidate_map.push_back(candidate_ids);
//    }


}