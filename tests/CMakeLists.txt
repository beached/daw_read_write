# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_read_write
#

include( cmake/test_compiler_options.cmake )

if( MSVC )
    add_compile_options( -DDEBUG /permissive- /EHsc )
    #add_compile_options( -DDEBUG /permissive- /EHsc /fsanitize=address )
    #add_link_options( /fsanitize=address -fno-omit-frame-pointer )
else()
    #add_compile_options( -fsanitize=address,undefined )
endif()

add_library( daw_read_write_test_lib INTERFACE )
target_link_libraries( daw_read_write_test_lib INTERFACE daw::daw-read-write )
target_include_directories( daw_read_write_test_lib INTERFACE include/ )
target_compile_options( daw_read_write_test_lib INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/permissive-> )

add_executable( daw_read_write_bin src/daw_read_write_test.cpp )
target_link_libraries( daw_read_write_bin PRIVATE daw_read_write_test_lib )
#target_link_options( daw_read_write_bin PRIVATE -fsanitize=address,undefined )
add_test( NAME daw_read_write_test COMMAND daw_read_write_bin )

