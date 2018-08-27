cmake_minimum_required(VERSION 3.0)

set(core_srcs
        config.c
        dictionary.c
        iniparser.c)

add_library(core ${core_srcs})