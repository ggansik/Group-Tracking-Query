//
// Created by Hyunsik Yoon on 2022-07-27.
//

#include "interframe_lazy_inheritance.hpp"




bool maximal_segment_check(State& track
        ,list<State>& answers)
{
    for(auto answer_iter = answers.begin(); answer_iter != answers.end();)
    {
        if((*answer_iter).group.size() > track.group.size())
        {
            bool group_include = includes((*answer_iter).group.begin(), (*answer_iter).group.end(), track.group.begin(),
                                          track.group.end(), [](obj *const &a, obj *const &b) -> bool
                                          { return a->id < b->id; });

            bool lifetime_include =
                    (*answer_iter).start_frame <= track.start_frame && track.end_frame <= (*answer_iter).end_frame;

            if (group_include && lifetime_include)
            {
                return false;
            }
        } else if((*answer_iter).group.size() < track.group.size())
        {


            bool group_include = includes(track.group.begin()
                    , track.group.end()
                    , (*answer_iter).group.begin()
                    , (*answer_iter).group.end()
                    , [](obj *const &a, obj *const &b) -> bool
                                          { return a->id < b->id; });
            bool lifetime_include =
                    track.start_frame <= (*answer_iter).start_frame && (*answer_iter).end_frame <= track.end_frame;

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
void appearance_update_phase(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,SL_tracking_trie& tracks
        ,vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption)
{
    //TODO: for each group in appeared_groups:
    //  if the group exists in tracks:
    //      update the last appearance of the group as frame_number
    //      update the last appearance of the subsets of the group as frame_number
    //  else:
    //      create a new track (group, frame_number, frame_number)
    //      update the last appearance of the subsets in tracks
//    cout << "frame_number: " << frame_number << "---------------------" << endl;
    for(auto& group : appeared_groups)
    {


//        if(frame_number == 51)
//        {
//            cout << "hello" << endl;
//        }
//        for(auto& obj : group)
//        {
//            cout << obj->id << ",";
//        }
//        cout << endl;


        auto exists_result = tracks.exists(group);

        if(exists_result.first == group.size()) //exists
        {
            exists_result.second->entry->end_frame = frame_number;
            tracks.lazy_subset_update(group, frame_number);
        } else
        {
            auto lifetime = make_pair(frame_number, frame_number);
            tracks.lazy_add_new_track(group, lifetime, exists_result);
            tracks.lazy_subset_update(group, frame_number);
        }
    }
}
void expire_phase(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,const size_t& last_frame_no
        ,vector<vector<obj*>>& appeared_groups
        ,SL_tracking_trie& tracks
        ,list<State>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption)
{
    //TODO: for each track in tracks:
    //  if the track satisfies thres_o:
    //      if track satisfies thres_f:
    //          if track is the maximal in the answer list of all the frame???
    //      for each t in tracks:
    //          if(intersect(track, t).size() >= thres_g:
    //              if intersect(track, t) not in tracks:
    //                  create a new track (intersect(track, t), min(track.start, t.start), max(track.end, t,end)
    //              else:
    //                  update the track intersect(track, t)
    //                  with (
    //                  intersect(track, t)
    //                  , min(intersect(track, t).start, track.start, t.start)
    //                  , max(intersect(track, t).end, track.end, t.end))
    //      abort the track
    //  else: continue;

    auto track_snapshot = tracks.states.size();
//    cout << "track_snapshot" << track_snapshot << endl;
    size_t track_idx = 0;
//
//    if(frame_number == 719)
//    {
//        cout << "I'm here!" << endl;
//    }

    for(auto track_iter = tracks.states.begin(); track_idx < track_snapshot;  ++track_idx)
    {

        if(frame_number==last_frame_no-1)
        {
//            cout << tracks.states.size() << endl;
//
//            for(auto& id : (*track_iter).group)
//            {
//                cout << id->id << ",";
//            }
//            cout << ": [";
//            cout << (*track_iter).start_frame << ", ";
//            cout << (*track_iter).end_frame << "]" << endl;

            if((*track_iter).end_frame - (*track_iter).start_frame > thres_f)
            {
                if(maximal_segment_check((*track_iter), answers)) //앞선 프레임에서 나온 결과들은 볼 필요 없다? 왜나면 last appearance가 무조건 현재 프레임 - thres_o보다 작다?
                {
                    answers.emplace_back((*track_iter));
//                    for(auto& id : (*track_iter).group)
//                    {
//                        cout << id << ",";
//                    }
//                    cout << ": [";
//                    cout << (*track_iter).start_frame << ", ";
//                    cout << (*track_iter).end_frame << "]" << endl;
                }
            }

            ++track_iter;
            continue;
        } else
        {

            if (frame_number - (*track_iter).end_frame > thres_o)
            {
                //determine the answers
                if ((*track_iter).end_frame - (*track_iter).start_frame > thres_f)
                {
                    if (maximal_segment_check((*track_iter),
                                              answers)) //앞선 프레임에서 나온 결과들은 볼 필요 없다? 왜나면 last appearance가 무조건 현재 프레임 - thres_o보다 작다?
                    {
                        answers.emplace_back((*track_iter));
//                    for(auto& id : (*track_iter).group)
//                    {
//                        cout << id << ",";
//                    }
//                    cout << ": [";
//                    cout << (*track_iter).start_frame << ", ";
//                    cout << (*track_iter).end_frame << "]" << endl;
                    }
                }
//            for(auto& obj : track_iter->group)
//            {
//                cout << obj->id << ", ";
//            }
//            cout << endl;
                //expire the state
                size_t t_track_idx = track_idx + 1;
                for (auto t_track_iter = next(track_iter); t_track_idx < track_snapshot; ++t_track_iter, ++t_track_idx)
                {
                    //삭제될 애들끼리 Intersection 하는건 무의미하다(snapshot 까지만 보기 때문에 필요한 것)
//

//                if((*track_iter).end_frame - (*track_iter).start_frame <= thres_f
//                && (*t_track_iter).end_frame - (*t_track_iter).start_frame <= thres_f
//                && frame_number - (*t_track_iter).end_frame > thres_o)
//                {
//                    continue;
//                }
                    if (frame_number - (*t_track_iter).end_frame > thres_o)
                    {
                        continue;
                    }

//                cout << "processing.."<< endl;
                    vector<obj *> intersection(max((*t_track_iter).group.size(), (*track_iter).group.size()) + 1);


                    auto intersect_iter = set_intersection(
                            (*track_iter).group.begin(), (*track_iter).group.end(), (*t_track_iter).group.begin(),
                            (*t_track_iter).group.end(), intersection.begin(), [](obj *const &a, obj *const &b) -> bool
                            { return a->id < b->id; });

                    intersection.resize(intersect_iter - intersection.begin());

                    if (intersection.size() >= thres_g)
                    {

                        auto exist_result = tracks.exists(intersection);
                        if (exist_result.first < intersection.size() ||
                            exist_result.first > intersection.size()) //not exists
                        {
                            //create a new track
//                            auto lifetime = make_pair(min((*track_iter).start_frame, (*t_track_iter).start_frame),
//                                                      max((*track_iter).end_frame, (*t_track_iter).end_frame));
                            auto lifetime = make_pair(min((*track_iter).start_frame, (*t_track_iter).start_frame),
                                                      (*t_track_iter).end_frame);

                            tracks.lazy_add_new_track(intersection, lifetime, exist_result);

                            //there is no need to subset appearance update here
                        } else
                        {
//                            exist_result.second->entry->start_frame =
//                                    min(
//                                            min(exist_result.second->entry->start_frame, (*track_iter).start_frame),
//                                            (*t_track_iter).start_frame
//                                    );

                            exist_result.second->entry->start_frame =
                                    min(exist_result.second->entry->start_frame, (*track_iter).start_frame
                                            );



//                            exist_result.second->entry->end_frame =
//                                    max(
//                                            max(exist_result.second->entry->end_frame, (*track_iter).end_frame),
//                                            (*t_track_iter).end_frame
//                                    );
                        }
                    }
                }
                //Expire the state
                //remove from the trie, remove from the trie.states 이거 할 때 entry pointer 안 꼬이는지 확인
                track_iter = tracks.lazy_expire(track_iter);

//            cout << "expired" << endl;
            } else
            {
                ++track_iter;
//            cout << "not expired" << endl;
                continue;
            }
        }
    }
}

void interframe_lazy(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,const size_t& last_frame_no
        ,vector<vector<obj*>>& appeared_groups
        ,SL_tracking_trie& tracks
        ,list<State>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        , vector<size_t>& memory_consumption)
{

//    chrono::steady_clock::time_point appearance_update_start = chrono::steady_clock::now();
    appearance_update_phase(frame_number
                            , thres_g
                            , thres_f
                            , thres_o
                            , appeared_groups
                            , tracks
                            , interframe_time_stats
                    , memory_consumption);
//    interframe_time_stats[0] += chrono::steady_clock::now() - appearance_update_start;

    auto temp = tracks.get_memory_consumption();
    if(memory_consumption[3] < temp.first)
    {
        memory_consumption[3] = temp.first;
    }

    if(memory_consumption[4] < temp.second)
    {
        memory_consumption[4] = temp.second;
    }

//    chrono::steady_clock::time_point expire_start = chrono::steady_clock::now();
    expire_phase(frame_number
                , thres_g
                , thres_f
                , thres_o
                , last_frame_no
                , appeared_groups
                , tracks
                , answers
                , interframe_time_stats,memory_consumption);
//    interframe_time_stats[1] += chrono::steady_clock::now() - expire_start;
}