// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw/io/daw_read_write.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_string_view.h>

#include <type_traits>

namespace daw::io::type_writer {
	namespace impl {
		inline constexpr auto digits100 = [] {
			std::array<char[2], 100> result{ };
			for( size_t n = 0; n < 100; ++n ) {
				result[n][0] =
				  static_cast<char>( ( n % 10 ) + static_cast<unsigned char>( '0' ) );
				result[n][1] =
				  static_cast<char>( ( n / 10 ) + static_cast<unsigned char>( '0' ) );
			}
			return result;
		}( );

		template<typename T>
		using base_int_type_t =
		  typename std::conditional_t<std::is_enum_v<T>, std::underlying_type<T>,
		                              daw::traits::identity<T>>::type;

		template<typename T>
		static constexpr void reverse( T *first, T *last ) {
			// Assume preconditions. This helps on CE in codegen but may
			// not matter here with inlining
			DAW_ASSUME( first and last );
			DAW_ASSUME( first <= last );
			auto rpos = last - first;
			auto lpos = 0;
			while( lpos < rpos ) {
				--rpos;
				auto tmp = DAW_MOVE( first[lpos] );
				first[lpos] = DAW_MOVE( first[rpos] );
				first[rpos] = DAW_MOVE( tmp );
				++lpos;
			}
		}

		template<typename Integer, typename WriteableType>
		[[nodiscard]] static constexpr daw::io::IOOpResult
		write_unsigned( WriteableType &writer, Integer const &value ) {
			using std::to_string;
			using under_type = base_int_type_t<Integer>;

			if constexpr( std::is_same_v<Integer, bool> ) {
				if( static_cast<bool>( value ) ) {
					return writer.put( '1' );
				} else {
					return writer.put( '0' );
				}
			} else if constexpr( std::disjunction_v<std::is_enum<Integer>,
			                                        daw::is_integral<Integer>> ) {
				auto v = static_cast<under_type>( value );

				if( DAW_UNLIKELY( v == 0 ) ) {
					return writer.put( '0' );
				} else {
					daw_io_assert( v > 0, "Unexpected number value" );
					char buff[daw::numeric_limits<under_type>::digits10 + 10]{ };
					char *ptr = buff;
					while( v >= 10 ) {
						auto const tmp = v % 100U;
						v /= 100U;
						ptr[0] = digits100[tmp][0];
						ptr[1] = digits100[tmp][1];
						ptr += 2;
					}
					if( v > 0 ) {
						*ptr++ = static_cast<char>( '0' + static_cast<char>( v ) );
					}
					reverse( buff, ptr );
					return writer.write( daw::string_view( +buff, ptr ) );
				}
			}
			using std::to_string;
			auto str_val = to_string( value );
			return writer.write( str_val );
		}

		template<typename Integer, typename WriteableType>
		[[nodiscard]] static constexpr daw::io::IOOpResult
		write_signed( WriteableType &writer, Integer const &value ) {
			using std::to_string;
			using under_type = base_int_type_t<Integer>;

			if constexpr( std::disjunction_v<std::is_enum<Integer>,
			                                 daw::is_integral<Integer>> ) {
				auto v = static_cast<under_type>( value );

				char buff[daw::numeric_limits<under_type>::digits10 + 10]{ };
				char *num_start = buff;
				char *ptr = buff;
				if( v < 0 ) {
					*ptr++ = '-';
					++num_start;
					// Do 1 round here just in case we are
					// daw::numeric_limits<intmax_t>::min( ) and cannot negate
					// This is a subtraction because when v < 0, v % 100 is negative
					auto const tmp = -static_cast<std::size_t>( v % 10 );
					v /= -10;
					*ptr++ = digits100[tmp][0];
					if( v == 0 ) {
						return writer.write( daw::string_view( +buff, ptr ) );
					}
				}

				if( v == 0 ) {
					*ptr++ = '0';
				}
				while( v >= 10 ) {
					auto const tmp = static_cast<std::size_t>( v % 100 );
					v /= 100;
					ptr[0] = digits100[tmp][0];
					ptr[1] = digits100[tmp][1];
					ptr += 2;
				}
				if( v > 0 ) {
					*ptr++ = static_cast<char>( '0' + static_cast<char>( v ) );
				}

				reverse( num_start, ptr );
				return writer.write( daw::string_view( +buff, ptr ) );
			}
			// Fallback to ADL
			using std::to_string;
			auto str_val = to_string( value );
			return writer.write( str_val );
		}
	} // namespace impl
	template<
	  typename Integer, typename Writer,
	  std::enable_if_t<daw::is_integral_v<Integer>, std::nullptr_t> = nullptr>
	daw::io::IOOpResult write_integer( Writer &writer, Integer const &i ) {
		if constexpr( daw::is_unsigned_v<Integer> ) {
			return impl::write_unsigned( writer, i );
		} else {
			return impl::write_signed( writer, i );
		}
	}
} // namespace daw::io::type_writer
