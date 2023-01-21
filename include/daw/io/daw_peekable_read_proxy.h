// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_read_proxy.h"
#include "daw_write_proxy.h"

#include <daw/daw_contiguous_view.h>
#include <daw/vector.h>

#include <cassert>
#include <cstddef>

namespace daw::io {
	struct PeekResult {
		IOOpResult io_result;
		daw::contiguous_view<std::byte const> buffer;
	};

	template<typename ReadableValue>
	class PeekableReader {
		daw::vector<std::byte> buffer{ };
		std::size_t idx_first = 0;
		Reader<ReadableValue> reader;

		inline std::size_t buffer_size( ) const {
			assert( idx_first <= buffer.size( ) );
			return buffer.size( ) - idx_first;
		}

		daw::contiguous_view<std::byte> read_buffer( std::size_t n ) {
			auto const amount =
			  static_cast<std::ptrdiff_t>( std::min( n, buffer_size( ) ) );
			auto first =
			  std::next( buffer.data( ), static_cast<std::ptrdiff_t>( idx_first ) );
			auto last = std::next( first, amount );
			return daw::contiguous_view( first, last );
		}

		daw::contiguous_view<std::byte> pop_buffer( std::size_t n ) {
			auto result = read_buffer( n );
			idx_first += result.size( );
			return result;
		}

	public:
		explicit constexpr PeekableReader( Reader<ReadableValue> r ) noexcept
		  : reader( std::move( r ) ) {}

		template<typename Byte>
		[[nodiscard]] constexpr IOOpResult read( std::span<Byte> sp ) {
			static_assert( daw::traits::is_one_of_v<Byte, char, std::byte> );
			auto *first = sp.data( );
			auto *const last = daw::data_end( sp );
			if( auto buff = pop_buffer( sp.size( ) ); not buff.empty( ) ) {
				first = daw::algorithm::convert_copy_n<Byte>( buff.data( ), first,
				                                              buff.size( ) );
			}
			if( first == last ) {
				return { IOOpStatus::Ok, sp.size( ) };
			}
			sp = std::span<Byte>( first, last );
			return reader.read( sp );
		}

		template<typename Byte>
		[[nodiscard]] constexpr IOOpResult get( Byte &c ) {
			static_assert( daw::traits::is_one_of_v<Byte, char, std::byte> );
			if( auto buff = pop_buffer( 1 ); not buff.empty( ) ) {
				c = static_cast<Byte>( buff[0] );
				return { IOOpStatus::Ok, 1 };
			}
			return reader.get( c );
		}

		[[nodiscard]] constexpr std::size_t max_peek_size( ) const {
			return buffer.max_size( );
		}

		constexpr void clear( ) {
			buffer.clear( );
			idx_first = 0;
		}

		[[nodiscard]] constexpr PeekResult peek( std::size_t sz ) {
			auto read_status = IOOpStatus::Ok;
			if( buffer_size( ) < sz ) {
				auto append_sz = sz - buffer_size( );
				(void)buffer.append_and_overwrite(
				  append_sz, [&]( std::byte *ptr, std::size_t size ) {
					  auto bsp = std::span<std::byte>( ptr, size );
					  auto result = reader.read( bsp );
					  read_status = result.status;
					  return result.count;
				  } );
			}
			auto buff = read_buffer( sz );
			return { IOOpResult{ read_status, buff.size( ) }, buff };
		}

		[[nodiscard]] constexpr PeekResult peek( ) {
			return peek( buffer_size( ) );
		}
	};
	template<typename ReadableValue>
	PeekableReader( Reader<ReadableValue> ) -> PeekableReader<ReadableValue>;
} // namespace daw::io
