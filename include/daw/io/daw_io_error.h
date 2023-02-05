// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write.h
//

#pragma once

#include <daw/daw_attributes.h>
#include <daw/daw_string_view.h>

namespace daw::io::io_impl {
	[[noreturn]] DAW_ATTRIB_NOINLINE inline void
	do_io_error( daw::string_view /*message*/ ) {
		std::terminate( );
	}
} // namespace daw::io::io_impl

#define daw_io_ensure( Condition, Message )         \
	do {                                              \
		if( not( Condition ) ) {                        \
			daw::io::io_impl::do_io_error( ( Message ) ); \
		}                                               \
	} while( false )

#if not defined( NDEBUG )
#define daw_io_assert( Condition, Message )         \
	do {                                              \
		if( not( Condition ) ) {                        \
			daw::io::io_impl::do_io_error( ( Message ) ); \
		}                                               \
	} while( false )
#else
#define daw_io_assert( Condition, Message ) \
	do {                                      \
	} while( false )
#endif