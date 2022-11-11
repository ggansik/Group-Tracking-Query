#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "preprocessing.hpp"
#include "evaluation.h"
#include "ellipse_determination_statistically.hpp"
#include <queue>

using namespace std;
#define MEMORY

int main() {


    chrono::nanoseconds preprocessing_time = chrono::nanoseconds();
    chrono::nanoseconds intraframe_time = chrono::nanoseconds();
    chrono::nanoseconds interframe_time = chrono::nanoseconds();
    vector<chrono::nanoseconds> intraframe_time_stats(8);
    vector<chrono::nanoseconds> intraframe_trie_stats(8);
    vector<chrono::nanoseconds> interframe_time_stats(8);
    vector<size_t> interframe_num_states = {9999999999999, 0, 0};//min, max, total
    vector<size_t> memory_consumptions = {0,0,0,0,0,0,INTMAX_MAX,0,0,0};
    //0: recursion max-memory, 1: maximal group, 2:answer for each frame, 3:group_state 4:trie-size 5: data
    // 6: group set ops min 7: group set ops avg 8: group set ops max 9: num of total groups
    //proposed 에서는 3, 4가 합쳐져 있음

    vector<vector<obj>> object_per_frame;
// for student003 infer
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/dataset/student003/";
//    string filename = "student003_det.txt";
//    string GTorDET = "DET";
//    string dataset_det = "student003";
//    size_t video_width = 720;
//    size_t video_height = 576;

// for student003 gt
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/dataset/student003/";
//    string filename = "student003_gt.txt";
//    string GTorDET = "GT";
//    string dataset_det = "student003";
//    size_t video_width = 720;
//    size_t video_height = 576;



// for MOT1604 gt
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/dataset/MOT16/train/MOT16-04/gt/";
//    string filename = "MOT16-04_gt_edited.txt";
//    string GTorDET = "GT";
//    string dataset_det = "MOT16-04";
//    size_t video_width = 1920;
//    size_t video_height = 1080;


// for MOT1604 infer
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/tracked/MOT16-04_crowdhuman_yolov5m/";
//    string filename = "MOT16-04.txt";
//    string GTorDET = "DET";
//    string dataset_det = "MOT16-04";
//    size_t video_width = 1920;
//    size_t video_height = 1080;


// for MOT03 inferences
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/tracked/MOT20-03_crowdhuman_yolov5m/";
//    string filename = "MOT20-03.txt";
//    string GTorDET = "DET";
//    string dataset_det = "MOT20-03";
//    size_t video_width = 1173;
//    size_t video_height = 880;

    // for MOT03 groundtruths
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/dataset/MOT20/train/MOT20-03/gt/";
//    string filename = "MOT20-03_gt_edited.txt";
//    string GTorDET = "GT";
//
//    string dataset_det = "MOT20-03";
//    size_t video_width = 1173;
//    size_t video_height = 880;

// for MOT02 inferences
//    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/tracked/MOT20-02_crowdhuman_yolov5m/";
//    string filename = "MOT20-02.txt";
//    string GTorDET = "DET";
//    string dataset_det = "MOT20-02";
//    size_t video_width = 1920;
//    size_t video_height = 1080;


// for MOT02 groundtruths
    string filepath = "/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/dataset/MOT20/train/MOT20-02/gt/";
    string filename = "MOT20-02_gt_edited.txt";
    string GTorDET = "GT";

    string dataset_det = "MOT20-02";
    size_t video_width = 1920;
    size_t video_height = 1080;


//    ofstream outputfile;
//    outputfile.open(dataset_det+"_"+GTorDET+"_performance.txt", ios_base::out|ios_base::app);
//    cout.rdbuf(outputfile.rdbuf());

    string fullpath = filepath + filename;



    pair<size_t, size_t> video_size = make_pair(video_width,video_height);

    // data read


//    cout << "data read start" << endl;
    if(GTorDET == "GT")
    {
        dataread_gt(object_per_frame, fullpath, video_size, filename);
    } else
    {
        dataread(object_per_frame, fullpath, video_size);
    }
//    cout << "data read end" << endl;
//    dataprint(object_per_frame);

    map<vector<size_t>,set<size_t>> group_state;
    vector<pair<vector<size_t>, set<size_t>>> answer;


    vector<vector<double>> parameters;
    // grid_method
    // intraframe_method 0: clustering and intersection 1: detection pruning 99: candidate validation
    // interframe_method 0: MFS  1: set-trie 2: set-trie + lazy intersection 3: MFS + SSG
    // pivot h, v, y, bb
    // thres_f
    // thres_g
    // thres_o
    // ellipse_grid_divider

    chrono::steady_clock::time_point slope_determination_start = chrono::steady_clock::now();
    auto slope_ret = slope_determination(object_per_frame);
    chrono::nanoseconds slope_determination_time = chrono::steady_clock::now() - slope_determination_start;

    cout << slope_ret.first << "," << slope_ret.second << endl;
    vector<vector<double>> parameter_lists;


    vector<double> intra_method_list = {1}; //0: naive 1:proposed
    vector<double> inter_method_list = {0,3,1}; //0: MFS 1: lazy-intersection 2: set-trie+lazy-intersection 3: MFS+SSG

    cerr <<intra_method_list.size() <<endl;
    cerr <<inter_method_list.size() <<endl;

    size_t repeat_same_params = 1;


    for(auto intra_method : intra_method_list)
    {
        for(auto inter_method : inter_method_list)
        {
            parameter_lists.push_back({0, /*intra*/intra_method, /*inter*/inter_method, 300, 100, 1064, 500, 2, 125, 8});
        }

    }





    for(auto& parameter : parameter_lists)
    {
        for(size_t iterations = 0; iterations < repeat_same_params ; iterations++)
        {
            parameters.push_back(parameter);
        }
    }
    parameters.push_back({1}); //for flush

//    parameter = {0, 0, 0, 250, 100, 600, 100, 125, 3, 0.5, 1};
//    parameters.push_back(parameter);
    cout << "repeat: " << repeat_same_params << endl;

    for(size_t i = 0; i < parameters.size()-1; ++i)
    {
        auto param = parameters[i];


        int grid_method = (int)param[0];
        int intraframe_method = (int)param[1];
        size_t interframe_method = (size_t) param[2];
        //user query - ellipse info(horizontal pixel radius, vertical pixel radius, bb_width) f-duration g-sized group
        double pivot_long_rad = (double )param[3];
        double pivot_short_rad = (double)param[4];
        double pivot_y_coord = (double)param[5];
        vector<double> ellipse_info = {slope_ret.first
                                       , slope_ret.second
                                       , pivot_long_rad
                                       , pivot_short_rad
                                       , pivot_y_coord};
        //(object_y_coord/pivot_y_coord) * pivot_h_pixel
        size_t thres_f = (size_t)param[6];
        size_t thres_g = (size_t)param[7];
        double thres_o = (double)param[8]; //ratio to thres_f
        double ellipse_grid_divider = (double)param[9];


        string intra_method;
        string inter_method;
        if(intraframe_method == 0)
        {
            intra_method = "naive";
        }else if(intraframe_method == 1)
        {
            intra_method = "seed pruning";
        }

        if(interframe_method == 0)
        {
            inter_method = "MFS";
        }else if(interframe_method == 1)
        {
            inter_method = "set-trie";
        }else if(interframe_method == 2)
        {
            inter_method = "set-trie + lazy intersection";
        }else if(interframe_method == 3)
        {
            inter_method = "MFS+SSG";
        }

        else if(interframe_method == 4)
        {
            inter_method = "answer verification";
        }else if(interframe_method == 5)
        {
            inter_method = "state verification";
        }
        list<State> answers;


        method(object_per_frame
                , grid_method
                , interframe_method
                , intraframe_method
                , video_size
                , ellipse_info
                , thres_g
                , thres_f
               , thres_o
               , ellipse_grid_divider
               , answers
               , intraframe_time
               , interframe_time
               , preprocessing_time
               , intraframe_time_stats
               , intraframe_trie_stats
               , interframe_time_stats
               , interframe_num_states
               , memory_consumptions);


        ofstream file("/home/hyunsik/yolov5_deepsort/Yolov5_DeepSort_OSNet/group_tracked/" + dataset_det + "/" + dataset_det \
    + "_" + to_string((size_t)ellipse_info[2]) \
    + "_" + to_string((size_t)ellipse_info[3]) \
    + "_" + to_string((size_t)ellipse_info[4]) \
    + "_" + to_string(thres_f) \
    + "_" +  to_string(thres_g) \
    + "_" + to_string((size_t)(thres_o)) \
    + "_" + GTorDET\
    + ".txt");

        cout << "python " << dataset_det << "_track2video.py" <<
    + " " + to_string((size_t)ellipse_info[2]) \
    + " " + to_string((size_t)ellipse_info[3]) \
    + " " + to_string((size_t)ellipse_info[4]) \
    + " " + to_string(thres_f) \
    + " " +  to_string(thres_g) \
    + " " + to_string((size_t)(thres_o)) \
    + " " + GTorDET<< endl;
        for(auto& an_answer : answers)
        {
            for(auto& a_obj : an_answer.group){
                string temp = to_string(a_obj->id);
                file.write(temp.c_str(), temp.size());
                file.write(",", 1);
            }
//            cout << ":";
            file.write(":", 1);

            for(size_t f = an_answer.start_frame; f <= an_answer.end_frame; f++)
            {
                string temp = to_string(f);
                file.write(temp.c_str(), temp.size());
                file.write(",", 1);
            }

            file.write("\n", 1);

        }
        file.close();

        if(i % repeat_same_params != repeat_same_params-1)
        {
            continue;
        }

        //0 grid_method
        //1 intraframe_method 0: clustering and intersection 1: detection pruning 99: candidate validation
        //2 interframe_method 0: MFS  1: set-trie 2: set-trie + lazy intersection 3: MFS + SSG
        //3-5 pivot h, v, y
        //6 thres_f
        //7 thres_g
        //8 thres_o
        //9 ellipse_grid_divider
        auto total_time = preprocessing_time + intraframe_time + interframe_time;
        cout << "----------------------------- <result statistics> ------------------------------" << endl;
        cout << "|  method                      : " << intra_method << ", " << inter_method << endl;
        cout << "|  ellipse(long, short, pivoty): " << param[3] << ", " << param[4] << ", " << param[5] << endl;
        cout << "|  duration threshold          : " << param[6] << endl;
        cout << "|  groupsize threshold         : " << param[7] << endl;
        cout << "|  occlusion tolerance         : " << param[8] << endl;
        cout << "|  ellipse_grid_ratio          :  " << param[9] << endl;
        cout << "-------------------------------------------------------" << endl;
        cout << "time ratio" << endl;
        cout << "preprocessing: " << ((double)preprocessing_time.count()/(double)total_time.count())*100 << "%" << endl;
        cout << "candidate computation: " << ((double)intraframe_time.count()/(double)total_time.count())*100 << "%" <<  endl;
        cout << "state maintenance: " << ((double)interframe_time.count()/(double)total_time.count())*100 << "%" << endl;

        cout << "total time: "
             << chrono::duration_cast<chrono::milliseconds>(total_time+slope_determination_time).count()/repeat_same_params<< "ms" << endl;
        cout << "slope_determination: " << chrono::duration_cast<chrono::milliseconds>(slope_determination_time).count() << endl;
        cout << "preprocessing time: "
             <<  chrono::duration_cast<chrono::milliseconds>(preprocessing_time).count()/repeat_same_params << "ms" << endl;

        cout << "intraframe processing time: "
             <<  chrono::duration_cast<chrono::milliseconds>(intraframe_time).count()/repeat_same_params << "ms" << endl
             << "--target_grid_time: "
             <<  chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[0]).count()/repeat_same_params << "ms" << endl
             << "--valid_obj_time: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[1]).count()/repeat_same_params << "ms" << endl
             << "--candidate_computation_time: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[2]
                                                            /*- intraframe_time_stats[5]
                                                            - intraframe_time_stats[6]
                                                            - intraframe_time_stats[7]*/).count()/repeat_same_params << "ms" << endl
             << "--maximal_group_computation_time: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[3]
                                                           /* + intraframe_time_stats[4]
                                                            + intraframe_time_stats[5]
                                                            + intraframe_time_stats[6]
                                                            + intraframe_time_stats[7]*/).count()/repeat_same_params << "ms" << endl
             << "----group sort: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[3]).count()/repeat_same_params << "ms" << endl
             << "----existence check: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[4]).count()/repeat_same_params << "ms" << endl
             << "----superset check: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[5]).count()/repeat_same_params << "ms" << endl
             << "----subset check: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[6]).count()/repeat_same_params << "ms" << endl
             << "----insertion time: "
             << chrono::duration_cast<chrono::milliseconds>(intraframe_time_stats[7]).count()/repeat_same_params << "ms" << endl;

        cout << "interframe processing time: "
             <<  chrono::duration_cast<chrono::milliseconds>(interframe_time).count()/repeat_same_params << "ms" << endl;
        cout << "--appearance update phase time: "
             <<  chrono::duration_cast<chrono::milliseconds>(interframe_time_stats[0]).count()/repeat_same_params << "ms" << endl;
        cout << "--expire phase time: "
             <<  chrono::duration_cast<chrono::milliseconds>(interframe_time_stats[1]).count()/repeat_same_params << "ms" << endl;
//        cout << "query processing time: "
//             << chrono::duration_cast<chrono::milliseconds>(intraframe_time).count()\
//         + chrono::duration_cast<chrono::milliseconds>(interframe_time).count() << "ms" << endl;
        cout << "state statistics(min, max, avg): "
             << "(" << interframe_num_states[0] << "," << interframe_num_states[1] << "," << (interframe_num_states[2]/object_per_frame.size())/repeat_same_params << ")" << endl;
        cout << "total memory consumption(KB): "
        << (double)(memory_consumptions[0]
        + memory_consumptions[1]
        + memory_consumptions[2]
        + memory_consumptions[3]
        + memory_consumptions[4]
        + memory_consumptions[5]) /(double)1000<< endl;
        cout << "memory consumption(recursion, maximal group, answers, group_state, trie, data): "
        << (double)memory_consumptions[0]/(double)1000 << ", "
        << (double)memory_consumptions[1]/(double)1000 <<", "
        << (double)memory_consumptions[2]/(double)1000 <<","
        << (double)memory_consumptions[3]/(double)1000 << ","
        << (double)memory_consumptions[4]/(double)1000 << ","
        << (double)memory_consumptions[5]/(double)1000 << endl;
        cout << "group set operations(min, max, avg): "
             << (double)memory_consumptions[6]<< ", "
             << (double)memory_consumptions[8]<<", "
             << (double)memory_consumptions[7]/(double)memory_consumptions[9]<< endl;

        //0: recursion max-memory, 1: maximal group, 2:answer for each frame, 3:group_state 4:trie-size 5:data


        preprocessing_time = chrono::nanoseconds();
        intraframe_time = chrono::nanoseconds();
        interframe_time = chrono::nanoseconds();
        intraframe_time_stats = vector<chrono::nanoseconds>(8);
        intraframe_trie_stats = vector<chrono::nanoseconds>(8);
        interframe_time_stats = vector<chrono::nanoseconds>(8);
        interframe_num_states = {9999999999999, 0, 0};//min, max, total
        memory_consumptions = {0,0,0,0,0,0,INTMAX_MAX,0,0,0};

    }


//    string output_file_name = "MOT20-01_crowdhuman_yolov5m";













    return 0;
}
