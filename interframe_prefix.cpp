//
// Created by Hyunsik Yoon on 2022-07-10.
//

#include "interframe_prefix.hpp"

//void interframe_processing_prefix_inheritance(map<size_t, map<size_t, prefix_tree>>& intra_candidate_bucket
//                                              ,map<size_t, map<size_t, prefix_tree>>& group_state
//        ,const size_t& current_frame_no
//        ,const size_t& video_length
//        ,const size_t& thres_g
//        ,const size_t& thres_f
//        ,const size_t& thres_o
//)
//{
//    //reflect the current frame candidates
//    for(auto length_iter = intra_candidate_bucket.begin(); length_iter != intra_candidate_bucket.end(); ++length_iter)
//    {
//        for(auto trie_iter = length_iter->second.begin(); trie_iter != length_iter->second.end(); ++trie_iter)
//        {
//            // trie_iter, group_state 비교
//            // 만약 있으면
//            // 거기에다 Frame 추가
//
//            vector<vector<size_t>> leaves;
//
//            trie_iter->second.get_candidates(leaves);
//
//
//
//
//            auto group_length_iter = group_state.find(length_iter->first);
//            if(group_length_iter != group_state.end()) //bucket exists
//            {
//                auto group_trie_iter = group_length_iter->second.find(trie_iter->first);
//
//                if(group_trie_iter != group_length_iter->second.end()) //same root exists
//                {
//                    if(group_trie_iter->second.trie_match(trie_iter->second)) //trie끼리 비교
//                    {
//
//                    } else //no state
//                    {
//
//                    }
//                } else //no state
//                {
//                    group_length_iter->second.insert(make_pair(trie_iter->first, prefix_tree();
//                }
//            } else //no state
//            {
//
//            }
//
//            // 만약 없으면
//            // 내 superset, subset 찾아
//            // superset에서 frame 받아와 (occlusion은 안받아와도 됨. 왜냐면 ABCDE 중에 어떤게 occlusion되었다? 그럼
//            // 해당 거가 occlusion된 채로 group으로 나타났을 것
//            // subset에는 프레임 반영해줘
//
//            //
//
//
//        }
//    }
//
//    //based on updated state,  expiring candidate and inheritance are needed
//    for(auto length_iter = group_state)
//}
