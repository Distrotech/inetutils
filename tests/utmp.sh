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

# Test to establish functionality of read_utmp() from Gnulib.
# The functionality is needed by our services syslog and talkd.
#

. ./tools.sh

READUTMP=${READUTMP:-./readutmp$EXEEXT}

if test ! -x $READUTMP; then
    echo >&2 "No executable '$READUTMP' present.  Skipping test."
    exit 77
fi

if test -n "$VERBOSE"; then
    set -x
fi

# Execute READUTMP once without argument, and once with
# ourself as argument.  Both should succeed, but if either
# fails report only a skipped test, giving some complimentary
# explanations.

errno=0

$READUTMP || errno=77

$READUTMP `func_id_user` || errno=77

if test $errno -ne 0; then
    cat >&2 <<-EOT
	NOTICE: read_utmp() from Gnulib just failed.  This can be
	due to running this test in sudo mode, or using a role.
	Should this script fail while running it as a logged in
	user, then read_utmp() is indeed broken on this system.
	Then the present syslog and talkd are not fully functional.
	You are then welcome to report this failure.
	EOT
fi

exit $errno
