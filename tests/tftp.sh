#!/bin/sh

# Copyright (C) 2010 Free Software Foundation, Inc.
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

TFTP="${TFTP:-../src/tftp$EXEEXT}"
TFTPD="${TFTPD:-$PWD/../src/tftpd$EXEEXT}"
INETD="${INETD:-../src/inetd$EXEEXT}"
IFCONFIG="${IFCONFIG:-../ifconfig/ifconfig$EXEEXT}"

PORT=7777
INETD_CONF="$PWD/inetd.conf.tmp"

ADDRESSES="`$IFCONFIG | grep 'inet addr:' | \
  sed -e's/inet addr:\([^ ]\+\)[[:blank:]].*$/\1/g'`"

if [ "$VERBOSE" ]; then
    set -x
    "$TFTP" --version
    "$TFTPD" --version
    "$INETD" --version
fi

# Create `inetd.conf'.  Note: We want $TFTPD to be an absolute file
# name because `inetd' chdirs to `/' in daemon mode; ditto for
# $INETD_CONF.
cat > "$INETD_CONF" <<EOF
$PORT dgram udp wait $USER $TFTPD   tftpd -l `pwd`/tftp-test
EOF

# Launch `inetd', assuming it's reachable at all $ADDRESSES.
$INETD "${VERBOSE:+-d}" "$INETD_CONF" &
inetd_pid="$!"

if [ -f /dev/urandom ]; then
    input="/dev/urandom"
else
    input="/dev/zero"
fi

rm -fr tftp-test tftp-test-file
mkdir tftp-test && \
    dd if="$input" of="tftp-test/tftp-test-file" bs=1024 count=170

for addr in $ADDRESSES; do
    echo "trying with address \`$addr'..." >&2

    rm -f tftp-test-file
    echo "get tftp-test-file" | "$TFTP" $addr $PORT

    cmp tftp-test/tftp-test-file tftp-test-file
    result=$?

    if [ "$result" -ne 0 ]; then
	# Failure.
	break
    fi
done

kill "$inetd_pid"

rm -rf tftp-test tftp-test-file "$INETD_CONF"

exit $result
