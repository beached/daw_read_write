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
	namespace io_details {
		struct ReadProxyImpl {
			explicit ReadProxyImpl( ) = default;
			constexpr virtual ~ReadProxyImpl( ) = default;

			virtual constexpr daw::unique_ptr<ReadProxyImpl> clone( ) const = 0;
			virtual constexpr IOOpResult read( std::span<char> sp ) = 0;
			virtual constexpr IOOpResult read( std::span<std::byte> sp ) = 0;
			virtual constexpr IOOpResult get( char &c ) = 0;
			virtual constexpr IOOpResult get( std::byte &b ) = 0;
			virtual constexpr std::size_t max_peek_size( ) const = 0;
			virtual constexpr IOOpResult peek( std::span<std::byte> sp ) = 0;
			virtual constexpr IOOpResult peek( std::span<char> sp ) = 0;
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

				constexpr std::size_t max_peek_size( ) const final {
					return ReadableInput<T>::max_peek_size( reader );
				}

				constexpr IOOpResult peek( std::span<char> sp ) final {
					return ReadableInput<T>::peek( reader, sp );
				}

				constexpr IOOpResult peek( std::span<std::byte> sp ) final {
					return ReadableInput<T>::peek( reader, sp );
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

		constexpr std::size_t max_peek_size( ) const {
			assert( reader );
			return reader->max_peek_size( );
		}

		constexpr IOOpResult peek( std::span<std::byte> sp ) {
			assert( reader );
			return reader->peek( sp );
		}

		constexpr IOOpResult peek( std::span<char> sp ) {
			assert( reader );
			return reader->peek( sp );
		}
	};

	template<typename Readable>
	struct Reader {
		struct data_t {
			Readable &readable_value;
		};
		daw::unique_ptr<data_t> data{ };

		explicit Reader( ) = default;
		explicit constexpr Reader( Readable &readable_value )
		  : data( daw::make_unique<data_t>( readable_value ) ) {}

		constexpr IOOpResult read( std::span<char> sp ) {
			assert( data );
			return ReadableInput<Readable>::read( data->readable_value, sp );
		}

		constexpr IOOpResult read( std::span<std::byte> sp ) {
			assert( data );
			return ReadableInput<Readable>::read( data->readable_value, sp );
		}

		constexpr IOOpResult get( char &c ) {
			assert( data );
			return ReadableInput<Readable>::get( data->readable_value, c );
		}

		constexpr IOOpResult get( std::byte &b ) {
			assert( data );
			return ReadableInput<Readable>::get( data->readable_value, b );
		}

		constexpr std::size_t max_peek_size( ) const {
			assert( data );
			return ReadableInput<Readable>::max_peek_size( data->readable_value );
		}

		constexpr IOOpResult peek( std::span<char> sp ) {
			assert( data );
			return ReadableInput<Readable>::peek( data->readable_value, sp );
		}

		constexpr IOOpResult peek( std::span<std::byte> sp ) {
			assert( data );
			return ReadableInput<Readable>::peek( data->readable_value, sp );
		}
	};
	template<typename ReadableType>
	Reader( ReadableType ) -> Reader<ReadableType>;
} // namespace daw::io
