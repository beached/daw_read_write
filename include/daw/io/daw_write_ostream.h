// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_write_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <numeric>
#include <ostream>
#include <span>

namespace daw::io {
	template<>
	struct WritableOutput<std::ostream> {
		[[nodiscard]] static inline IOOpResult write( std::ostream &os,
		                                              daw::string_view sv ) {
			if( not os.good( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto old_pos = os.tellp( );
			os.write( sv.data( ), static_cast<std::streamsize>( sv.size( ) ) );
			if( os.good( ) ) {
				return { IOOpStatus::Ok, sv.size( ) };
			}
			auto new_pos = os.tellp( );
			if( os.eof( ) ) {
				return { IOOpStatus::Eof,
				         new_pos >= 0 ? static_cast<std::size_t>( new_pos - old_pos )
				                      : 0 };
			}

			return { IOOpStatus::Error,
			         new_pos >= 0 ? static_cast<std::size_t>( new_pos - old_pos )
			                      : 0 };
		}

		[[nodiscard]] static inline IOOpResult
		write( std::ostream &os, std::initializer_list<daw::string_view> svs ) {
			std::size_t written = 0;
			for( daw::string_view const &sv : svs ) {
				auto const r = WritableOutput::write( os, sv );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}

		[[nodiscard]] static inline IOOpResult
		write( std::ostream &os, std::span<std::byte const> sp ) {
			if( not os.good( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto old_pos = os.tellp( );
			os.write( reinterpret_cast<char const *>( sp.data( ) ),
			          static_cast<std::streamsize>( sp.size( ) ) );
			if( os.good( ) ) {
				return { IOOpStatus::Ok, sp.size( ) };
			}
			auto new_pos = os.tellp( );
			if( os.eof( ) ) {
				return { IOOpStatus::Eof,
				         new_pos >= 0 ? static_cast<std::size_t>( new_pos - old_pos )
				                      : 0 };
			}

			return { IOOpStatus::Error,
			         new_pos >= 0 ? static_cast<std::size_t>( new_pos - old_pos )
			                      : 0 };
		}

		[[nodiscard]] static inline IOOpResult
		write( std::ostream &os,
		       std::initializer_list<std::span<std::byte const>> sps ) {
			std::size_t written = 0;
			for( std::span<std::byte const> const &sp : sps ) {
				auto const r = WritableOutput::write( os, sp );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}
	};
} // namespace daw::io
