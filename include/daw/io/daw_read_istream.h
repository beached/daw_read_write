// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_read_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <istream>
#include <optional>
#include <span>

namespace daw::io {
	template<>
	struct ReadableInput<std::istream> {
		using char_type = typename std::istream::char_type;
		template<typename Byte>
		static IOOpResult read( std::istream &is, std::span<Byte> buff ) {
			if( is.eof( ) ) {
				return { IOOpStatus::Eof, 0 };
			} else if( not is.good( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto *ptr = [&] {
				if constexpr( std::is_same_v<char_type, Byte> ) {
					return buff.data( );
				} else {
					return reinterpret_cast<char_type *>( buff.data( ) );
				}
			}( );
			is.read( ptr, static_cast<std::streamsize>( buff.size( ) ) );
			auto num_read = is.gcount( );
			assert( num_read >= 0 );
			if( is.eof( ) ) {
				return { IOOpStatus::Eof, static_cast<std::size_t>( num_read ) };
			}
			return { is.good( ) ? IOOpStatus::Ok : IOOpStatus::Error,
			         static_cast<std::size_t>( num_read ) };
		}

		template<typename Byte>
		static inline IOOpResult get( std::istream &is, Byte &c ) {
			if( is.eof( ) ) {
				return { IOOpStatus::Eof, 0 };
			} else if( not is.good( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto i = is.get( );
			using traits = typename std::istream::traits_type;
			if( i == traits::eof( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			c = static_cast<Byte>( i );
			if( is.eof( ) ) {
				return { IOOpStatus::Eof, 1 };
			}
			return { is.good( ) ? IOOpStatus::Ok : IOOpStatus::Error, 1 };
		}

		static inline std::size_t max_peek_size( std::istream const &is ) {
			return is.good( ) ? 1 : 0;
		}

		template<typename Byte>
		static inline IOOpResult peek( std::istream &is, std::span<Byte> buff ) {
			if( is.eof( ) ) {
				return { IOOpStatus::Eof, 0 };
			} else if( not is.good( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto i = is.peek( );
			using traits = typename std::istream::traits_type;
			if( i == traits::eof( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			buff[0] = static_cast<Byte>( i );
			if( is.eof( ) ) {
				return { IOOpStatus::Eof, 1 };
			}
			return { is.good( ) ? IOOpStatus::Ok : IOOpStatus::Error, 1 };
		}
	};
} // namespace daw::io
