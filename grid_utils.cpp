//
// Created by Hyunsik Yoon on 2022-05-09.
//

#include "grid_utils.hpp"
#include <iostream>
#include <cmath>
using namespace std;


pair<size_t, size_t> get_grid_num_multi(const size_t& x_coord, const size_t& y_coord, const pair<size_t,size_t>& grid_size, const pair<size_t, size_t>& grid_row_col)
{
    auto x_offset = (size_t)floor((double)x_coord/(double)grid_size.first);
    auto y_offset = (size_t)floor((double)y_coord/(double)grid_size.second);

    return make_pair(x_offset, y_offset);
}

//template <typename T>
//size_t get_grid_num(const T x_coord, const T y_coord, const pair<size_t,size_t> grid_size, const pair<size_t, size_t> grid_row_col)
//{
//    //grid_row_col = grid_row, grid_col
//    return (size_t)floor((double)x_coord/(double)grid_size.first) + (size_t)floor((double)y_coord/(double)grid_size.second) * grid_row_col.first;
//}


size_t get_grid_num(const size_t x_coord, const size_t y_coord, const pair<size_t,size_t> grid_size, const pair<size_t, size_t> grid_row_col)
{
    //grid_row_col = grid_row, grid_col
    return (size_t)floor((double)x_coord/(double)grid_size.first) + (size_t)floor((double)y_coord/(double)grid_size.second) * grid_row_col.first;
}

pair<double,double> getgrid_get_ellipse_radius(const size_t& ycoord, const vector<double>& pivot_info)
{
    //long, short
    return make_pair(pivot_info[2] + pivot_info[0]*(ycoord-pivot_info[4])
            , pivot_info[3] + pivot_info[1]*(ycoord-pivot_info[4]));
}

void get_target_grid_rectangle(vector<size_t>& target_grid
        , const size_t& seed_grid_num
        , const size_t& grid_row
        , const size_t& grid_col
        , const vector<double>& pivot_info
        , const pair<size_t,size_t>& grid_size
        , const pair<size_t, size_t>& video_size)
{
    size_t seed_x_offset = seed_grid_num%grid_row;
    size_t seed_y_offset = seed_grid_num/grid_row;

    size_t seed_left_x = seed_x_offset * grid_size.first;
    size_t seed_top_y = seed_y_offset * grid_size.second;
    size_t seed_right_x = seed_left_x + grid_size.first;
    size_t seed_bottom_y = seed_top_y + grid_size.second;

    auto radiuses = getgrid_get_ellipse_radius(seed_bottom_y, pivot_info);
    double max_ellipse_long_radius = max((double)radiuses.first,0.0);
    double max_ellipse_short_radius = max((double)radiuses.second,0.0);


    //diagonal vertices of the rectangle
    double min_y = max((double)0, (double)(seed_top_y - max_ellipse_short_radius));
    double min_x = max((double)0, (double)(seed_left_x - max_ellipse_long_radius));
    double max_y = min((double)video_size.second - 0.1, (double)seed_bottom_y + max_ellipse_short_radius);
    double max_x = min((double)video_size.first - 0.1, (double)seed_right_x + max_ellipse_long_radius);

    pair<size_t, size_t> grid_row_col = make_pair(grid_row, grid_col);
    auto min_grid_num = get_grid_num(min_x, min_y, grid_size, grid_row_col);
    auto end_of_first_row = get_grid_num(max_x, min_y, grid_size, grid_row_col);
    auto end_of_first_col = get_grid_num(min_x, max_y, grid_size, grid_row_col);


    // min_grid_num, min_grid_num + 1, ..., end_of_first_row
    for(size_t row = 0; row <= ((end_of_first_col - min_grid_num)/grid_row); ++row)
    {
        size_t offset = row*grid_row;
        for(size_t i = min_grid_num; i <= end_of_first_row; ++i)
        {
            target_grid.push_back(i+offset);
        }
    }

//    cout << "seed num: " << seed_grid_num << endl;
//    cout << "seed coord(left,top,right,bottom): " << seed_left_x <<","<< seed_top_y <<","<< seed_right_x <<","<< seed_bottom_y << endl;
//    cout << "ellipse: " << max_ellipse_long_radius << "," << max_ellipse_short_radius << endl;
//    cout << "rectangle boundary(left, top, right, bottom): " <<  min_x <<","<< min_y <<","<< max_x <<","<< max_y << endl;
//    for(auto grid_num : target_grid)
//    {
//        cout << grid_num<< endl;
//    }

}
void get_target_grid(vector<size_t>& target_grid
        , const size_t& seed_grid_num
        , const size_t& grid_row
        , const size_t& grid_col
        , const size_t& ellipse_grid_divider)
{

    if(seed_grid_num%grid_row == 0) //left edge
    {
        if(seed_grid_num == 0) //left top vertex
        {
            //target_grid = {0,1,grid_row,grid_row+1};
            for(size_t i = seed_grid_num; i <= seed_grid_num + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num + grid_row; i <= seed_grid_num + grid_row + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }

        }
        else if(seed_grid_num == grid_row*(grid_col-1)) //left bottom vertex
        {
            target_grid = {seed_grid_num, seed_grid_num + 1
                           , seed_grid_num - grid_row, seed_grid_num - grid_row+ 1};
            for(size_t i = seed_grid_num; i <= seed_grid_num + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num - grid_row; i <= seed_grid_num - grid_row + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }

        }
        else
        {
//            target_grid = {seed_grid_num - grid_row, seed_grid_num - grid_row + 1
//                           , seed_grid_num, seed_grid_num + 1
//                           , seed_grid_num + grid_row, seed_grid_num + grid_row + 1};

            for(size_t i = seed_grid_num - grid_row; i <= seed_grid_num - grid_row + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num; i <= seed_grid_num + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num + grid_row; i <= seed_grid_num + grid_row + ellipse_grid_divider; ++i)
            {
                target_grid.push_back(i);
            }

        }
    }
    else if(seed_grid_num%grid_row == grid_row-1) //right edge
    {
        if(seed_grid_num == grid_row - 1) //right top vertex
        {
//            target_grid = {seed_grid_num - 1, seed_grid_num
//                            ,seed_grid_num + grid_row - 1, seed_grid_num + grid_row};

            for(size_t i = seed_grid_num - ellipse_grid_divider; i <= seed_grid_num; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num + grid_row - ellipse_grid_divider; i <= seed_grid_num + grid_row; ++i)
            {
                target_grid.push_back(i);
            }
        }
        else if(seed_grid_num == grid_row*grid_col - 1) //right bottom vertex
        {
//            target_grid = {seed_grid_num - grid_row - 1, seed_grid_num - grid_row
//                    ,seed_grid_num - 1, seed_grid_num};

            for(size_t i = seed_grid_num - grid_row - ellipse_grid_divider; i <= seed_grid_num - grid_row ; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num - ellipse_grid_divider; i <= seed_grid_num; ++i)
            {
                target_grid.push_back(i);
            }
        }
        else
        {
//            target_grid = {seed_grid_num - grid_row -1, seed_grid_num - grid_row
//                    , seed_grid_num - 1, seed_grid_num
//                    , seed_grid_num + grid_row - 1, seed_grid_num + grid_row};

            for(size_t i = seed_grid_num - grid_row - ellipse_grid_divider; i <= seed_grid_num - grid_row; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num - ellipse_grid_divider; i <= seed_grid_num; ++i)
            {
                target_grid.push_back(i);
            }
            for(size_t i = seed_grid_num + grid_row - ellipse_grid_divider; i <= seed_grid_num + grid_row; ++i)
            {
                target_grid.push_back(i);
            }

        }
    }
    else if(seed_grid_num + grid_row >= grid_row*grid_col) //bottom edge
    {
//        target_grid = {seed_grid_num - grid_row -1, seed_grid_num - grid_row, seed_grid_num - grid_row + 1
//                , seed_grid_num - 1, seed_grid_num, seed_grid_num + 1};

        for(size_t i = seed_grid_num - grid_row - ellipse_grid_divider; i <= seed_grid_num - grid_row + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }
        for(size_t i = seed_grid_num - ellipse_grid_divider; i <= seed_grid_num + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }

    }
    else if((int)seed_grid_num - (int)grid_row < 0) //top edge
    {
//        target_grid = {seed_grid_num -1, seed_grid_num, seed_grid_num + 1, seed_grid_num + grid_row - 1, seed_grid_num + grid_row, seed_grid_num + grid_row + 1};

        for(size_t i = seed_grid_num - ellipse_grid_divider; i <= seed_grid_num + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }
        for(size_t i = seed_grid_num + grid_row - ellipse_grid_divider; i <= seed_grid_num + grid_row + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }
    }
    else
    {
        //target_grid = {seed_grid_num - grid_row -1, seed_grid_num - grid_row, seed_grid_num - grid_row + 1
//                ,seed_grid_num -1, seed_grid_num, seed_grid_num + 1
//                , seed_grid_num + grid_row - 1, seed_grid_num + grid_row, seed_grid_num + grid_row + 1};

        for(size_t i = seed_grid_num - grid_row - ellipse_grid_divider; i <= seed_grid_num - grid_row + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }
        for(size_t i = seed_grid_num - ellipse_grid_divider; i <= seed_grid_num + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }
        for(size_t i = seed_grid_num + grid_row - ellipse_grid_divider; i <= seed_grid_num + grid_row + ellipse_grid_divider; ++i)
        {
            target_grid.push_back(i);
        }
    }
}