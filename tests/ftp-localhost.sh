#!/bin/sh

# Copyright (C) 2011 Free Software Foundation, Inc.
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

# Written by Simon Josefsson

# FIXME: Strict IPv4 setup, until Mats has completed the migration
# of ftpd to support IPv6.  Address mapping IPv4-to-IPv6 is not
# uniform an all platforms, thus `tcp4' for inetd and '-4' for ftp.

set -e

FTP=${FTP:-../ftp/ftp$EXEEXT}
FTPD=${FTPD:-../ftpd/ftpd$EXEEXT}
INETD=${INETD:-../src/inetd$EXEEXT}
TARGET=${TARGET:-127.0.0.1}

if [ $VERBOSE ]; then
    set -x
    $FTP --version | head -1
    $FTPD --version | head -1
    $INETD --version | head -1
fi

if [ `id -u` != 0 ]; then
    echo "ftpd needs to run as root"
    exit 77
fi

if ! id -u ftp > /dev/null; then
    echo "anonymous ftpd needs a 'ftp' user"
    exit 77
fi

if [ ! -d ~ftp ]; then
    echo "the user 'ftp' must have a home directory"
    exit 77
fi

# Note that inetd changes directory to / when --debug is not given so
# all paths must be absolute for things to work.

TMPDIR=`mktemp -d $PWD/tmp.XXXXXXXXXX`

posttesting () {
    test -f "$TMPDIR/inetd.pid" && test -r "$TMPDIR/inetd.pid" \
	&& kill -9 "$(cat $TMPDIR/inetd.pid)"
    rm -rf "$TMPDIR"
}

trap posttesting 0 1 2 3 15

echo "4711 stream tcp4 nowait root $PWD/$FTPD ftpd -A -l" > $TMPDIR/inetd.conf
echo "machine $TARGET login ftp password foobar" > $TMPDIR/.netrc
chmod 600 $TMPDIR/.netrc

$INETD --pidfile=$TMPDIR/inetd.pid $TMPDIR/inetd.conf

# Wait for inetd to write pid and open socket
sleep 2

cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP $TARGET 4711 -4 -v -p -t >$TMPDIR/ftp.stdout

errno=$?
cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output from ftp client?
    exit 1
fi

exit 0
