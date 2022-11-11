//
// Created by Hyunsik Yoon on 2022-07-05.
//

#ifndef VIDEOGROUP_PRINT_UTILS_HPP
#define VIDEOGROUP_PRINT_UTILS_HPP
#include "dataread.hpp"

void print_intraframe(obj& seed_obj, vector<obj*>& t_objs)
{
    cout << seed_obj.id << ": ";

    for(auto a_obj : t_objs)
    {
        cout << a_obj->id << "(" << a_obj->bb_width << ")" << ", ";
    }
    cout << endl;
}
#endif //VIDEOGROUP_PRINT_UTILS_HPP
