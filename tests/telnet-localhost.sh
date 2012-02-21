#!/bin/sh

# Copyright (C) 2011, 2012 Free Software Foundation, Inc.
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

# Test the TELNET client as connection vehicle!
#
# Written by Mats Erik Andersson.

# Prerequisites:
#
#  * Shell: SVR4 Bourne shell, or newer.
#
#  * cat(1), expr(1), head(1), kill(1), pwd(1), rm(1).
#
#  * id(1), grep(1), mktemp(1), ps(1), tty(1).

# Is usage explanation in demand?
#
if test "$1" = "-h" || test "$1" = "--help" || test "$1" = "--usage"; then
    cat <<HERE
Minimal test for TELNET client.

The following environment variables are used:

VERBOSE		Be verbose, if set.
TARGET4		Receiving IPv4 address.
TARGET6		Receiving IPv6 address.

HERE
    exit 0
fi

# The executables under test.
#
INETD=../src/inetd$EXEEXT
TELNET=../telnet/telnet$EXEEXT
ADDRPEEK=./addrpeek$EXEEXT

# Selected targets.
TARGET4=${TARGET4:-127.0.0.1}
TARGET6=${TARGET6:-::1}
TARGET46="::ffff:$TARGET4"

# Step into `tests/', should the invokation
# have been made outside of it.
#
test -d src && test -f tests/telnet-localhost.sh && cd tests/

if test -n "$VERBOSE"; then
    set -x
    $INETD --version | head -1
    $TELNET --version | head -1
fi

# The use of telnet is portable only with a connected TTY.
if tty >/dev/null; then
    :
else
    echo 'No TTY assigned to this process.  Skipping test.' >&2
    exit 77
fi

if test ! -x $INETD; then
    echo "Missing executable '$INETD'.  Skipping test." >&2
    exit 77
fi

if test ! -x $TELNET; then
    echo "Missing executable '$TELNET'.  Skipping test." >&2
    exit 77
fi

if test ! -x $ADDRPEEK; then
    echo "Missing executable '$ADDRPEEK'.  Skipping test." >&2
    exit 77
fi

# Portability fix for SVR4
PWD="${PWD:-`pwd`}"

# For file creation below IU_TESTDIR.
umask 0077

USER=`id -u -n`

# Random base directory at testing time.
TMPDIR=`mktemp -d $PWD/tmp.XXXXXXXXXX` ||
    {
	echo 'Failed at creating test directory.  Aborting.' >&2
	exit 1
    }

INETD_CONF="$TMPDIR/inetd.conf"
INETD_PID="$TMPDIR/inetd.pid.$$"

posttesting () {
    if test -n "$TMPDIR" && test -f "$INETD_PID" \
	&& test -r "$INETD_PID" \
	&& ps "`cat $INETD_PID`" >/dev/null 2>&1
    then
	kill "`cat $INETD_PID`" >/dev/null 2>&1 ||
	kill -9 "`cat $INETD_PID`" >/dev/null 2>&1
    fi
    test -n "$TMPDIR" && test -d "$TMPDIR" \
	&& rm -rf "$TMPDIR" $FILELIST
}

trap posttesting EXIT HUP INT QUIT TERM

PORT=`expr 4973 + ${RANDOM:-$$} % 973`

cat > "$INETD_CONF" <<-EOF ||
	$PORT stream tcp4 nowait $USER $ADDRPEEK addrpeek addr
	$PORT stream tcp6 nowait $USER $ADDRPEEK addrpeek addr
EOF
    {
	echo 'Could not create configuration file for Inetd.  Aborting.' >&2
	exit 1
    }

# Must use '-d' consistently to prevent daemonizing, but we
# would like to suppress the verbose output.
#
# In non-verbose mode the variables `display' and `display_err'
# redirect output streams to `/dev/null'.

test -n "${VERBOSE+yes}" || display='>/dev/null'
test -n "${VERBOSE+yes}" || display_err='2>/dev/null'


eval "$INETD -d -p'$INETD_PID' '$INETD_CONF' $display_err &"

# Debug mode allows the shell to recover PID of Inetd.
spawned_pid=$!

# Wait somewhat for the service to settle.
sleep 2

inetd_pid="`cat $INETD_PID 2>/dev/null`" ||
    {
	cat <<-EOT >&2
		Inetd did not create a PID-file.  Aborting test,
		but loosing control whether an Inetd process is
		still around.
	EOT
	exit 1
    }

test -z "$VERBOSE" || echo "Launched Inetd as process $inetd_pid." >&2

telnet_opts="--no-rc --no-escape --no-login"

errno=0

if test -n "$TARGET4"; then
    output=`$TELNET $telnet_opts $TARGET4 $PORT 2>/dev/null`
    echo "$output" | eval "grep 'Your address is $TARGET4.' $display"
    if test $? -ne 0; then
	errno=1
	echo "Failed at '$TARGET4'." >&2
    fi
fi

if test -n "$TARGET6"; then
    output=`$TELNET $telnet_opts $TARGET6 $PORT 2>/dev/null`
    echo "$output" | eval "grep 'Your address is $TARGET6.' $display"
    if test $? -ne 0; then
	errno=1
	echo "Failed at '$TARGET6'." >&2
    fi
fi

if test -n "$TARGET46"; then
    output=`$TELNET $telnet_opts $TARGET46 $PORT 2>/dev/null`
    echo "$output" | eval "grep 'Your address is .*$TARGET4.' $display"
    if test $? -ne 0; then
	echo "Informational: Unsuccessful with mapped address '$TARGET46'." >&2
    fi
fi

exit $errno
