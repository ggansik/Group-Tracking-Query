//
// Created by Hyunsik Yoon on 2022-05-09.
//

#ifndef VIDEOGROUP_GRID_UTILS_HPP
#define VIDEOGROUP_GRID_UTILS_HPP
#include <iostream>
#include <vector>
#include <cmath>
#include "dataread.hpp"
#include "distance.hpp"
using namespace std;
class grid{
public:
    vector<grid> children;
    vector<obj*> objects;

    pair<double, double> lefttop;
    pair<double, double> rightbot;

    grid()
    {
        children = vector<grid>(4);
    }
};
pair<size_t, size_t> get_grid_num_multi(const size_t& x_coord
        , const size_t& y_coord
        , const pair<size_t,size_t>& grid_size
        , const pair<size_t, size_t>& grid_row_col);

//template<typename T>
//size_t get_grid_num(const T& x_coord, const T& y_coord, const pair<size_t,size_t>& grid_size, const pair<size_t, size_t>& grid_row_col)
//{
//    //grid_row_col = grid_row, grid_col
//    return (size_t)floor((double)x_coord/(double)grid_size.first) + (size_t)floor((double)y_coord/(double)grid_size.second) * grid_row_col.first;
//}
size_t get_grid_num(const size_t x_coord, const size_t y_coord, const pair<size_t,size_t> grid_size, const pair<size_t, size_t> grid_row_col);



void get_target_grid_rectangle(vector<size_t>& target_grid
        , const size_t& seed_grid_num
        , const size_t& grid_row
        , const size_t& grid_col
        , const vector<double>& pivot_info
        , const pair<size_t,size_t>& grid_size
        , const pair<size_t, size_t>& video_size);

void get_target_grid(vector<size_t>& target_grid
        , const size_t& seed_grid_num
        , const size_t& grid_row
        , const size_t& grid_col
        , const size_t& ellipse_grid_divider);


#endif //VIDEOGROUP_GRID_UTILS_HPP
