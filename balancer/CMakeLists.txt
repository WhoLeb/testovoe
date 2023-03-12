include_directories(./)

file(GLOB all_srcs "./*.cpp" "./*.h")

add_executable(balancer ${all_srcs})