//
// Created by Hyunsik Yoon on 2022-08-15.
//

#include "interframe_lazy_notrie.hpp"
//
// Created by Hyunsik Yoon on 2022-07-27.
//





bool maximal_segment_check_notrie(notrie_state& track
        ,list<notrie_state>& answers)
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

string otos_notrie(vector<obj*>& group)
{
    string g = "";

    for(auto& obj : group)
    {

        g += to_string(obj->id);
        g += ",";
    }
    return g;
}
void appearance_update_phase_notrie(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,unordered_map<string, notrie_state>& tracks
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

        auto exists_result = tracks.find(otos_notrie(group));

        if(exists_result != tracks.end()) //exists
        {
            exists_result->second.end_frame = frame_number;
        } else
        {
            /* create new track */
            tracks.insert(make_pair(otos_notrie(group), notrie_state(group, frame_number, frame_number)));

        }
        /*subset update*/
        for(auto track : tracks)
        {
            if (track.second.group.size() < group.size())
            {
                if (includes(group.begin(), group.end(), (track.second.group).begin(), (track.second.group).end(),
                             [](obj *const &a, obj *const &b) -> bool
                             { return a->id < b->id; }))
                {
                    track.second.end_frame = frame_number;
                }
            }
        }
    }
}

void expire_phase_notrie(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,unordered_map<string, notrie_state>& tracks
        ,list<notrie_state>& answers
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

    auto track_snapshot = tracks.size();
    size_t track_idx = 0;
////
    for(auto track_iter = tracks.begin(); track_idx < track_snapshot && track_iter != tracks.end(); ++track_idx)
    {
        if(frame_number - (*track_iter).second.end_frame > thres_o)
        {
            if((*track_iter).second.end_frame - (*track_iter).second.start_frame > thres_f)
            {
                if(maximal_segment_check_notrie((*track_iter).second, answers)) //앞선 프레임에서 나온 결과들은 볼 필요 없다? 왜나면 last appearance가 무조건 현재 프레임 - thres_o보다 작다?
                {
                    answers.emplace_back((*track_iter).second);

                }
            }


//            //expire the state
            size_t t_track_idx = track_idx + 1;
            for(auto t_track_iter = next(track_iter); t_track_idx < track_snapshot; ++t_track_iter, ++t_track_idx)
            {
//                //삭제될 애들끼리 Intersection 하는건 무의미하다(snapshot 까지만 보기 때문에 필요한 것)

                if(frame_number - (*t_track_iter).second.end_frame > thres_o)
                {
                    continue;
                }
//
////                cout << "processing.."<< endl;
                vector<obj*> intersection(max((*t_track_iter).second.group.size(), (*track_iter).second.group.size())+1);
//
//
                auto intersect_iter = set_intersection(
                        (*track_iter).second.group.begin()
                        , (*track_iter).second.group.end()
                        , (*t_track_iter).second.group.begin()
                        , (*t_track_iter).second.group.end()
                        , intersection.begin()
                        , [](obj* const & a, obj* const & b) -> bool { return a->id < b->id;});
//
                intersection.resize(intersect_iter-intersection.begin());
//
                if(intersection.size() >= thres_g)
                {
                    auto exist_result = tracks.find(otos_notrie(intersection));
                    if(exist_result == tracks.end()) //not exists
                    {
                        //create a new track

                        tracks.insert(make_pair(
                                otos_notrie(intersection)
                                , notrie_state(intersection
                                        , min((*track_iter).second.start_frame, (*t_track_iter).second.start_frame)
                                        , max((*track_iter).second.end_frame, (*t_track_iter).second.end_frame))));

                        //there is no need to subset appearance update here
                    } else
                    {
                        exist_result->second.start_frame =
                                min(
                                        min(exist_result->second.start_frame ,(*track_iter).second.start_frame)
                                        , (*t_track_iter).second.start_frame
                                );
                        exist_result->second.end_frame =
                                max(
                                        max(exist_result->second.end_frame ,(*track_iter).second.end_frame)
                                        , (*t_track_iter).second.end_frame
                                );
                    }
                }
            }
//            //Expire the state
//            //remove from the trie, remove from the trie.states 이거 할 때 entry pointer 안 꼬이는지 확인
//            track_iter = tracks.lazy_expire(track_iter);
//            cout << tracks.size() << endl;
            tracks.erase(track_iter++);


//
////            cout << "expired" << endl;
        } else
        {
//            cout << tracks.size() << endl;
            ++track_iter;
//            cout << "not expired" << endl;
            continue;
        }
    }
}

void interframe_lazy_notrie(
        const size_t& frame_number
        ,const size_t& thres_g
        ,const size_t& thres_f
        ,const size_t& thres_o
        ,vector<vector<obj*>>& appeared_groups
        ,unordered_map<string, notrie_state>& tracks
        ,list<notrie_state>& answers
        ,vector<chrono::nanoseconds>& interframe_time_stats
        ,vector<size_t>& memory_consumption)
{

//    chrono::steady_clock::time_point appearance_update_start = chrono::steady_clock::now();

    appearance_update_phase_notrie(frame_number
            , thres_g
            , thres_f
            , thres_o
            , appeared_groups
            , tracks
            , interframe_time_stats
            , memory_consumption);
//    interframe_time_stats[0] += chrono::steady_clock::now() - appearance_update_start;


//    chrono::steady_clock::time_point expire_start = chrono::steady_clock::now();
    expire_phase_notrie(frame_number
            , thres_g
            , thres_f
            , thres_o
            , appeared_groups
            , tracks
            , answers
            , interframe_time_stats,memory_consumption);
//    interframe_time_stats[1] += chrono::steady_clock::now() - expire_start;
}