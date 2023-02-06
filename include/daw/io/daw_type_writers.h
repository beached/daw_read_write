// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_read_write.h"
#include "type_writers/daw_integer_writer.h"

#include <daw/cpp_17.h>

#include <type_traits>

namespace daw::io::type_writer {
	namespace impl {
		template<typename T>
		using has_type_writer_test = decltype( daw::io::type_writer::type_writer(
		  std::declval<daw::io::WriteProxy &>( ), std::declval<T const &>( ) ) );
	}

	template<typename T>
	inline constexpr bool has_type_writer_v =
	  daw::is_detected_v<impl::has_type_writer_test, T>;

	/// Attempt to use the type_writer interface when args isn't a string like parameter.
	template<typename Writer, typename...Ts>
	IOOpResult write_all( Writer & writer, Ts&&...args ) {
		auto result = IOOpResult{};
		auto write_item = [&]( auto && item ) {
			using item_t = DAW_TYPEOF( item );
			auto item_result = IOOpResult{};
			if( result.status != IOOpStatus::Ok ) {
				return;
			}
			if constexpr( daw::traits::is_string_view_like_v<item_t>) {
				item_result =  writer.write( item );
			} else {
				static_assert( has_type_writer_v<item_t>, "Unsupported type.  Maybe a to_string is needed" );
				item_result = type_writer( writer, item );
			}
			result.status = item_result.status;
			result.count += item_result.count;
		};
		int expander[] = { write_item( args )... };
		(void)expander;
		return result;
	}
} // namespace daw::io::type_writer