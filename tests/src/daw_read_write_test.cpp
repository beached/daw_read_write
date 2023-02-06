// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#include <daw/io/daw_read_write.h>
#include <daw/io/daw_read_write_fd.h>
#include <daw/io/daw_type_writers.h>
#include <daw/io/daw_write_stream.h>

#include <iostream>
#include <string>

int main( int, char **argv ) {
	{
		auto wp = daw::io::WriteProxy( std::cout );
		auto wsb = daw::io::write_streambuf( wp );
		(void)wp.write( "ostream start\n" );
		auto wo = daw::io::write_ostream( wp );
		wo.write( "Hello\n", 6 );
		wo << "Hello World  " << 5555 << " WHAT!\n\n";
		(void)wp.write( "ostream done\n" );
	}
	auto s = std::string( );
	auto p = daw::io::WriteProxy( s );
	if( p.write( argv[0] ).status != daw::io::IOOpStatus::Ok ) {
		std::terminate( );
	}
	p = daw::io::WriteProxy( std::cout );
	if( p.write( { "Proxy\n", s, "\n" } ).status != daw::io::IOOpStatus::Ok ) {
		std::terminate( );
	}
	auto sv = daw::string_view( s );
	auto rp = daw::io::ReadProxy( sv );
	auto sv2 = daw::string_view( s );
	auto rpr = daw::io::PeekableReader( daw::io::Reader( sv2 ) );
	auto peek_result = rpr.peek( 5 );
	std::cout << "Peek Result\n"
	          << std::string_view(
	               reinterpret_cast<char const *>( peek_result.buffer.data( ) ),
	               peek_result.buffer.size( ) )
	          << '\n';
	char buff[1024]{ };
	auto sp = std::span( buff );
	auto result = rp.read( sp );
	if( result.count != s.size( ) ) {
		std::terminate( );
	}
	std::cout << "Readable Proxy\n";
	std::cout << buff << '\n';
	std::memset( buff, 0, 1024 );
	std::cout << "Reader\n";
	sp = std::span( buff );
	sv = s;
	auto rr = daw::io::Reader( sv );
	result = rp.read( sp );
	(void)result;
	if( result.count != s.size( ) ) {
		std::terminate( );
	}
	std::cout << buff << '\n';
	memset( buff, 0, 1024 );
	auto bspan = std::span<char>( buff, 1024 );
	auto w = daw::io::Writer( bspan );
	auto sz = w.write( { "Hello", " ", "World", "\n" } );
	(void)sz;
	auto fd = daw::io::fd_wrap_t( STDOUT_FILENO );

	auto fdw = daw::io::WriteProxy( fd );
	(void)fdw.write( std::string_view( buff ) );
	auto *bptr = buff;
	auto b = daw::io::WriteProxy( bptr );
	memset( buff, 0, 1024 );
	(void)b.write( "This is a test\n" );
	(void)fdw.write( std::string_view( buff ) );
	daw::io::type_writer::type_writer( fdw, 5555 );
	daw::io::type_writer::type_writer( fdw, 3333U );
}
