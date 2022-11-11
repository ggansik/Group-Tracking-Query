//
// Created by Hyunsik Yoon on 2022-07-10.
//

#ifndef VIDEOGROUP_INTERFRAME_PREFIX_HPP
#define VIDEOGROUP_INTERFRAME_PREFIX_HPP

#include <map>
#include <vector>
#include "prefix_tree.hpp"

void interframe_processing_prefix_inheritance(map<size_t, map<size_t, prefix_tree>>& intra_candidate_bucket
                                                ,map<size_t, map<size_t, prefix_tree>>& group_state
                                                ,const size_t& current_frame_no
                                                ,const size_t& video_length
                                                ,const size_t& thres_g
                                                ,const size_t& thres_f
                                                ,const size_t& thres_o
                                                );

#endif //VIDEOGROUP_INTERFRAME_PREFIX_HPP
