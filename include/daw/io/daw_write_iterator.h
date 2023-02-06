// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_io_error.h"
#include "daw_write_proxy.h"

#include <daw/daw_move.h>

namespace daw::io {
	/// @brief A compatibility output iterator
	struct write_iterator {
		using value_type = void;
		using reference = void;
		using pointer = void;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::output_iterator_tag;

	private:
		WriteProxy m_writer;

	public:
		/// @tparam Writable A Writable type
		explicit constexpr write_iterator( WriteProxy const &wp )
		  : m_writer( wp ) {}

		explicit constexpr write_iterator( WriteProxy &&wp )
		  : m_writer( std::move( wp ) ) {}

		template<typename WritableType,
		         std::enable_if_t<
		           not std::is_const_v<std::remove_reference_t<WritableType>>,
		           std::nullptr_t> = nullptr>
		explicit constexpr write_iterator( WritableType &&writer )
		  : m_writer( DAW_FWD( writer ) ) {}

		constexpr write_iterator &operator++( ) noexcept {
			return *this;
		}

		constexpr write_iterator operator++( int ) noexcept {
			return *this;
		}

		constexpr write_iterator &operator*( ) noexcept {
			return *this;
		}

		constexpr write_iterator &operator=( char c ) {
			auto ret = m_writer.put( c );
			if( ret.status != IOOpStatus::Ok ) {
				io_impl::do_io_error( "Error writing" );
			}
			return *this;
		}

		constexpr write_iterator &operator=( daw::string_view sv ) {
			auto ret = m_writer.write( sv );
			if( ret.status != IOOpStatus::Ok ) {
				io_impl::do_io_error( "Error writing" );
			}
			return *this;
		}
	};

} // namespace daw::io
