// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw/io/daw_write_proxy.h"

#include <ostream>
#include <utility>

namespace daw::io {
	class write_streambuf : public std::basic_streambuf<char> {
		WriteProxy m_writer;

		template<typename WritableType>
		static constexpr bool is_writable_type =
		  not std::is_same_v<write_streambuf, WritableType> and
		  not std::is_same_v<WriteProxy, WritableType> and
		  not std::is_const_v<WritableType>;

	public:
		template<typename WritableType,
		         std::enable_if_t<is_writable_type<WritableType>, std::nullptr_t> =
		           nullptr>
		explicit inline write_streambuf( WritableType &writer )
		  : m_writer( writer ) {}

		explicit inline write_streambuf( WriteProxy const &writer )
		  : m_writer( writer ) {}

		explicit inline write_streambuf( WriteProxy &&writer )
		  : m_writer( std::move( writer ) ) {}

		inline int overflow( int_type c = traits_type::eof( ) ) override {
			if( not traits_type::eq_int_type( c, traits_type::eof( ) ) ) {
				auto ret = m_writer.put( static_cast<char>( c ) );
				if( ret.status != IOOpStatus::Ok ) {
					return traits_type::eof( );
				}
			}
			return traits_type::not_eof( c );
		}

		inline std::streamsize xsputn( char_type const *str,
		                               std::streamsize n ) override {
			auto ret = m_writer.write(
			  daw::string_view( str, static_cast<std::size_t>( n ) ) );
			if( ret.status != IOOpStatus::Ok ) {
				return traits_type::eof( );
			}
			return static_cast<std::streamsize>( ret.count );
		}

		WriteProxy const &writer( ) const {
			return m_writer;
		}

		WriteProxy &writer( ) {
			return m_writer;
		}
	};

	class write_ostream : public std::ostream {
		write_streambuf m_stream;

		template<typename WritableType>
		static constexpr bool is_writable_type =
		  not std::is_same_v<write_ostream, WritableType> and
		  not std::is_same_v<WriteProxy, WritableType> and
		  not std::is_const_v<WritableType>;

	public:
		template<typename WritableType,
		         std::enable_if_t<is_writable_type<WritableType>, std::nullptr_t> =
		           nullptr>
		explicit inline write_ostream( WritableType &writer )
		  : std::ostream( nullptr )
		  , m_stream( writer ) {
			rdbuf( std::addressof( m_stream ) );
		}

		explicit inline write_ostream( WriteProxy const &writer )
		  : std::ostream( nullptr )
		  , m_stream( writer ) {
			rdbuf( std::addressof( m_stream ) );
		}

		explicit inline write_ostream( WriteProxy &&writer )
		  : std::ostream( nullptr )
		  , m_stream( writer ) {
			rdbuf( std::addressof( m_stream ) );
		}

		WriteProxy const &writer( ) const {
			return m_stream.writer( );
		}

		WriteProxy &writer( ) {
			return m_stream.writer( );
		}
	};
} // namespace daw::io
