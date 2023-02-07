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

	/// Attempt to use the type_writer interface when args isn't a string like
	/// parameter.
	template<typename Writer, typename... Ts>
	[[nodiscard]] IOOpResult write_all( Writer &writer, Ts &&...args ) {
		auto result = IOOpResult{ };
		auto write_item = [&]( auto &&item ) {
			using item_t = DAW_TYPEOF( item );
			auto item_result = IOOpResult{ };
			if( result.status != IOOpStatus::Ok ) {
				return;
			}
			if constexpr( std::is_constructible_v<char, item_t> and
			              sizeof( item_t ) == 1 ) {
				item_result = writer.write( daw::string_view( &item, 1 ) );
			} else if constexpr( std::is_constructible_v<daw::string_view,
			                                             decltype( item )> ) {
				item_result = writer.write( item );
			} else {
				static_assert( has_type_writer_v<item_t>,
				               "Unsupported type.  Maybe a to_string is needed" );
				item_result = type_writer( writer, item );
			}
			result.status = item_result.status;
			result.count += item_result.count;
		};
		int expander[] = { ( write_item( args ), 1 )... };
		(void)expander;
		return result;
	}

	struct incorrect_param_count_exception {
		explicit incorrect_param_count_exception( ) = default;
	};
	template<typename... Ts>
	struct format_str {
		daw::string_view value;

		template<typename T,
		         std::enable_if_t<std::is_constructible_v<daw::string_view, T>,
		                          std::nullptr_t> = nullptr>
		consteval format_str( T &&fmt_str ) noexcept
		  : value( fmt_str ) {
			check( );
		}

		consteval void check( ) {
			auto str = daw::string_view( value );
			constexpr std::size_t size_needed = sizeof...( Ts );
			std::size_t param_count = 0;
			std::size_t brace_count = 0;
			while( not str.empty( ) ) {
				str.remove_prefix_until( '{', nodiscard );
				if( str.starts_with( '{' ) ) {
					str.remove_prefix( );
					str.trim_prefix( );
					++brace_count;
					if( brace_count % 2 == 1 and str.starts_with( '}' ) ) {
						str.remove_prefix( );
						brace_count = 0;
						++param_count;
					}
				}
			}
			if( param_count != size_needed ) {
				throw incorrect_param_count_exception{ };
			}
		}
	};

	template<typename Writer, typename... Ts>
	IOOpResult print( Writer &writer,
	                  format_str<std::type_identity_t<Ts>...> format_string,
	                  Ts &&...args ) {
		auto fmt_str = format_string.value;
		std::size_t brace_count = 0;
		auto last_result = IOOpResult{ };
		auto process = [&]( auto &&arg ) -> IOOpResult {
			if( last_result.status != IOOpStatus::Ok ) {
				return { last_result.status, 0 };
			}
			char const *first = fmt_str.data( );
			while( not fmt_str.empty( ) ) {
				auto part = fmt_str.pop_front_until( '{' );
				fmt_str.trim_prefix( );
				++brace_count;
				if( brace_count % 2 == 1 and fmt_str.starts_with( '}' ) ) {
					brace_count = 0;
					fmt_str.remove_prefix( );
					last_result =
					  write_all( writer, daw::string_view( first, part.data_end( ) ),
					             DAW_FWD( arg ) );
					if( not fmt_str.empty( ) ) {
						first = fmt_str.data( );
					}
					return last_result;
				}
			}
			// This should never be reached as we know there are the same number of {}
			// as args
			std::abort( );
		};
		IOOpResult results[] = { process( DAW_FWD( args ) )... };
		if( last_result.status == IOOpStatus::Ok ) {
			auto tmp = writer.write( fmt_str );
			last_result.status = tmp.status;
			last_result.count += tmp.count;
		}
		auto total_size =
		  std::accumulate( std::data( results ), daw::data_end( results ),
		                   std::size_t{ 0 }, []( std::size_t t, auto const &r ) {
			                   return t + r.count;
		                   } );
		return { last_result.status, total_size };
	}
} // namespace daw::io::type_writer