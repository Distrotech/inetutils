#!/bin/sh
set -x
autoheader -l header
aclocal
automake -a
autoconf
