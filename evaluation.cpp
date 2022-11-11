//
// Created by Hyunsik Yoon on 2022-06-26.
//

#include "evaluation.h"



void method(vector<vector<obj>>& object_per_frame
        , const size_t& grid_method
        , const size_t& interframe_method
        , const size_t& intraframe_method
        , const pair<size_t, size_t>& video_size
        , const vector<double>& pivot_info
        , const size_t& thres_g
        , const size_t& thres_f
        , const size_t& thres_o
        , const double& ellipse_grid_divider
        , list<State>& answers
        , chrono::nanoseconds& intraframe_time
        , chrono::nanoseconds& interframe_time
        , chrono::nanoseconds& preprocessing_time
        , vector<chrono::nanoseconds>& intraframe_time_stats
        , vector<chrono::nanoseconds>& intraframe_trie_stats
        , vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& interframe_num_states
        , vector<size_t>& memory_consumption)
{

//    size_t grid_size_x = (size_t)((double)pivot_info[2]/(ellipse_grid_divider*4));
//    size_t grid_size_y = (size_t)((double)pivot_info[3]/(ellipse_grid_divider*4));

    size_t grid_size_x = (size_t) ceil(((double)video_size.first/(double)ellipse_grid_divider));
    size_t grid_size_y = (size_t) ceil(((double)video_size.first/(double)ellipse_grid_divider));
//    size_t grid_size_y = grid_size_x;
    pair<size_t,size_t> grid_size = make_pair(grid_size_x, grid_size_y);
//    cout << grid_size.first << ", " << grid_size.second << endl;

    vector<vector<vector<obj*>>> grid_index_obj;
    vector<vector<grid>> grid_index_obj_multi;
    chrono::steady_clock::time_point preprocessing_start = chrono::steady_clock::now();
    preprocessing(object_per_frame, grid_method, video_size, grid_size, grid_index_obj, grid_index_obj_multi);
    preprocessing_time += chrono::steady_clock::now() - preprocessing_start;

    size_t video_length = object_per_frame.size();


    map<vector<obj*>, State> group_state;
    SSG ssg_group_state;
    SL_tracking_trie tracks;
    unordered_map<string, notrie_state> tracks_notrie;

    size_t current_frame_no = 0;
    if(intraframe_method == 99)
    {
        cout << "verification mode" << endl;
    }



    map<size_t, size_t> length_stat;
    for(auto& a_frame : grid_index_obj)
    {
//        cout << current_frame_no << endl;
//        if(current_frame_no % 1000 == 0)
//        {
//            cout << current_frame_no << " frame processed" << endl;
//        }

        size_t data_memory = 0;
        for(auto& grid : a_frame)
        {
            data_memory+=grid.size()*sizeof(obj);
        }

        if(data_memory> memory_consumption[5])
        {
            memory_consumption[5] = data_memory;
        }

        vector<vector<obj*>> maximal_groups;
        chrono::steady_clock::time_point group_candidate_computation_start = chrono::steady_clock::now();
        if(intraframe_method == 0) //naive
        {
            intraframe_processing(a_frame
                    , video_size
                    , grid_size
                    , ellipse_grid_divider
                    , pivot_info
                    , thres_g
                    , maximal_groups
                    , intraframe_time_stats
                    , memory_consumption);


        }else if(intraframe_method == 1) //detection pruning
        {
//            cerr << "hello" << endl;
            intraframe_processing_seedbased(a_frame
                    , video_size
                    , grid_size
                    , ellipse_grid_divider
                    , maximal_groups
                    , pivot_info
                    , thres_g
                    , intraframe_time_stats
                    , memory_consumption);

//            cout << current_frame_no << "----------------------------------------------" <<endl;
//            for(auto& group : maximal_groups)
//            {
//                for(auto group2 : maximal_groups)
//                {
//
//                }
//                for(auto& obj : group)
//                {
//                    cout << obj->id << ",";
//                }
//                cout << endl;
//            }

//            cout << maximal_groups.size() << endl;
        }
        intraframe_time += chrono::steady_clock::now() - group_candidate_computation_start;

//        for(auto group : maximal_groups_list)
//        {
//            maximal_groups.emplace_back(group);
//        }

        list<State> answer_per_frame;
        list<notrie_state> answer_per_frame_notrie;
//        cout << maximal_groups.size() << endl;
        chrono::steady_clock::time_point group_state_maintenance_start = chrono::steady_clock::now();
        if(interframe_method == 0) //clustering and intersection
        {
            interframe_processing(current_frame_no
                    , video_length
                    , thres_g
                    , thres_f
                    , thres_o
                    , maximal_groups
                    , group_state
                    , answer_per_frame
                    , interframe_time_stats
                    , memory_consumption);

            interframe_num_states[0] = min(interframe_num_states[0], group_state.size());
            interframe_num_states[1] = max(interframe_num_states[1], group_state.size());
            interframe_num_states[2]+= group_state.size();


        }else if(interframe_method == 1) //no set trie
        {
            interframe_lazy_notrie(current_frame_no
                    , thres_g
                    , thres_f
                    , thres_o
                    , maximal_groups
                    , tracks_notrie
                    , answer_per_frame_notrie
                    , interframe_time_stats
                    , memory_consumption);

        }else if(interframe_method == 2)
        {
            //TODO: exception handling for answers that appear end of video
            interframe_lazy(current_frame_no
                    , thres_g
                    , thres_f
                    , thres_o
                    , object_per_frame.size()
                    , maximal_groups
                    , tracks
                    , answer_per_frame
                    , interframe_time_stats
                    , memory_consumption);
            interframe_num_states[0] = min(interframe_num_states[0], tracks.states.size());
            interframe_num_states[1] = max(interframe_num_states[1], tracks.states.size());
            interframe_num_states[2]+= tracks.states.size();


            size_t maximal_group_memory = 0;
            for(auto& group : maximal_groups)
            {
                maximal_group_memory += group.size()*sizeof(obj*);
            }

            if(maximal_group_memory > memory_consumption[1])
            {
                memory_consumption[1] = maximal_group_memory;
            }

            size_t answer_memory = 0;
            for(auto& state : answer_per_frame)
            {
                answer_memory += state.get_memory_consumption();
            }

            if(answer_memory> memory_consumption[2])
            {
                memory_consumption[2] = answer_memory;
            }




        }else if(interframe_method == 3)
        {
            interframe_processing_SSG(current_frame_no
                    , video_length
                    , thres_g
                    , thres_f
                    , thres_o
                    , maximal_groups
                    , ssg_group_state
                    , answer_per_frame
                    , interframe_time_stats
                    , memory_consumption);
            interframe_num_states[0] = min(interframe_num_states[0], ssg_group_state.entire_state.size());
            interframe_num_states[1] = max(interframe_num_states[1], ssg_group_state.entire_state.size());
            interframe_num_states[2]+= ssg_group_state.entire_state.size();

            size_t maximal_group_memory = 0;
            for(auto& group : maximal_groups)
            {
                maximal_group_memory += group.size()*sizeof(obj*);
            }

            if(maximal_group_memory > memory_consumption[1])
            {
                memory_consumption[1] = maximal_group_memory;
            }

            size_t answer_memory = 0;
            for(auto& state : answer_per_frame)
            {
                answer_memory += state.get_memory_consumption();
            }

            if(answer_memory> memory_consumption[2])
            {
                memory_consumption[2] = answer_memory;
            }

        }else if(interframe_method == 4) //interframe answer verification
        {
            interframe_lazy(current_frame_no
                    , thres_g
                    , thres_f
                    , thres_o
                    , object_per_frame.size()
                    , maximal_groups
                    , tracks
                    , answer_per_frame
                    , interframe_time_stats
                    , memory_consumption);

            list<State> answers_per_frame;
            interframe_processing_SSG(current_frame_no
                    , video_length
                    , thres_g
                    , thres_f
                    , thres_o
                    , maximal_groups
                    , ssg_group_state
                    , answers_per_frame
                    , interframe_time_stats
                    , memory_consumption);


            bool problem = false;

            for (auto &answer : answer_per_frame)
            {

                bool matched = false;
                for (auto &answer2 : answers_per_frame)
                {
                    if(answer.group.size()==answer2.group.size())
                    {
                        bool group_matched = true;
                        for(size_t i = 0; i < answer.group.size(); ++i)
                        {
                            if(answer.group[i]->id != answer2.group[i]->id)
                            {
                                group_matched = false;
                                break;
                            }
                        }

                        if(group_matched
                           && answer.start_frame == answer2.start_frame
                           && answer.end_frame == answer2.end_frame)
                        {
                            matched = true;
                            break;
                        }

                    }

                }
                if(!matched)
                {
                    problem = true;
                    break;
                }
            }




            if(problem || answer_per_frame.size() != answers_per_frame.size())
            {
                cout << answer_per_frame.size() <<","<< answers_per_frame.size() << endl;
                cout << "------------------------------------------------" << endl;
                cout << "|                  frame: " << current_frame_no << "                   |" << endl;
                cout << "------------------------------------------------" << endl;
                cout << "                   lazy" << endl;
                for (auto &answer : answer_per_frame)
                {
                    for (auto &obj : answer.group)
                    {
                        cout << obj->id << ",";
                    }
                    cout << ": [";
                    cout << answer.start_frame << ",";
                    cout << answer.end_frame << "]" << endl;
                }


                cout << "                   SSG" << endl;
                for (auto &answer: answers_per_frame)
                {
                    for (auto &obj : answer.group)
                    {
                        cout << obj->id << ",";
                    }
                    cout << ": [";
                    cout << answer.start_frame << ",";
                    cout << answer.end_frame << "]" << endl;
                }
            }

        }else if(interframe_method == 5) //interframe state verification
        {
            interframe_lazy(current_frame_no
                    , thres_g
                    , thres_f
                    , thres_o
                    , object_per_frame.size()
                    , maximal_groups
                    , tracks
                    , answer_per_frame
                    , interframe_time_stats
                    , memory_consumption);

            list<State> answers_per_frame;


            interframe_processing_SSG(current_frame_no
                    , video_length
                    , thres_g
                    , thres_f
                    , thres_o
                    , maximal_groups
                    , ssg_group_state
                    , answers_per_frame
                    , interframe_time_stats
                    , memory_consumption);


            if(ssg_group_state.entire_state.size() != tracks.states.size())
            {
                cout << "------------------------------------------------" << endl;
                cout << "|                  frame: " << current_frame_no << "                   |" << endl;
                cout << "------------------------------------------------" << endl;

                cout << "SSG---------------------------------------------" << endl;
                for (auto &state : ssg_group_state.entire_state)
                {
                    cout << state.first;
                    cout << ": ";
                    cout << state.second.start_frame << ", ";
                    cout << state.second.end_frame << endl;

                }
                cout << "SSG-PS--------------------------------------------" << endl;
                for (auto &state : ssg_group_state.PSList)
                {
                    cout << state.first;
                    cout << ": ";
                    cout << state.second->start_frame << ", ";
                    cout << state.second->end_frame << endl;

                }


                cout << "lazy---------------------------------------------" << endl;
                for (auto &state : tracks.states)
                {
                    for (auto &obj : state.group)
                    {
                        cout << obj->id << ",";
                    }
                    cout << ": ";
                    cout << state.start_frame << ", ";
                    cout << state.end_frame << endl;
                }
            }

        }
        interframe_time += chrono::steady_clock::now() - group_state_maintenance_start;

        for(auto& answer : answer_per_frame)
        {
            answers.emplace_back(answer);
        }


        current_frame_no++;
    }
         //
//     for(auto& size : length_stat)
//     {
//        cout << size.first << ": " << size.second << endl;
//     }

}
//void method_proposed()
//{
//
//}