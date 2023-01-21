// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_read
//

#pragma once

#include "daw_readable_input.h"

#include <daw/daw_assume.h>
#include <daw/daw_string_view.h>
#include <daw/daw_unique_ptr.h>

#include <cstddef>
#include <initializer_list>
#include <span>

namespace daw::io {

	template<typename Readable>
	class Reader {
		struct data_t {
			Readable &readable_value;
		};
		daw::unique_ptr<data_t> data{ };

	public:
		explicit Reader( ) = default;
		explicit constexpr Reader( Readable &readable_value )
		  : data( daw::make_unique<data_t>( readable_value ) ) {}

		template<typename Byte>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult
		read( std::span<Byte> sp ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			assert( data );
			return ReadableInput<Readable>::read( data->readable_value, sp );
		}

		template<typename Byte>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult get( Byte &c ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			assert( data );
			return ReadableInput<Readable>::get( data->readable_value, c );
		}
	};
	template<typename ReadableType>
	Reader( ReadableType ) -> Reader<ReadableType>;

	namespace io_details {
		struct ReadProxyImpl {
			explicit ReadProxyImpl( ) = default;
			constexpr virtual ~ReadProxyImpl( ) = default;

			virtual constexpr daw::unique_ptr<ReadProxyImpl> clone( ) const = 0;
			virtual constexpr IOOpResult read( std::span<char> sp ) = 0;
			virtual constexpr IOOpResult read( std::span<std::byte> sp ) = 0;
			virtual constexpr IOOpResult get( char &c ) = 0;
			virtual constexpr IOOpResult get( std::byte &b ) = 0;
		};

		namespace {
			template<typename T>
			class ReadProxyImplChild final : public ReadProxyImpl {
				T &reader;

			public:
				explicit constexpr ReadProxyImplChild( T &readable_value )
				  : reader( readable_value ) {}

				constexpr daw::unique_ptr<ReadProxyImpl> clone( ) const final;

				constexpr IOOpResult read( std::span<char> sp ) final {
					return ReadableInput<T>::read( reader, sp );
				}

				constexpr IOOpResult read( std::span<std::byte> sp ) final {
					return ReadableInput<T>::read( reader, sp );
				}

				constexpr IOOpResult get( char &c ) final {
					return ReadableInput<T>::get( reader, c );
				}

				constexpr IOOpResult get( std::byte &b ) final {
					return ReadableInput<T>::get( reader, b );
				}
			};

			template<typename T>
			constexpr daw::unique_ptr<ReadProxyImpl>
			ReadProxyImplChild<T>::clone( ) const {
				return daw::make_unique<ReadProxyImplChild<T>>( reader );
			}
		} // namespace
	}   // namespace io_details

	class ReadProxy {
		daw::unique_ptr<io_details::ReadProxyImpl> reader;

	public:
		template<typename T>
		explicit constexpr ReadProxy( T &readable_value )
		  : reader( daw::make_unique<io_details::ReadProxyImplChild<T>>(
		      readable_value ) ) {}

		constexpr ReadProxy( ReadProxy const &other )
		  : reader( other.reader ? other.reader->clone( ) : nullptr ) {}

		constexpr ReadProxy &operator=( ReadProxy const &rhs ) {
			if( not rhs.reader ) {
				reader.reset( );
			} else {
				reader = rhs.reader->clone( );
			}
			return *this;
		}

		constexpr ReadProxy( ReadProxy && ) noexcept = default;
		constexpr ReadProxy &operator=( ReadProxy && ) noexcept = default;
		constexpr ReadProxy( ) = default;

		constexpr IOOpResult read( std::span<char> sp ) {
			assert( reader );
			return reader->read( sp );
		}

		constexpr IOOpResult read( std::span<std::byte> sp ) {
			assert( reader );
			return reader->read( sp );
		}

		constexpr IOOpResult get( char &c ) {
			assert( reader );
			return reader->get( c );
		}

		constexpr IOOpResult get( std::byte &b ) {
			assert( reader );
			return reader->get( b );
		}
	};

	template<>
	class Reader<ReadProxy> {
		ReadProxy reader;

	public:
		explicit Reader( ) = default;
		explicit constexpr Reader( ReadProxy &&rp )
		  : reader( std::move( rp ) ) {}

		template<typename Byte>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult
		read( std::span<Byte> sp ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			return reader.read( sp );
		}

		template<typename Byte>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult get( Byte &b ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			return reader.get( b );
		}
	};
} // namespace daw::io
