#!/bin/sh
set -x
aclocal
autoheader -l headers
automake -a
autoconf
