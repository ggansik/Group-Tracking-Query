cmake_minimum_required(VERSION 3.10)
project(videogroup)

set(CMAKE_CXX_STANDARD 14)

add_executable(videogroup main.cpp preprocessing.cpp preprocessing.hpp intraframe.cpp intraframe.hpp interframe.cpp interframe.hpp dataread.cpp dataread.hpp grid_utils.cpp grid_utils.hpp distance.cpp distance.hpp evaluation.cpp evaluation.h evaluation.cpp prefix_tree.hpp intraframe_trie.cpp intraframe_trie.hpp print_utils.hpp intraframe_buckets.cpp intraframe_buckets.hpp interframe_prefix.cpp interframe_prefix.hpp candidate_computation_test.cpp candidate_computation_test.hpp set_trie.hpp SL_set_trie.hpp intraframe_seedbased.hpp intraframe_seedbased.cpp interframe_lazy_inheritance.cpp interframe_lazy_inheritance.hpp interframe_hash.cpp interframe_hash.hpp ellipse_determination_statistically.cpp ellipse_determination_statistically.hpp interframe_lazy_notrie.cpp interframe_lazy_notrie.hpp)