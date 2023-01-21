// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw/io/daw_read_proxy.h"
#include "daw/io/daw_write_proxy.h"

namespace daw::io::util {
	struct CopyResult {
		IOOpResult read_result;
		IOOpResult write_result;
	};
	namespace util_details {
		struct copy_op {
			explicit copy_op( ) = default;
		};
	} // namespace util_details

	template<std::size_t BuffSize = 4096U>
	struct transform_n_t {
		explicit transform_n_t( ) = default;

		template<typename T, typename U, typename Func>
		[[nodiscard]] constexpr CopyResult
		operator( )( Writer<T> &writer, Reader<U> &reader, std::size_t count,
		             Func &&func ) {
			static_assert( BuffSize > 0 );
			std::byte buffer[BuffSize];
			auto read_result = IOOpResult{ };
			auto write_result = IOOpResult{ };
			while( count > 0 and read_result.status == IOOpStatus::Ok and
			       write_result.status == IOOpStatus::Ok ) {
				auto buff_sp =
				  std::span<std::byte>( buffer, std::min( count, BuffSize ) );
				auto const rr = reader.read( buff_sp );
				read_result.status = rr.status;
				read_result.count += rr.count;
				count -= rr.count;
				if( rr.count > 0 ) {
					if constexpr( not std::is_same_v<util_details::copy_op,
					                                 std::remove_cvref_t<Func>> ) {
						(void)daw::algorithm::transform_n( buffer, buffer, rr.count, func );
					}
					buff_sp = std::span<std::byte>( buffer, rr.count );
					auto const wr = writer.write( buff_sp );
					write_result.status = wr.status;
					write_result.count += wr.count;
				}
			}
			return { read_result, write_result };
		}
	};

	template<>
	struct transform_n_t<1> {
		explicit transform_n_t( ) = default;

		template<typename T, typename U, typename Func>
		[[nodiscard]] constexpr CopyResult
		operator( )( Writer<T> &writer, Reader<U> &reader, std::size_t count,
		             Func &&func ) {
			auto read_result = IOOpResult{ };
			auto write_result = IOOpResult{ };
			while( count > 0 and read_result.status == IOOpStatus::Ok and
			       write_result.status == IOOpStatus::Ok ) {
				auto buff = std::byte{ 0 };
				auto const rr = reader.get( buff ).status;
				read_result.status = rr.status;
				read_result.count += rr.count;
				count -= rr.count;
				if( rr.count > 0 ) {
					if constexpr( not std::is_same_v<util_details::copy_op,
					                                 std::remove_cvref_t<Func>> ) {
						buff = func( buff );
					}
					auto const wr = writer.put( buff );
					write_result.status = wr.status;
					write_result.count += wr.couunt;
				}
			}
			return { read_result, write_result };
		}
	};

	inline constexpr auto transform_n = transform_n_t<4096>{ };

	template<std::size_t BuffSize>
	inline constexpr auto transform_n_b = transform_n_t<BuffSize>{ };

	template<std::size_t BuffSize = 4096U, typename T, typename U>
	[[nodiscard]] constexpr CopyResult
	copy_n( Writer<T> &writer, Reader<U> &reader, std::size_t count ) {
		return transform_n_b<BuffSize>( writer, reader, count,
		                                util_details::copy_op{ } );
	}

	template<std::size_t BuffSize = 4096U>
	struct transform_t {
		explicit transform_t( ) = default;

		template<typename T, typename U, typename Func>
		[[nodiscard]] constexpr CopyResult
		operator( )( Writer<T> &writer, Reader<U> &reader, Func &&func ) {
			static_assert( BuffSize > 0 );
			std::byte buffer[BuffSize];
			auto read_result = IOOpResult{ };
			auto write_result = IOOpResult{ };
			while( read_result.status == IOOpStatus::Ok and
			       write_result.status == IOOpStatus::Ok ) {
				auto buff_sp = std::span<std::byte>( buffer, BuffSize );
				auto const rr = reader.read( buff_sp );
				read_result.status = rr.status;
				read_result.count += rr.count;
				if( rr.count > 0 ) {
					buff_sp = std::span<std::byte>( buffer, rr.count );
					if constexpr( not std::is_same_v<util_details::copy_op,
					                                 std::remove_cvref_t<Func>> ) {
						(void)daw::algorithm::transform_n( buffer, buffer, rr.count, func );
					}
					auto const wr = writer.write( buff_sp );
					write_result.status = wr.status;
					write_result.count += wr.count;
				}
			}
			return { read_result, write_result };
		}
	};

	template<>
	struct transform_t<1> {
		explicit transform_t( ) = default;

		template<typename T, typename U, typename Func>
		[[nodiscard]] constexpr CopyResult
		operator( )( Writer<T> &writer, Reader<U> &reader, Func &&func ) {
			auto read_result = IOOpResult{ };
			auto write_result = IOOpResult{ };
			while( read_result.status == IOOpStatus::Ok and
			       write_result.status == IOOpStatus::Ok ) {
				auto buff = std::byte{ 0 };
				auto const rr = reader.get( buff );
				read_result.status = rr.status;
				read_result.count += rr.count;
				if( rr.count > 0 ) {
					if constexpr( not std::is_same_v<util_details::copy_op,
					                                 std::remove_cvref_t<Func>> ) {
						buff = func( buff );
					}
					auto const wr = writer.put( buff );
					write_result.status = wr.status;
					write_result.count += wr.count;
				}
			}
			return { read_result, write_result };
		}
	};
	inline constexpr auto transform = transform_t<4096>{ };

	template<std::size_t BuffSize>
	inline constexpr auto transform_b = transform_t<BuffSize>{ };

	template<std::size_t BuffSize = 4096U, typename T, typename U>
	[[nodiscard]] constexpr CopyResult copy( Writer<T> &writer,
	                                         Reader<U> &reader ) {
		return transform_n_b<BuffSize>( writer, reader, util_details::copy_op{ } );
	}

} // namespace daw::io::util
