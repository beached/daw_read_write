// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include <daw/daw_algorithm.h>

#include <iterator>

namespace daw::io::io_details {
	template<typename CharT, typename ContiguousRange>
	[[nodiscard]] constexpr CharT *write_to_buffer( CharT *__restrict buff,
	                                                ContiguousRange &&sv ) {
		return daw::algorithm::transform_n( std::data( sv ), buff, std::size( sv ),
		                                    []( auto c ) {
			                                    return static_cast<CharT>( c );
		                                    } )
		  .output;
	}

} // namespace daw::io::io_details
