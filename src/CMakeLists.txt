include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/stb/)
aux_source_directory(./ SRC)
add_library(${PROJECT_NAME} SHARED ${SRC})