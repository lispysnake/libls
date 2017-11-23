# libuf

General purpose C library. I have a number of C projects in transit that are
starting to gain much commonality, so it is high time they all got bundled
into one place.

Long story short, the use case is "I need to build a C thing, but glib2 is too
heavy". Invariably this is a very low level component or long running daemon
process, or indeed something that execve's after bootstrap.

Primarily this is arising out of my Valgrind OCD and as a spiritual successor
to the much older [libnica](https://github.com/intel/libnica).

The focus will be C11 and portability, and ensuring we're sane to use outside
of glibc, i.e. with musl for some of the static binary projects within Solus.

Several shortcomings will be address, namely having a sane ABI for dynamic
linking, as well as performance and compliance (not-insane INI parser..)

This will hopefully contain the last INI parser I ever have to write, too, so
it can be repurposed into every single project using some custom hacky parsers
or copy thereof..

Not a lot to look at right now, check back later.

### Authors

Copyright © 2017 Ikey Doherty

libuf is available under the terms of the LGPL-2.1 license.
