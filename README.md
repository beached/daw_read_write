# daw_read_write
A generic I/O library that abstracts over read/write

The library is composed of a few parts.  
* The type trait for describing how to write to a type or read from it
* Non-Type erased Reader/Writer types that are constructible from a mapped type
* Type erased ReadProxy/WriteProxy types that allow one to type erase
* A Peekable Reader Type that allows one to Peek ahead

For most things using `#include <daw/io/daw_read_write.h>` is enough.  For file descriptors, one needs to additionally add `#include <daw/io/daw_read_write_fd.h>`
