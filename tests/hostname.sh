#!/bin/sh

# Copyright (C) 2011, 2012, 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Inetutils.
#
# GNU Inetutils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# GNU Inetutils is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see `http://www.gnu.org/licenses/'.

# Prerequisites:
#
#  * Shell: SVR3 Bourne shell, or newer.
#
#  * id(1), uname(1).

. ./tools.sh

hostname=${hostname:-../src/hostname$EXEEXT}

if [ $VERBOSE ]; then
    set -x
    $hostname --version
fi

errno=0

$hostname || errno=$?
test $errno -eq 0 || echo "Failed to get hostname." >&2
test $errno -eq 0 || exit $errno

test `$hostname` = `uname -n` || errno=$?
test $errno -eq 0 || echo "Failed to get hostname." >&2
test $errno -eq 0 || exit $errno

if [ `func_id_uid` != 0 ]; then
    echo "hostname: skipping tests to set host name"
else
    # Only run this if hostname succeeded...
    if [ $errno -eq 0 ]; then
	$hostname `$hostname` || errno=$?
	test $errno -eq 0 || echo "Failed to set hostname." >&2
	test $errno -eq 0 || exit $errno
    fi
fi

exit $errno
