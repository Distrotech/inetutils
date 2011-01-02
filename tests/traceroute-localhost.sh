#!/bin/sh

# Copyright (C) 2009, 2010, 2011 Free Software Foundation, Inc.
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

TRACEROUTE=${TRACEROUTE:-../src/traceroute$EXEEXT}
TARGET=${TARGET:-127.0.0.1}

if [ $VERBOSE ]; then
    set -x
    $TRACEROUTE --version
fi

if [ `id -u` != 0 ]; then
    echo "traceroute needs to run as root"
    exit 77
fi

errno=0
errno2=0

$TRACEROUTE --type=udp $TARGET || errno=$?
test $errno -eq 0 || echo "Failed at UDP tracing." >&2

$TRACEROUTE --type=icmp $TARGET || errno2=$?
test $errno2 -eq 0 || echo "Failed at ICMP tracing." >&2

test $errno -eq 0 || exit $errno

exit $errno2
