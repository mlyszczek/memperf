About
=====

Application to perform memory benchmark.

Dependencies
============

Application is written in ANSI C and that is all you need to compile and run
this application. Just note, that you will be stuck only with *clock\_t* timer,
which may have reduced resolution. If automake detects *clock\_gettime*
function, additional clock *CLOCK\_REALTIME* will be available which has at
last as good resolution as *clock\_t*

Compiling and Installing
========================

memperf uses automake, so compilation is pretty standard

~~~{.sh}
autoreconf -i
./configure
make
make install
~~~

tests can be run with

~~~{.sh}
make check
~~~

License
=======

Library is licensed under BSD 2-clause license. See LICENSE file for details.

Contact
=======

Michał Łyszczek <michal.lyszczek@bofc.pl>

Thanks to
=========

Myself, for creating mtest (https://github.com/mlyszczek/mtest) - a small and
nice framework for unit testing in C.
