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
#include <cstdio>
#include <initializer_list>
#include <numeric>
#include <span>

namespace daw::io {
	template<>
	struct WritableOutput<FILE *> {
		[[nodiscard]] static inline IOOpResult write( FILE *fp,
		                                              daw::string_view sv ) {
			auto result = std::fwrite( sv.data( ), 1, sv.size( ), fp );
			if( std::feof( fp ) ) {
				return { IOOpStatus::Eof, result };
			}
			if( std::ferror( fp ) ) {
				return { IOOpStatus::Error, result };
			}
			return { IOOpStatus::Ok, result };
		}

		[[nodiscard]] static inline IOOpResult
		write( FILE *fp, std::initializer_list<daw::string_view> svs ) {
			std::size_t written = 0;
			for( daw::string_view const &sv : svs ) {
				auto const r = WritableOutput::write( fp, sv );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}

		[[nodiscard]] static inline IOOpResult
		write( FILE *fp, std::span<std::byte const> sp ) {
			auto result = std::fwrite( sp.data( ), 1, sp.size( ), fp );
			if( std::feof( fp ) ) {
				return { IOOpStatus::Eof, result };
			}
			if( std::ferror( fp ) ) {
				return { IOOpStatus::Error, result };
			}
			return { IOOpStatus::Ok, result };
		}

		static constexpr IOOpResult
		write( FILE *fp, std::initializer_list<std::span<std::byte const>> sps ) {
			std::size_t written = 0;
			for( std::span<std::byte const> const &sp : sps ) {
				auto const r = WritableOutput::write( fp, sp );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}

		template<typename B>
		[[nodiscard]] static inline IOOpResult put( FILE *fp, B b ) {
			static_assert( daw::traits::is_one_of_v<B, char, std::byte> );
			(void)std::fputc( static_cast<char>( b ), fp );
			if( std::feof( fp ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			if( std::ferror( fp ) ) {
				return { IOOpStatus::Error, 0 };
			}
			return { IOOpStatus::Ok, 1 };
		}
	};
} // namespace daw::io
