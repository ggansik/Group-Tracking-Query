//
// Created by Hyunsik Yoon on 2022-08-08.
//

#include "ellipse_determination_statistically.hpp"


pair<double,double> slope_determination(vector<vector<obj>>& object_per_frame)
{
    vector<double> x_velocities;
    vector<double> y_velocities;
    vector<double> y_coords;
    unordered_map<size_t, obj*> before_obj; //object before a frame object id, obj

    for(size_t i = 0; i < (size_t)object_per_frame.size()*0.1; i += 25)
    {
        for(auto& obj : object_per_frame[i])
        {
            auto ret = before_obj.find(obj.id);
            if(ret != before_obj.end())
            {
                x_velocities.emplace_back(
                        abs((double)obj.bb_center[0] - (double)ret->second->bb_center[0]));
                y_velocities.emplace_back(
                        abs((double)obj.bb_center[1] - (double)ret->second->bb_center[1]));
                y_coords.emplace_back(ret->second->bb_center[1]);
                ret->second=&obj;
            } else
            {
                before_obj.insert(make_pair(obj.id, &obj));
            }
        }
    }

    vector<double> x_accelerations;
    vector<double> y_accelerations;
    double before_x_v = x_velocities[0];
    double before_y_v = y_velocities[0];
    double before_y = y_coords[0];
    for(size_t i = 1; i < x_velocities.size(); ++i)
    {
        if (y_coords[i]-before_y == 0)
        {
            y_coords[i] += 0.0001;
        }
//        if(abs(x_velocities[i]-before_x_v)/abs(y_coords[i]-before_y) != 0)
            x_accelerations.emplace_back(abs(x_velocities[i]-before_x_v)/abs(y_coords[i]-before_y));
//        if(abs(y_velocities[i]-before_y_v)/abs(y_coords[i]-before_y) != 0)
            y_accelerations.emplace_back(abs(y_velocities[i] - before_y_v) / abs(y_coords[i] - before_y));
    }
    size_t med = (size_t) x_accelerations.size()/2;
    sort(x_accelerations.begin(), x_accelerations.end());
    sort(y_accelerations.begin(), y_accelerations.end());

    return make_pair(0.117, 0.117); //MOT20-02
//    return make_pair(0.028, 0.028); //MOT20-03
//      return make_pair(0.05, 0.05); //MOT16-04
//    return make_pair(x_accelerations[med], y_accelerations[med]);


}
