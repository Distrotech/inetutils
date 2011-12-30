#!/bin/sh

# Copyright (C) 2010, 2011 Free Software Foundation, Inc.
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

# FIXME: Presently tailored for IPv4.

if [ "$VERBOSE" ]; then
    set -x
fi

TFTP="${TFTP:-../src/tftp$EXEEXT}"
TFTPD="${TFTPD:-$PWD/../src/tftpd$EXEEXT}"
INETD="${INETD:-../src/inetd$EXEEXT}"
IFCONFIG="${IFCONFIG:-../ifconfig/ifconfig$EXEEXT --format=unix}"

AF=${AF:-inet}
PROTO=${PROTO:-udp}
PORT=${PORT:-7777}
USER=${USER:-$LOGNAME}

# Random base diractory at testing time.
TMPDIR=`mktemp -d $PWD/tmp.XXXXXXXXXX`
INETD_CONF="$TMPDIR/inetd.conf.tmp"
INETD_PID="$TMPDIR/inetd.pid.$$"

posttesting () {
    if test -f "$INETD_PID" && test -r "$INETD_PID" \
	&& ps "$(cat $INETD_PID)" >/dev/null 2>&1
    then
	kill -9 "$(cat $INETD_PID)"
    fi
    rm -rf "$TMPDIR" $FILELIST
}

trap posttesting EXIT HUP INT QUIT TERM

# Use only "127.0.0.1" as default address list.
# Other configured addresses might be set under
# strict filter policies, thus might block.
#
# Allow a setting "ADDRESSES=sense" to compute the
# available addresses and then to test them all.
ADDRESSES="${ADDRESSES:-127.0.0.1}"

if [ "$ADDRESSES" = "sense" ]; then
    ADDRESSES="`$IFCONFIG | sed -e "/$AF /!d" \
	-e "s/^.*$AF[[:blank:]]\([:.0-9]\{1,\}\)[[:blank:]].*$/\1/g"`"
fi

# Check that netstat works before proceeding.
netstat -na > /dev/null
if [ ! $? -eq 0 ]; then
    echo "netstat: command failed to execute successfully"
    exit 77
fi

# Work around the peculiar output of netstat(1m,solaris).
#
# locate_port family proto port
#
locate_port () {
    if [ "$(uname -s)" = "SunOS" ]; then
	netstat -na -f$1 -P$2 | grep -q -E "\.$3[^0-9]"
    else
	netstat -na | grep -q -E "^$2(4|6|46)?.*[^0-9]$3[^0-9]"
    fi
}

if [ "$VERBOSE" ]; then
    "$TFTP" --version
    "$TFTPD" --version
    "$INETD" --version
fi

# Check that the port is still available
locate_port inet $PROTO $PORT
if test $? -eq 0; then
    echo "Desired port $PORT/$PROTO is already in use."
    exit 77
fi

# Create `inetd.conf'.  Note: We want $TFTPD to be an absolute file
# name because `inetd' chdirs to `/' in daemon mode; ditto for
# $INETD_CONF.
cat > "$INETD_CONF" <<EOF
$PORT dgram ${PROTO}4 wait $USER $TFTPD   tftpd -l $TMPDIR/tftp-test
EOF

# Launch `inetd', assuming it's reachable at all $ADDRESSES.
$INETD -d -p"$INETD_PID" "$INETD_CONF" &
sleep 1
inetd_pid="$(cat $INETD_PID)"

test -z "$VERBOSE" || echo "Launched Inetd as process $inetd_pid." >&2

# Wait somewhat for the service to settle.
sleep 1

# Did `inetd' really succeed in establishing a listener?
locate_port inet $PROTO $PORT
if test $? -ne 0; then
    # No it did not.
    ps "$inetd_pid" >/dev/null 2>&1 && kill "$inetd_pid"
    echo "Failed in starting correct Inetd instance." >&2
    exit 1
fi

if [ -f /dev/urandom ]; then
    input="/dev/urandom"
else
    input="/dev/zero"
fi

rm -fr $TMPDIR/tftp-test tftp-test-file*
mkdir -p $TMPDIR/tftp-test

# It is important to test data of differing sizes.
#
# Input format:
#
#  name  block-size  count

FILEDATA="file-small 320 1
file-medium 320 2
tftp-test-file 1024 170
"

echo "$FILEDATA" |
while read name bsize count; do
    test -z "$name" && continue
    dd if="$input" of="$TMPDIR/tftp-test/$name" \
	bs=$bsize count=$count 2>/dev/null
done

FILELIST="$(echo "$FILEDATA" | sed 's/[[:space:]].*//' | tr "\n" ' ')"

SUCCESSES=0
EFFORTS=0
RESULT=0

echo "Looking into \"$(echo $ADDRESSES | tr "\n" " ")\"."

for addr in $ADDRESSES; do
    echo "trying with address \`$addr'..." >&2

    for name in $FILELIST; do
	EFFORTS=$((EFFORTS + 1))
	rm -f $name
	echo "get $name" | "$TFTP" $addr $PORT

	cmp "$TMPDIR/tftp-test/$name" "$name"
	result=$?

	if [ "$result" -ne 0 ]; then
	    # Failure.
	    test -z "$VERBOSE" || echo "Failed comparison for $addr/$name." >&2
	    RESULT=$result
	else
	    SUCCESSES=$((SUCCESSES + 1))
	    test -z "$VERBOSE" || echo "Successful comparison for $addr/$name." >&2
	fi
   done
done

# Minimal clean up. Main work in posttesting().
echo
echo Tests in $0 had $SUCCESSES successes out of $EFFORTS cases.

exit $RESULT
