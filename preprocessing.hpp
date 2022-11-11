//
// Created by Hyunsik Yoon on 2022-04-13.
//

#ifndef VIDEOGROUP_PREPROCESSING_HPP
#define VIDEOGROUP_PREPROCESSING_HPP

#include <iostream>
#include <vector>
#include "dataread.hpp"
#include "grid_utils.hpp"
#include <cmath>
using namespace std;

void print_preprocessing(vector<vector<vector<obj*>>>& grid_index_obj);

void make_grid_fixed(vector<vector<obj>>& object_per_frame
        , const pair<size_t, size_t>& video_size
        , const size_t& grid_size
        , vector<vector<vector<obj*>>>& grid_index_obj);

void make_grid_multilevel(vector<vector<obj>>& object_per_frame
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& level
        , vector<vector<grid>>& grid_index_obj_multi);

void preprocessing(vector<vector<obj>>& object_per_frame
        , const size_t grid_method
        , const pair<size_t, size_t> video_size
        , const pair<size_t, size_t> grid_size
        , vector<vector<vector<obj*>>>& grid_index_obj
        ,vector<vector<grid>>& grid_index_obj_multi);

#endif //VIDEOGROUP_PREPROCESSING_HPP
