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

} // namespace daw::io::type_writer