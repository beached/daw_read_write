// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_writable_output.h"

#include <daw/daw_assume.h>
#include <daw/daw_string_view.h>
#include <daw/daw_unique_ptr.h>

#include <cstddef>
#include <initializer_list>
#include <span>

namespace daw::io {
	template<typename Writable>
	class Writer {
		struct data_t {
			Writable &writable_value;
		};
		daw::unique_ptr<data_t> data{ };

	public:
		constexpr Writer( Writable &writable_value ) noexcept
		  : data( daw::make_unique<data_t>( writable_value ) ) {}

		[[nodiscard]] constexpr IOOpResult write( daw::string_view sv ) {
			return WritableOutput<Writable>::write( data->writable_value, sv );
		}

		[[nodiscard]] constexpr IOOpResult
		write( std::initializer_list<daw::string_view> const &svs ) {
			return WritableOutput<Writable>::write( data->writable_value, svs );
		}

		[[nodiscard]] constexpr IOOpResult write( std::span<std::byte const> sp ) {
			return WritableOutput<Writable>::write( data->writable_value, sp );
		}

		[[nodiscard]] constexpr IOOpResult
		write( std::initializer_list<std::span<std::byte const>> const &sps ) {
			return WritableOutput<Writable>::write( data->writable_value, sps );
		}

		[[nodiscard]] constexpr IOOpResult put( std::byte b ) {
			return WritableOutput<Writable>::put( data->writable_value, b );
		}

		[[nodiscard]] constexpr IOOpResult put( char c ) {
			return WritableOutput<Writable>::put( data->writable_value, c );
		}
	};
	template<typename Writable>
	Writer( Writable ) -> Writer<Writable>;

	namespace io_details {
		struct WriteProxyImpl {
			explicit WriteProxyImpl( ) = default;
			constexpr virtual ~WriteProxyImpl( ) = default;

			[[nodiscard]] constexpr virtual daw::unique_ptr<WriteProxyImpl>
			clone( ) const = 0;
			[[nodiscard]] constexpr virtual IOOpResult write( daw::string_view ) = 0;
			[[nodiscard]] constexpr virtual IOOpResult
			write( std::initializer_list<daw::string_view> const & ) = 0;
			[[nodiscard]] constexpr virtual IOOpResult
			  write( std::span<std::byte const> ) = 0;
			[[nodiscard]] constexpr virtual IOOpResult
			write( std::initializer_list<std::span<std::byte const>> const & ) = 0;
			[[nodiscard]] constexpr virtual IOOpResult put( std::byte ) = 0;
			[[nodiscard]] constexpr virtual IOOpResult put( char ) = 0;
		};

		namespace {
			template<typename T>
			class WriteProxyImplChild final : public WriteProxyImpl {
				T &writer;

			public:
				[[nodiscard]] constexpr daw::unique_ptr<WriteProxyImpl>
				clone( ) const final;

				constexpr WriteProxyImplChild( T &writable_value ) noexcept
				  : writer( writable_value ) {}

				[[nodiscard]] constexpr IOOpResult write( daw::string_view sv ) final {
					return WritableOutput<T>::write( writer, sv );
				}

				[[nodiscard]] constexpr IOOpResult
				write( std::initializer_list<daw::string_view> const &svs ) final {
					return WritableOutput<T>::write( writer, svs );
				}

				[[nodiscard]] constexpr IOOpResult
				write( std::span<std::byte const> sp ) final {
					return WritableOutput<T>::write( writer, sp );
				}

				[[nodiscard]] constexpr IOOpResult write(
				  std::initializer_list<std::span<std::byte const>> const &sps ) final {
					return WritableOutput<T>::write( writer, sps );
				}

				[[nodiscard]] constexpr IOOpResult put( std::byte b ) override {
					return WritableOutput<T>::put( writer, b );
				}

				[[nodiscard]] constexpr IOOpResult put( char c ) override {
					return WritableOutput<T>::put( writer, c );
				}
			};

			template<typename T>
			constexpr daw::unique_ptr<WriteProxyImpl>
			WriteProxyImplChild<T>::clone( ) const {
				return daw::make_unique<WriteProxyImplChild<T>>( writer );
			}
		} // namespace
	}   // namespace io_details
	class WriteProxy {
		daw::unique_ptr<io_details::WriteProxyImpl> writer;

	public:
		template<typename T>
		explicit constexpr WriteProxy( T &writable_value )
		  : writer( daw::make_unique<io_details::WriteProxyImplChild<T>>(
		      writable_value ) ) {}

		constexpr WriteProxy( WriteProxy const &other )
		  : writer( other.writer ? other.writer->clone( ) : nullptr ) {}

		constexpr WriteProxy &operator=( WriteProxy const &rhs ) {
			if( not rhs.writer ) {
				writer.reset( );
			} else {
				writer = rhs.writer->clone( );
			}
			return *this;
		}

		constexpr WriteProxy( WriteProxy && ) noexcept = default;
		constexpr WriteProxy &operator=( WriteProxy && ) noexcept = default;
		constexpr WriteProxy( ) = default;

		[[nodiscard]] constexpr IOOpResult write( daw::string_view sv ) {
			assert( writer );
			return writer->write( sv );
		}

		[[nodiscard]] constexpr IOOpResult
		write( std::initializer_list<daw::string_view> const &svs ) {
			assert( writer );
			return writer->write( svs );
		}

		[[nodiscard]] constexpr IOOpResult write( std::span<std::byte const> sp ) {
			assert( writer );
			return writer->write( sp );
		}

		[[nodiscard]] constexpr IOOpResult
		write( std::initializer_list<std::span<std::byte const>> const &sps ) {
			assert( writer );
			return writer->write( sps );
		}

		template<typename Byte>
		[[nodiscard]] constexpr IOOpResult put( Byte b ) {
			static_asssert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			assert( writer );
			return writer->put( b );
		}
	};

	template<>
	class Writer<WriteProxy> {
		WriteProxy writer;

	public:
		DAW_ATTRIB_INLINE constexpr Writer( WriteProxy &&wp ) noexcept
		  : writer( std::move( wp ) ) {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult
		write( daw::string_view sv ) {
			return writer.write( sv );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult
		write( std::initializer_list<daw::string_view> const &svs ) {
			return writer.write( svs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult
		write( std::span<std::byte const> sp ) {
			return writer.write( sp );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult
		write( std::initializer_list<std::span<std::byte const>> const &sps ) {
			return writer.write( sps );
		}

		template<typename Byte>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr IOOpResult put( Byte b ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			return writer.put( b );
		}
	};

} // namespace daw::io
