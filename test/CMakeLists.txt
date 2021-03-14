include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
aux_source_directory(./ SRC)
add_executable(ut ${SRC})
target_link_libraries(ut gtest stb_wrapper pthread)