#!/bin/sh
set -x
autoheader -l headers
aclocal
automake -a
autoconf
