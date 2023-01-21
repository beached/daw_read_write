// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_io_fd_wrap.h"
#include "daw_write_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <numeric>
#include <span>
#include <unistd.h>

namespace daw::io {
	template<>
	struct WritableOutput<fd_wrap_t> {
		[[nodiscard]] static inline IOOpResult write( fd_wrap_t fd,
		                                              daw::string_view sv ) {
			std::size_t total = 0;
			while( not sv.empty( ) ) {
				auto result = ::write( fd.value, sv.data( ), sv.size( ) );
				if( result < 0 ) {
					return { IOOpStatus::Error, total };
				}
				auto const szt = static_cast<std::size_t>( result );
				if( szt == sv.size( ) ) {
					return { IOOpStatus::Ok, total + szt };
				}
				assert( szt < sv.size( ) );
				total += szt;
				sv.remove_prefix( szt );
			}
			assert( total == 0 );
			return { IOOpStatus::Ok, 0 };
		}

		[[nodiscard]] static inline IOOpResult
		write( fd_wrap_t fd, std::initializer_list<daw::string_view> svs ) {
			std::size_t written = 0;
			for( daw::string_view const &sv : svs ) {
				auto const r = WritableOutput::write( fd, sv );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}

		[[nodiscard]] static inline IOOpResult
		write( fd_wrap_t fd, std::span<std::byte const> sp ) {
			std::size_t total = 0;
			while( not sp.empty( ) ) {
				auto result = ::write( fd.value, sp.data( ), sp.size( ) );
				if( result < 0 ) {
					return { IOOpStatus::Error, total };
				}
				auto const szt = static_cast<std::size_t>( result );
				if( szt == sp.size( ) ) {
					return { IOOpStatus::Ok, total + szt };
				}
				assert( szt < sp.size( ) );
				total += szt;
				sp = sp.subspan( szt );
			}
			assert( total == 0 );
			return { IOOpStatus::Ok, 0 };
		}

		static constexpr IOOpResult
		write( fd_wrap_t fd,
		       std::initializer_list<std::span<std::byte const>> sps ) {
			std::size_t written = 0;
			for( std::span<std::byte const> const &sp : sps ) {
				auto const r = WritableOutput::write( fd, sp );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}
	};
} // namespace daw::io
