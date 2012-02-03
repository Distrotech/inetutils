#!/bin/sh

# Copyright (C) 2010, 2011, 2012 Free Software Foundation, Inc.
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

# Run `inetd' with `tftpd' and try to fetch a file from there using `tftp'.

# Prerequisites:
#
#  * Shell: SVR4 Bourne shell, or newer.
#
#  * cat(1), expr(1), head(1), kill(1), pwd(1), rm(1).
#
#  * cmp(1), dd(1), id(1), grep(1), mktemp(1), netstat(8),
#    ps(1), sed(1), uname(1).

if [ "$VERBOSE" ]; then
    set -x
fi

# Portability fix for SVR4
PWD="${PWD:-`pwd`}"

TFTP="${TFTP:-../src/tftp$EXEEXT}"
TFTPD="${TFTPD:-$PWD/../src/tftpd$EXEEXT}"
INETD="${INETD:-../src/inetd$EXEEXT}"
IFCONFIG="${IFCONFIG:-../ifconfig/ifconfig$EXEEXT --format=unix}"
IFCONFIG_SIMPLE=`expr X"$IFCONFIG" : X'\([^ ]*\)'`	# Remove options

if [ ! -x $TFTP ]; then
    echo "No TFTP client '$TFTP' present.  Skipping test" >&2
    exit 77
elif [ ! -x $TFTPD ]; then
    echo "No TFTP server '$TFTPD' present.  Skipping test" >&2
    exit 77
elif [ ! -x $INETD ]; then
    echo "No inetd superserver '$INETD' present.  Skipping test" >&2
    exit 77
elif [ ! -x $IFCONFIG_SIMPLE ]; then	# Remove options
    echo "No ifconfig '$IFCONFIG_SIMPLE' present.  Skipping test" >&2
    exit 77
fi

# Check that netstat works before proceeding.
netstat -na > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "netstat: command failed to execute successfully" >&2
    exit 77
fi

# And grep!
echo 'Good luck.' | grep 'ood' > /dev/null 2>&1 \
    || {
	echo 'grep(1) is not available.  Skipping test.' >&2
	exit 77
    }

AF=${AF:-inet}
PROTO=${PROTO:-udp}
USER=`id -u -n`

# Random base directory at testing time.
TMPDIR=`mktemp -d $PWD/tmp.XXXXXXXXXX` ||
    {
	echo 'Failed at creating test directory.  Aborting.' >&2
	exit 1
    }

INETD_CONF="$TMPDIR/inetd.conf.tmp"
INETD_PID="$TMPDIR/inetd.pid.$$"

posttesting () {
    if test -n "$TMPDIR" && test -f "$INETD_PID" \
	&& test -r "$INETD_PID" \
	&& ps "`cat $INETD_PID`" >/dev/null 2>&1
    then
	kill -9 "`cat $INETD_PID`" 2>/dev/null
    fi
    test -n "$TMPDIR" && test -d "$TMPDIR" \
	&& rm -rf "$TMPDIR" $FILELIST
}

trap posttesting EXIT HUP INT QUIT TERM

# Use only "127.0.0.1 ::1" as default address list.
# Other configured addresses might be set under
# strict filter policies, thus might block.
#
# Allow a setting "ADDRESSES=sense" to compute the
# available addresses and then to test them all.
ADDRESSES="${ADDRESSES:-127.0.0.1 ::1}"

if [ "$ADDRESSES" = "sense" ]; then
    ADDRESSES="`$IFCONFIG | sed -e "/$AF /!d" \
	-e "s/^.*$AF \([:.0-9]\{1,\}\) .*$/\1/g"`"
fi

# Work around the peculiar output of netstat(1m,solaris).
#
# locate_port proto port
#
locate_port () {
    if [ "`uname -s`" = "SunOS" ]; then
	netstat -na -finet -finet6 -P$1 |
	grep "\.$2[^0-9]" >/dev/null 2>&1
    else
	netstat -na |
	grep "^$1\(4\|6\|46\)\{0,1\}.*[^0-9]$2[^0-9]" >/dev/null 2>&1
    fi
}

if [ "$VERBOSE" ]; then
    "$TFTP" --version | head -1
    "$TFTPD" --version | head -1
    "$INETD" --version | head -1
    "$IFCONFIG_SIMPLE" --version | head -1
fi

# Find an available port number.  There will be some
# room left for a race condition, but we try to be
# flexible enough for running copies of this script.
#
if test -z "$PORT"; then
    for PORT in 7777 7779 7783 7791 7807 7839 none; do
	test $PORT = none && break
	if locate_port $PROTO $PORT; then
	    continue
	else
	    break
	fi
    done
    if test "$PORT" = 'none'; then
	echo 'Our port allocation failed.  Skipping test.' >&2
	exit 77
    fi
fi

# Create `inetd.conf'.  Note: We want $TFTPD to be an absolute file
# name because `inetd' chdirs to `/' in daemon mode; ditto for
# $INETD_CONF.  Thus the dependency on file locations will be
# identical in daemon-mode and in debug-mode.
write_conf () {
    cat > "$INETD_CONF" <<-EOF
	$PORT dgram ${PROTO}4 wait $USER $TFTPD   tftpd -l $TMPDIR/tftp-test
	$PORT dgram ${PROTO}6 wait $USER $TFTPD   tftpd -l $TMPDIR/tftp-test
	EOF
}

write_conf ||
    {
	echo 'Could not create configuration file for Inetd.  Aborting.' >&2
	exit 1
    }

# Launch `inetd', assuming it's reachable at all $ADDRESSES.
# Must use '-d' consistently to prevent daemonizing, but we
# would like to suppress the verbose output.  The variable
# REDIRECT is set to '2>/dev/null' in non-verbose mode.
#
test -n "${VERBOSE+yes}" || REDIRECT='2>/dev/null'

eval "$INETD -d -p'$INETD_PID' '$INETD_CONF' $REDIRECT &"

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

# Wait somewhat for the service to settle.
sleep 1

# Did `inetd' really succeed in establishing a listener?
locate_port $PROTO $PORT
if test $? -ne 0; then
    # No it did not.
    ps "$inetd_pid" >/dev/null 2>&1 && kill -9 "$inetd_pid" 2>/dev/null
    rm -f "$INETD_PID"

    echo 'First attempt at starting Inetd has failed.' >&2
    echo 'A new attempt will follow after some delay.' >&2
    echo 'Increasing verbosity for better backtrace.' >&2
    sleep 7

    # Select a new port, with offset and some randomness.
    PORT=`expr $PORT + 137 + \( ${RANDOM:-$$} % 517 \)`
    write_conf ||
	{
	    echo 'Could not create configuration file for Inetd.  Aborting.' >&2
	    exit 1
	}

    $INETD -d -p"$INETD_PID" "$INETD_CONF" &
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

    echo "Launched Inetd as process $inetd_pid." >&2

    if locate_port $PROTO $PORT; then
	: # Successful this time.
    else
	echo "Failed again at starting correct Inetd instance." >&2
	exit 1
    fi
fi

if [ -r /dev/urandom ]; then
    input="/dev/urandom"
else
    input="/dev/zero"
fi

test -d "$TMPDIR" && rm -fr "$TMPDIR/tftp-test" tftp-test-file*
test -d "$TMPDIR" && mkdir -p "$TMPDIR/tftp-test" \
    || {
	echo 'Failed at creating directory for master files.  Aborting.' >&2
	exit 1
    }

# It is important to test data of differing sizes.
#
# Input format:
#
#  name  block-size  count

FILEDATA="file-small 320 1
file-medium 320 2
tftp-test-file 1024 170"

echo "$FILEDATA" |
while read name bsize count; do
    test -z "$name" && continue
    dd if="$input" of="$TMPDIR/tftp-test/$name" \
	bs=$bsize count=$count 2>/dev/null
done

FILELIST="`echo "$FILEDATA" | sed 's/ .*//' | tr "\n" ' '`"

SUCCESSES=0
EFFORTS=0
RESULT=0

echo "Looking into '`echo $ADDRESSES | tr "\n" ' '`'."

for addr in $ADDRESSES; do
    echo "trying address '$addr'..." >&2

    for name in $FILELIST; do
	EFFORTS=`expr $EFFORTS + 1`
	rm -f $name
	echo "get $name" | "$TFTP" ${VERBOSE+-v} "$addr" $PORT

	cmp "$TMPDIR/tftp-test/$name" "$name" 2>/dev/null
	result=$?

	if [ "$result" -ne 0 ]; then
	    # Failure.
	    test -z "$VERBOSE" || echo "Failed comparison for $addr/$name." >&2
	    RESULT=$result
	else
	    SUCCESSES=`expr $SUCCESSES + 1`
	    test -z "$VERBOSE" || echo "Successful comparison for $addr/$name." >&2
	fi
   done
done

# Minimal clean up. Main work in posttesting().
echo
echo Tests in $0 had $SUCCESSES successes out of $EFFORTS cases.

exit $RESULT
