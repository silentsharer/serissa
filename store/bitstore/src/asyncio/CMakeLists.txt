cmake_minimum_required(VERSION 3.0)

set(aio_srcs
        aio.c
        aiocontext.c
        blockdevice.c
)

add_definitions(-D HAVE_LIBAIO)
add_library(asyncio ${aio_srcs})
target_link_libraries(asyncio core)
