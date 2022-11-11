//
// Created by Hyunsik Yoon on 2022-07-31.
//

#include "interframe_hash.hpp"


bool SSG_maximal_segment_check(SSG_State* track
        ,list<State>& answers)
{
    for(auto answer_iter = answers.begin(); answer_iter != answers.end();)
    {
        if((*answer_iter).group.size() > track->group.size())
        {
            bool group_include = includes((*answer_iter).group.begin(), (*answer_iter).group.end(),  track->group.begin(),
                                          track->group.end(), [](obj *const &a, obj *const &b) -> bool
                                          { return a->id < b->id; });

            bool lifetime_include =
                    (*answer_iter).start_frame <= track->start_frame && track->end_frame <= (*answer_iter).end_frame;

            if (group_include && lifetime_include)
            {
                return false;
            }
        } else if((*answer_iter).group.size() < track->group.size())
        {


            bool group_include = includes(track->group.begin()
                    , track->group.end()
                    , (*answer_iter).group.begin()
                    , (*answer_iter).group.end()
                    , [](obj *const &a, obj *const &b) -> bool
                                          { return a->id < b->id; });
            bool lifetime_include =
                    track->start_frame <= (*answer_iter).start_frame && (*answer_iter).end_frame <= track->end_frame;

            if (group_include && lifetime_include)
            {

//                if((*answer_iter).group[0]->id == 15
//                   && (*answer_iter).group[1]->id == 277
//                   && (*answer_iter).group[2]->id == 278
//                   && (*answer_iter).group.size() == 3)
//                {
//                    cout <<" I'm here!" << endl;
//                }

                answer_iter = answers.erase(answer_iter);
                continue;
            }
        }
        ++answer_iter;
    }
    return true;
}

string otos(vector<obj*>& group)
{
    string g = "";

    for(auto& obj : group)
    {

        g += to_string(obj->id);
        g += ",";
    }
    return g;
}




size_t visit_state(unordered_map<string, SSG_State*>::iterator& state
        , const size_t& frame_number
        , unordered_map<string, SSG_State>::iterator inserted
        , const size_t& thres_g
        , SSG& group_state)
{

//    cout << inserted->first << endl;

//    if(state->second->subset_visited == frame_number)
//    {
//        return 2;
//    }
//
    if(state->first == inserted->first)
    {
        return 2;
    }
    if(state->second->intersect_visited == inserted->first)
    {
        return 2;
    } else
    {
        state->second->intersect_visited = inserted->first;
    }

    vector<obj *> intersect(max(inserted->second.group.size(), state->second->group.size()) + 1);
    auto intersect_iter = set_intersection(
            inserted->second.group.begin()
            , inserted->second.group.end()
            , state->second->group.begin()
            , state->second->group.end(), intersect.begin(),
            [](obj *const &a, obj *const &b) -> bool
            { return a->id < b->id; });

    intersect.resize(intersect_iter - intersect.begin());

//    cout << state->first << endl;
//    cout << inserted->first << endl;
//
//    for(auto& obj : intersect)
//    {
//        cout <<obj->id<<",";
//    }
//
//
//    cout << endl;
    if(intersect.size() == state->second->group.size()) //i'm the superset of this state
    {
        state->second->start_frame = min(state->second->start_frame, inserted->second.start_frame);
        state->second->end_frame = frame_number;
        state->second->subset_update(frame_number, state->second->start_frame);

        state->second->parents.insert(make_pair(inserted->first, &inserted->second));
        inserted->second.children.insert(make_pair(state->first, state->second));
        group_state.PSList.erase((state++)->first);

        return 0;
    }else if (intersect.size() == inserted->second.group.size()) // this state is a superset of me
    {
        state->second->children.insert(make_pair(inserted->first, &inserted->second));
        inserted->second.parents.insert(make_pair(state->first, state->second));

        inserted->second.start_frame = min(state->second->start_frame, inserted->second.start_frame);

        for(auto child = state->second->children.begin(); child != state->second->children.end(); )
        {
            if(!visit_state(child
                    ,frame_number
                    ,inserted
                    ,thres_g
                    ,group_state) == 0)
            {
                ++child;
            }
        }


        return 1;
    } else
    {
        if (intersect.size() >= thres_g)
        {

            string intersect_string = otos(intersect);
            auto inter_exists = group_state.entire_state.find(intersect_string);

            if(inter_exists != group_state.entire_state.end()) //intersect is exists
            {
                //link and update
                inserted->second.children.insert(make_pair(inter_exists->first, &inter_exists->second));
                inter_exists->second.parents.insert(make_pair(inserted->first, &inserted->second));

                inter_exists->second.start_frame = min(inter_exists->second.start_frame, state->second->start_frame);
                inter_exists->second.end_frame = frame_number;
                inter_exists->second.subset_update(frame_number, inter_exists->second.start_frame);

                for(auto child = state->second->children.begin(); child != state->second->children.end();)
                {
                    if(!visit_state(child
                            ,frame_number
                            ,inserted
                            ,thres_g
                            ,group_state) == 0)
                    {
                        ++child;
                    }
                }
            }else //not exists
            {
                //create and link and get the parent's lifetime
                auto inter_inserted = group_state.entire_state.insert(
                        make_pair(intersect_string,
                                  SSG_State(intersect
                                          , state->second->start_frame
                                          , frame_number
                                          , nullptr, true)));

                //make a linkage for two parents
                inter_inserted.first->second.parents.insert(make_pair(inserted->first, &inserted->second));
                inter_inserted.first->second.parents.insert(make_pair(state->first, state->second));

                inserted->second.children.insert(
                        make_pair(inter_inserted.first->first, &inter_inserted.first->second));
                state->second->children.insert(
                        make_pair(inter_inserted.first->first, &inter_inserted.first->second));

                for(auto child = state->second->children.begin(); child != state->second->children.end();)
                {
                    if(!visit_state(child
                            ,frame_number
                            ,inserted
                            ,thres_g
                            ,group_state) == 0)
                    {
                        ++child;
                    }
                }
            }


        }

        return 2;
    }


}


void SSG_CM(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thres_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& current_candidate
        , SSG& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption)
{
    vector<string> current_candidate_string;
    for(auto & cand : current_candidate)
    {
        string cand_string = "";
        for(auto& obj : cand)
        {
            cand_string += to_string(obj->id);
            cand_string += ",";
        }
        current_candidate_string.emplace_back(cand_string);
    }


    vector<map<vector<obj*>,State>::iterator> new_added_state;


    for(size_t i = 0; i < current_candidate.size(); ++i)
    {
        //existence check!!
        auto exists_result = group_state.entire_state.find(current_candidate_string[i]);
        if (exists_result != group_state.entire_state.end())
        {
            exists_result->second.end_frame = frame_number;
            //appearance update for my children
            exists_result->second.subset_update(frame_number, exists_result->second.start_frame);
        } else
        {
            //insert the new state
            exists_result = group_state.entire_state.insert(make_pair(current_candidate_string[i]
                    , SSG_State(current_candidate[i], frame_number, frame_number, nullptr, false))).first;

        }
        bool no_parent = true;
        auto pslist_snapshot = group_state.PSList.size();
        size_t psIdx = 0;
        for(auto state = group_state.PSList.begin(); psIdx < pslist_snapshot && psIdx < group_state.PSList.size();++psIdx)
        {
            size_t visit_result = visit_state(state
                    , frame_number
                    , exists_result
                    ,thres_g
                    ,group_state); //0: state erased 1: superset exists 2: common

            if(visit_result == 1)
            {
                no_parent = false;
            }
            if(visit_result != 0)
            {
                ++state;
            }
        }
        if(no_parent) //new state, there is no superset of me
        {
            group_state.PSList.insert(make_pair(current_candidate_string[i],
                                                &exists_result->second));
        }

    }

    //occlusion & id_switch handling

//    cout << "erased state" << endl;

    size_t temp = group_state.get_memory_consumption();
    if(memory_consumption[3] < temp)
    {
        memory_consumption[3] = temp;
    }


    for(auto iter = group_state.entire_state.begin(); iter != group_state.entire_state.end();)
    {
        //TODO: which one is better?
        // 1. # of occlusions are larger than thres_f * thres_o
        // 2. # of occlusions are larger than duration from first-appeared frame * thres_o
        // 2-2. if a group appeared and occluded in next frame, this group is erased immediately.

        if(frame_number == video_length-1)
        {
            if(thres_f < (int)iter->second.end_frame - (int)iter->second.start_frame)
            {
                answer.emplace_back(State(iter->second.group
                        , iter->second.start_frame
                        , iter->second.end_frame
                        ,nullptr));
                iter++;
            }else
            {

                iter++;
            }
        }else
        {
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
                    if(SSG_maximal_segment_check(&iter->second, answer))
                    {
//                        if(iter->first[0]->id == 15
//                           && iter->first[1]->id == 277
//                           && iter->first[2]->id == 278
//                           && iter->first.size() == 3)
//                        {
//                            cout <<" I'm here!" << endl;
//                        }

                        answer.emplace_back(State(iter->second.group
                                , iter->second.start_frame
                                , iter->second.end_frame
                                ,nullptr));
                    }
                }


                if(iter->second.parents.empty())//내가 ps라면
                {

                    for(auto& child : iter->second.children)
                    {
                        child.second->parents.erase(iter->first);
                        if(child.second->parents.empty())
                        {
                            group_state.PSList.insert(make_pair(child.first, child.second)); //ps로 만들어준다
                        }

                    }
                    group_state.PSList.erase(iter->first);
                } else //내가 ps가 아니라 중간노드라면
                {
                    for(auto& child : iter->second.children)
                    {
                        child.second->parents.erase(iter->first); //나를 지우고

                        //이미 내 자식을 내 부모가 갖고 있다면? Unordered map이니까 삽입 안 되고 끝남
                        for(auto& myparent : iter->second.parents)
                        {
                            myparent.second->children.insert(make_pair(child.first, child.second));

                            child.second->parents.insert(
                                    make_pair(myparent.first, myparent.second));
                        }

                    }

                    for(auto& parent : iter->second.parents)
                    {
                        parent.second->children.erase(iter->first);
                    }
                }

                group_state.entire_state.erase(iter++);

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



void interframe_processing_SSG(const size_t& frame_number
        , const size_t& video_length
        , const size_t& thres_g
        , const size_t& thres_f
        , const double& thres_o
        , vector<vector<obj*>>& maximal_groups
        , SSG& group_state
        , list<State>& answer
        , vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption){

    SSG_CM(frame_number
            , video_length
            , thres_g
            , thres_f
            , thres_o
            , maximal_groups
            , group_state
            , answer
            , interframe_time_stats
            , memory_consumption);


//    cout << "inter finished" << endl;
}