cmake_minimum_required(VERSION 3.0)

set(core_srcs
        thread.c
        config.c
        log.cc
        dictionary.c
        iniparser.c
)

add_library(core ${core_srcs})
target_link_libraries(core glog)