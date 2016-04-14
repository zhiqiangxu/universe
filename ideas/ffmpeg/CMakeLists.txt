project( ffmpeg )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(FFMPEG REQUIRED)

include_directories(${FFMPEG_INCLUDE_DIR})
add_executable( decoding_encoding decoding_encoding.c )
target_link_libraries( decoding_encoding ${FFMPEG_LIBRARIES} m pthread rt swresample )
