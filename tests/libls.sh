#!/bin/sh

# Copyright (C) 2014 Free Software Foundation, Inc.
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

# Check response in libls.
# Very simple testing, aiming mostly at code coverage.

set -u

. ./tools.sh

: ${EXEEXT:=}
silence=
bucket=

# Executable under test.
#
LS=${LS:-./ls$EXEEXT}

if test ! -x "$LS"; then
    echo >&2 "Missing executable '$LS'.  Skipping test."
    exit 77
fi

if test -z "${VERBOSE+set}"; then
    silence=:
    bucket='>/dev/null'
fi

# Several runs with different switches are compared by
# a simple count of printed lines.
#
REPLY_a1=`$LS -a1 | $SED -n '$='`
REPLY_A1=`$LS -A1 | $SED -n '$='`

REPLY_C=`$LS -C`
REPLY_Cf=`$LS -Cf`
REPLY_Cr=`$LS -Cr`
REPLY_Ct=`$LS -Ct`
REPLY_x=`$LS -x`
REPLY_m=`$LS -m`

REPLY_l=`$LS -l`
REPLY_lT=`$LS -l`
REPLY_n=`$LS -n`

# In an attempt to counteract lack of subsecond accuracy,
# probe the parent directory where timing is known to be more
# varied, than in the subdirectory "tests/".
#
REPLY_Ccts=`$LS -Ccts ..`
REPLY_Cuts=`$LS -Cuts ..`

errno=0

test $REPLY_a1 -eq `expr $REPLY_A1 + 2` ||
  { errno=1; echo >&2 'Failed to suppress "." and "..".'; }

test x"$REPLY_C" != x"$REPLY_Cf" ||
  { errno=1; echo >&2 'Failed to disable sorting with "-f".'; }

test x"$REPLY_C" != x"$REPLY_Cr" ||
  { errno=1; echo >&2 'Failed to reverse sorting with "-r".'; }

test x"$REPLY_C" != x"$REPLY_Ct" ||
  { errno=1; echo >&2 'Failed to sort on modification with "-t".'; }

test x"$REPLY_C" != x"$REPLY_x" ||
  { errno=1; echo >&2 'Failed to distinguish "-C" from "-x".'; }

test x"$REPLY_C" != x"$REPLY_m" ||
  { errno=1; echo >&2 'Failed to distinguish "-C" from "-m".'; }

test x"$REPLY_l" != x"$REPLY_n" ||
  { errno=1; echo >&2 'Failed to distinguish "-l" from "-n".'; }

test x"$REPLY_Ccts" != x"$REPLY_Cuts" ||
  { errno=1; echo >&2 'Failed to distinguish "-u" from "-c".'; }

test $errno -ne 0 || $silence echo "Successful testing".

exit $errno
