//
// Created by Hyunsik Yoon on 2022-04-14.
//

#include "interframe.hpp"


//  TODO: Intra-frame processing:
//      1. Group lifetime, occlusion update
//          option 1: MFS + SSG
//          option 2: Inheritance method

void subset_update(vector<obj*>& group, map<vector<obj*>,State>& group_state, const size_t& frame_number)
{
    for(auto& state : group_state) {

        if(state.first.size() < group.size()) //subset check
        {

            if(includes(group.begin()
                    , group.end()
                    , state.first.begin()
                    , state.first.end()
                    ,[](obj *const &a, obj *const &b) -> bool
                        { return a->id < b->id; }))
            {
                state.second.end_frame = frame_number;
            }
        }
    }
}

bool existence_and_update(vector<obj*>& group, map<vector<obj*>,State>& group_state, const size_t& frame_number)
{
    for(auto& state : group_state)
    {
        if(state.first.size() == group.size())
        {
            bool flag = true;
            for(size_t i = 0; i < group.size(); ++i)
            {
                if(state.first[i]->id != group[i]->id)
                {
                    flag = false;
                    break;
                }
            }
            if(flag)
            {
                //apperance update for existing state
                state.second.end_frame = frame_number;
                return true;
            }
        }
    }
    return false;
}

bool intersection_existence_and_update(vector<obj*>& group
        , map<vector<obj*>,State>& group_state
        ,const size_t& superset_start_frame
        ,const size_t& frame_number)
{
    for(auto& state : group_state)
    {
        if(state.first.size() == group.size())
        {
            bool flag = true;
            for(size_t i = 0; i < group.size(); ++i)
            {
                if(state.first[i]->id != group[i]->id)
                {
                    flag = false;
                    break;
                }
            }
            if(flag)
            {
                //apperance update for existing state
                state.second.start_frame = min(state.second.start_frame, superset_start_frame);
                state.second.end_frame = frame_number;
                return true;
            }
        }
    }
    return false;
}

bool existence(vector<obj*>& group, map<vector<obj*>,State>& group_state)
{
    for(auto& state : group_state)
    {
        if(state.first.size() == group.size())
        {
            bool flag = true;
            for(size_t i = 0; i < group.size(); ++i)
            {
                if(state.first[i]->id != group[i]->id)
                {
                    flag = false;
                    break;
                }
            }
            if(flag)
            {
                return true;
            }
        }
    }
    return false;
}


void MFS_intersection(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thres_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& current_candidate
        , map<vector<obj*>,State>& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats)
{
    //check the candidate is exists or not
//    for(const auto& a_candidate : current_candidate){
//        for(const auto& id : a_candidate){
//            cout << id << ",";
//        }
//        cout << endl;
//    }
//    vector<vector<size_t>> group_state_for_iteration;
//    for(auto& group : group_state){
//        group_state_for_iteration.push_back(group.first);
//    }
    //state maintenance
//    cout << "group_state_size: " << current_candidate.size() << endl;

    vector<map<vector<obj*>,State>::iterator> new_added_state;



//    if(frame_number == 565)
//    {
//        cout << "i'm here!" << endl;
//    }

//    vector<obj*> test_vector = {new obj(15), new obj(277), new obj(278)};

    for(auto& appearance : current_candidate)
    {

//        if(includes(appearance.begin()
//                               ,appearance.end()
//                                ,test_vector.begin()
//                                ,test_vector.end()
//                                ,[](obj *const &a, obj *const &b) -> bool
//                                 { return a->id < b->id; }))
//        {
//            cout <<" I'm here!" << endl;
//        }

        //existence check!!
        if (existence_and_update(appearance, group_state, frame_number))
        {
            subset_update(appearance, group_state, frame_number);
        } else
        {
            //superset들 한테서 다 받아와야 함

            auto inserted = group_state.insert(make_pair(appearance,
                                                         State(appearance, frame_number, frame_number, nullptr)));

            //superset들 한테서 다 받아오기
            for (auto &group : group_state)
            {
                if(appearance.size() < group.first.size()) //superset check
                {
                    if(includes(
                            group.first.begin()
                            , group.first.end()
                            , appearance.begin()
                            , appearance.end()
                            ,[](obj *const &a, obj *const &b) -> bool
                                { return a->id < b->id; }))
                    {
                        inserted.first->second.start_frame
                        = min(inserted.first->second.start_frame, group.second.start_frame);
                    }
                }
            }

            //intersection Of added state
            for (auto &group : group_state)
            {
                vector<obj *> intersect(max(appearance.size(), group.first.size()) + 1);
                auto intersect_iter = set_intersection(
                        appearance.begin(), appearance.end(), group.first.begin(), group.first.end(), intersect.begin(),
                        [](obj *const &a, obj *const &b) -> bool
                        { return a->id < b->id; });

                intersect.resize(intersect_iter - intersect.begin());

                if (intersect.size() >= thres_g)
                { //intersection can be a new state

                    if(intersect.size() < appearance.size())
                    {
                        if (!intersection_existence_and_update(
                                intersect
                                , group_state
                                , group.second.start_frame
                                , frame_number)) //존재하지 않으면 새로운 state로 추가, 여기서는 subset update할 필요 없음
                        {//there is no state same as intersect
                            auto output = group_state.insert(
                                    make_pair(intersect, State(intersect, min(group.second.start_frame, inserted.first->second.start_frame),
                                            frame_number, nullptr)));
                            //store the frames from my parent(group, candidate)



                        }
                    }
                }
            }
        }
    }
    //occlusion & id_switch handling

//    cout << "erased state" << endl;
    for(auto iter = group_state.begin(); iter != group_state.end();){
        //TODO: which one is better?
        // 1. # of occlusions are larger than thres_f * thres_o
        // 2. # of occlusions are larger than duration from first-appeared frame * thres_o
        // 2-2. if a group appeared and occluded in next frame, this group is erased immediately.
        if(frame_number == video_length-1){
            if(thres_f < (int)iter->second.end_frame - (int)iter->second.start_frame){
                answer.push_back(iter->second);
                iter++;
            }else{

                iter++;
            }
        }else{
            if((int)frame_number - (int)iter->second.end_frame > thres_o)
            {
//                if(includes(iter->first.begin()
//                        ,iter->first.end()
//                        ,test_vector.begin()
//                        ,test_vector.end()
//                        ,[](obj *const &a, obj *const &b) -> bool
//                            { return a->id < b->id; }))
//                {
//                    cout <<" I'm here!" << endl;
//                }
                if(iter->second.end_frame - iter->second.start_frame > thres_f)
                {
                    if(maximal_segment_check(iter->second, answer))
                    {
//                        if(iter->first[0]->id == 15
//                           && iter->first[1]->id == 277
//                           && iter->first[2]->id == 278
//                           && iter->first.size() == 3)
//                        {
//                            cout <<" I'm here!" << endl;
//                        }


                        answer.push_back(iter->second);
                    }
                }
                group_state.erase(iter++);
            } else
            {
                iter++;
            }


            //answer condition before
//            if( thres_f * thres_o < (int)frame_number - (int)*(iter->second.begin()) - (int)iter->second.size()){
//    //            print_state(*iter);
//                if(thres_f <= (int)frame_number - (int)*(iter->second.begin())){
//                    answer.push_back(*iter);
//                }
//                group_state.erase(iter++);
//            }else{
//                iter++;
//            }
        }
    }
}



bool naive_existence_test(const size_t& frame_number
                        , vector<vector<size_t>>& current_candidate
                        , vector<pair<vector<size_t>,vector<size_t>>>& group_state)
{
    for(auto& group : group_state)
    {
        //create new s

        for(auto& candidate : current_candidate)
        {

        }
    }


    for(auto& candidate : current_candidate) //existence check for each current_candidates
    {
        bool make_new_state = true;
        vector<size_t> heritage;
        for(auto& group : group_state)
        {
            vector<size_t> output(10);

            auto it = set_difference(candidate.begin(), candidate.end()
                                    , group.first.begin(), group.first.end()
                                    , output.begin());
            output.resize(it-output.begin());

            if(output.size() == 0) //candidate is an identical or a subset of the group
            {
                if(candidate.size() == group.first.size()) //candidate is identical with the group
                {
                    make_new_state = false;
                    //insert the current frame number into group.second
                    group.second.push_back(frame_number);
                    break;
                } else //candidate is a subset of this group
                {
                    //TODO: ancester-descendant relationship linkage
                    //TODO: avoidance of duplicated frame insertion
                    for(auto& appeared_frame : group.second)
                    {
                        heritage.push_back(appeared_frame);
                    }
                }
            } else //candidate is not a subset of this group
            {
                //check whether candidate is a superset of this group

                vector<size_t> output2(10);
                auto it = set_difference(group.first.begin(), group.first.end()
                        ,candidate.begin(), candidate.end()
                        , output2.begin());
                output.resize(it-output2.begin());

                if(output2.size() == 0) //candidate is a superset of this group
                {
                    //TODO: ancester-descendant relationship linkage
                }else{ //candidate and this group is disjoint

                }
            }
        }
        if(make_new_state)
        {
            //make a new state for candidate, get the heritage
        }else
        {
            //candidate is already exists
            //TODO: linkage commit to
        }
    }
}






void interframe_processing(const size_t& frame_number
                            , const size_t& video_length
                            , const size_t& thres_g
                            , const size_t& thres_f
                            , const double& thres_o
                            , vector<vector<obj*>>& maximal_groups
                            , map<vector<obj*>,State>& group_state
                            , list<State>& answer
                            , vector<chrono::nanoseconds>& interframe_time_stats
                            , vector<size_t>& memory_consumption){

    MFS_intersection(frame_number
            , video_length
            , thres_g
            , thres_f
            , thres_o
            , maximal_groups
            , group_state
            , answer
            , interframe_time_stats);


//    cout << "inter finished" << endl;
}

void print_state(pair<const vector<size_t> , set<size_t>>& a_state){
    for(auto& a_obj : a_state.first){
        cout << a_obj << ",";
    }
    cout << ": ";
    for(auto& a_frame : a_state.second){
        cout << a_frame << ", ";
    }
    cout << endl;
}

void print_group_state(map<vector<size_t>, set<size_t>>& group_state){
    for(auto& a_state : group_state){
        print_state(a_state);
    }
}