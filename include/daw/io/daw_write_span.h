// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_write_base.h"
#include "daw_write_to_buffer.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <numeric>
#include <span>
#include <string_view>

namespace daw::io {
	template<typename CharT, std::size_t Extent>
	struct WritableOutput<std::span<CharT, Extent>> {
		static_assert( Extent == std::dynamic_extent,
		               "Only dynamic extent span's are supported" );
		using value_type = std::span<CharT, Extent>;

		[[nodiscard]] static constexpr IOOpResult
		write( value_type &s, std::initializer_list<daw::string_view> svs ) {
			auto const total_sz =
			  std::accumulate( svs.begin( ), svs.end( ), std::size_t{ 0 },
			                   []( std::size_t sz, daw::string_view sv ) {
				                   return sz + sv.size( );
			                   } );
			if( s.size( ) < total_sz ) {
				return { IOOpStatus::Eof, 0 };
			}
			auto out_ptr = s.data( );
			for( daw::string_view const &sv : svs ) {
				out_ptr = io_details::write_to_buffer( out_ptr, sv );
			}
			s = s.subspan( total_sz );
			return { IOOpStatus::Ok, total_sz };
		}

		[[nodiscard]] static constexpr IOOpResult write( value_type &s,
		                                                 daw::string_view sv ) {
			if( s.size( ) < sv.size( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			(void)io_details::write_to_buffer( s.data( ), sv );
			s = s.subspan( sv.size( ) );
			return { IOOpStatus::Ok, sv.size( ) };
		}

		[[nodiscard]] static constexpr IOOpResult
		write( value_type &s, std::span<std::byte const> sp ) {
			if( s.size( ) < sp.size( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			(void)io_details::write_to_buffer( s.data( ), sp );
			s = s.subspan( sp.size( ) );
			return { IOOpStatus::Ok, sp.size( ) };
		}

		[[nodiscard]] static constexpr IOOpResult
		write( value_type &s,
		       std::initializer_list<std::span<std::byte const>> sps ) {
			auto const total_sz =
			  std::accumulate( sps.begin( ), sps.end( ), std::size_t{ 0 },
			                   []( std::size_t sz, daw::string_view sv ) {
				                   return sz + sv.size( );
			                   } );
			if( s.size( ) < total_sz ) {
				return { IOOpStatus::Eof, 0 };
			}
			auto out_ptr = s.data( );
			for( std::span<std::byte const> const &sv : sps ) {
				out_ptr = io_details::write_to_buffer( out_ptr, sv );
			}
			s = s.subspan( total_sz );
			return { IOOpStatus::Ok, total_sz };
		}

		[[nodiscard]] static constexpr IOOpResult put( value_type &writer,
		                                               char c ) {
			if( writer.empty( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			writer[0] = static_cast<CharT>( c );
			writer = writer.subspan( 1 );
			return { IOOpStatus::Ok, 1 };
		}

		[[nodiscard]] static constexpr IOOpResult put( value_type &writer,
		                                               std::byte b ) {
			if( writer.empty( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			writer[0] = static_cast<CharT>( b );
			writer = writer.subspan( 1 );
			return { IOOpStatus::Ok, 1 };
		}
	};
} // namespace daw::io
