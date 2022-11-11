//
// Created by Hyunsik Yoon on 2022-04-14.
//

#ifndef VIDEOGROUP_DATAREAD_HPP
#define VIDEOGROUP_DATAREAD_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;


//  0~3: <frame > < id > < bb_left > < bb_top >
//  4~6: < bb_width > < bb_height > < conf >
//  7~9: < x > < y > < z >
class obj{
public:
    size_t id;
    size_t bb_left;
    size_t bb_top;
    size_t bb_width;
    size_t bb_height;
    vector<size_t> bb_center;

    obj();

    obj(size_t _id, size_t _bb_left, size_t _bb_top, size_t _bb_width, size_t _bb_height, pair<size_t, size_t> video_size, double width_factor)
    {
        id = _id;
        bb_left = _bb_left;
        bb_top = _bb_top;
        bb_width = _bb_width;
        bb_height = _bb_height;
        bb_center.emplace_back((size_t)min((double)(bb_left+(bb_width/2)), (double)(video_size.first-1))); //video frame밖으로 넘어갈 경우 모서리의 좌표값으로
        bb_center.emplace_back((size_t)min((double)(video_size.second-1), (double)(bb_top+(bb_height))));
        //bb_width = width_factor*(bb_top+bb_height);
    };

    obj(size_t _id) //for group state test
    {
        id = _id;
        //bb_width = width_factor*(bb_top+bb_height);
    };






    bool operator<(obj* rhs) const
    {
        return id < rhs->id;
    }
//    bool operator>(const obj& rhs) const
//    {
//        return id > rhs.id;
//    }

    void printobj()
    {
        cout << id << " " << bb_left << " " << bb_top << " " << bb_width << " " << bb_height;
    }
};
void dataread_gt(vector<vector<obj>>& object_per_frame
        , const string& filename
        , pair<size_t, size_t> video_size
        , string isStu003);
void dataread(vector<vector<obj>>& object_per_frame
            , const string& filename
            , pair<size_t, size_t> video_size);

void dataprint(vector<vector<obj>>& object_per_frame);


#endif //VIDEOGROUP_DATAREAD_HPP
