#!/bin/sh

# Copyright (C) 2012 Free Software Foundation, Inc.
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

set -e

DNSDOMAINNAME=${DNSDOMAINNAME:-../src/dnsdomainname$EXEEXT}

if ! test -x $DNSDOMAINNAME; then
    echo "No executable $DNSDOMAINNAME available.  Skipping test."
    exit 77
fi

$DNSDOMAINNAME > /dev/null
rc=$?
if test $rc -ne 0; then
    echo "invoking $DNSDOMAINNAME failed with error code $rc"
    exit 1
fi

$DNSDOMAINNAME --version > /dev/null
rc=$?
if test $rc -ne 0; then
    echo "invoking $DNSDOMAINNAME --version failed with error code $rc"
    exit 1
fi

$DNSDOMAINNAME --help > /dev/null
rc=$?
if test $rc -ne 0; then
    echo "invoking $DNSDOMAINNAME --help failed with error code $rc"
    exit 1
fi

exit 0
