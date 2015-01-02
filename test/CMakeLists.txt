include_directories(${LIBEASTWOOD_SOURCE_DIR}/include)

add_executable(testy test.cpp)
target_link_libraries(testy eastwood)