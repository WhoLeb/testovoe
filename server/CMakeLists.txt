include_directories(./)

file(GLOB all_srcs "./*.cpp" "./*.h")

add_executable(server ${all_srcs})