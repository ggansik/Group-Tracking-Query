#include "preprocessing.hpp"

// TODO: Preprocessing:
//          1. Grid Size
//              option 1: Fixed Grid Size
//              option 2: Adaptive Grid Size
//              option 3: retangular grid size - for ellipse
//          2. Occlusion map
//class obj
//int id;
//double bb_left;
//double bb_top;
//double bb_width;
//double bb_height;
//vector<double> bb_center;

void print_preprocessing(vector<vector<vector<obj>>>& grid_index_obj);

void make_grid_fixed(vector<vector<obj>>& object_per_frame
                    , const pair<size_t, size_t>& video_size
                    , const pair<size_t,size_t>& grid_size
                    , vector<vector<vector<obj*>>>& grid_index_obj)
{


    const size_t grid_row = (size_t) ceil((double)video_size.first/(double)grid_size.first);
    const size_t grid_col = (size_t) ceil((double)video_size.second/(double)grid_size.second);


//    const size_t grid_row = grid_size.first; //grid granularity for
//    const size_t grid_col = (size_t) ceil((double)video_size.second/((double)video_size.first/(double)grid_row));

    const size_t grid_width = (size_t) ceil(((double)video_size.first/(double)grid_row));
    pair<size_t, size_t> grid_row_col = make_pair(grid_row, grid_col);

    for(auto& frame : object_per_frame)
    {
        //space allocation for entire grid in this frame
        vector<vector<obj*>> a_frame = vector<vector<obj*>>(grid_row * grid_col);

        grid_index_obj.push_back(a_frame);

        for(auto& object : frame)
        {

            auto grid_num = get_grid_num(object.bb_center[0], object.bb_center[1], make_pair(grid_width,grid_width), grid_row_col);
            grid_index_obj.back()[grid_num].push_back(&object);
        }
    }
}

void make_grid_multilevel(vector<vector<obj>>& object_per_frame
        , const pair<size_t, size_t>& video_size
        , const pair<size_t,size_t>& grid_size
        , const size_t& level
        , vector<vector<grid>>& grid_index_obj_multi)
        {

    vector<size_t> grid_row(level);
    vector<size_t> grid_col(level);

    size_t num_grid_row = (size_t) ceil((double)video_size.first/(double)grid_size.first);
    size_t num_grid_col = (size_t) ceil((double)video_size.second/(double)grid_size.second);

    //store the number of grids for each direction and level
    for(int current_level = 0; current_level < level; current_level++)
    {
        grid_row[current_level] = num_grid_row * pow(2, current_level);
        grid_col[current_level] = num_grid_col * pow(2, current_level);
    }

    //make grid index for each frame
    for(auto& frame : object_per_frame)
    {

        //space allocation for entire grid in this frame
        vector<grid> a_frame = vector<grid>(grid_row[0] * grid_col[0]);
        grid_index_obj_multi.push_back(a_frame);

        //object assignment for each grid in level 0
        //grid_index_obj_multi.back()->current frame
        //TODO: object assignment for children for more than level size 2
        for(auto& object : frame)
        {
            auto grid_index_x = (size_t)floor((double)object.bb_center[0]/(double)grid_size.first);
            auto grid_index_y = (size_t)floor((double)object.bb_center[1]/(double)grid_size.second);

            auto grid_center_x = grid_index_x * grid_size.first + (double)grid_size.first/2.0;
            auto grid_center_y = grid_index_y * grid_size.second + (double)grid_size.second/2.0;

            auto grid_num = grid_index_x + grid_index_y*grid_row[0];

            grid_index_obj_multi.back()[grid_num].objects.push_back(&object);

            size_t child_num = 4;
            if(object.bb_center[0] < grid_center_x)
            {
                if(object.bb_center[1] < grid_center_y)
                {
                    child_num = 0;
                }else
                {
                    child_num = 2;
                }
            } else
            {
                if(object.bb_center[1] < grid_center_y)
                {
                    child_num = 1;
                } else
                {
                    child_num = 3;
                }
            }
            grid_index_obj_multi.back()[grid_num].children[child_num].objects.push_back(&object);
        }

    }

}


void preprocessing(vector<vector<obj>>& object_per_frame
                    , const size_t grid_method
                    , const pair<size_t, size_t> video_size
                    , const pair<size_t, size_t> grid_size
                    , vector<vector<vector<obj*>>>& grid_index_obj
                    , vector<vector<grid>>& grid_index_obj_multi)
{
    switch (grid_method)
    {
        case 0:{ //square or rectangle fixed grid
            make_grid_fixed(object_per_frame, video_size, grid_size, grid_index_obj);
        }
        case 1:{ //rectangle + multilevel
            //make_grid_multilevel(object_per_frame, video_size, grid_size, 2, grid_index_obj_multi);
        }
        case 2:{

        }
    }

//    print_preprocessing(grid_index_obj);
}

void print_preprocessing(vector<vector<vector<obj>>>& grid_index_obj){
    int frame_number = 0;
    cout << "frame_number: " << frame_number << endl;
    for(auto& a_frame : grid_index_obj)
    {
        int grid_number = 0;
        for(auto& a_grid : a_frame)
        {
            cout << "grid_number " << grid_number << ": ";
            for(auto& a_obj: a_grid)
            {
                cout << a_obj.id << "(" << a_obj.bb_center[0] << "," << a_obj.bb_center[1] << ")" <<  ", ";
            }
            cout << endl;
            grid_number++;
        }
        frame_number++;
    }
}