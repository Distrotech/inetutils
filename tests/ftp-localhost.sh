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

# Written by Simon Josefsson

# FIXME: Separate tests of IPv4 and IPv6 using `inetd', no testing
# of standalone daemon yet.

# Address mapping IPv4-to-IPv6 is not uniform an all platforms,
# thus separately using `tcp4' and `tcp6' for streams in `inetd.conf'.
# FIXME: Once functionality attains, reduce code duplication when
# evaluating partial tests.

set -e

FTP=${FTP:-../ftp/ftp$EXEEXT}
FTPD=${FTPD:-../ftpd/ftpd$EXEEXT}
INETD=${INETD:-../src/inetd$EXEEXT}
TARGET=${TARGET:-127.0.0.1}
TARGET6=${TARGET6:-::1}
TARGET46=${TARGET46:-::ffff:127.0.0.1}

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
echo "4711 stream tcp6 nowait root $PWD/$FTPD ftpd -A -l" >> $TMPDIR/inetd.conf
echo "machine $TARGET login ftp password foobar" > $TMPDIR/.netrc
echo "machine $TARGET6 login ftp password foobar" >> $TMPDIR/.netrc
echo "machine $TARGET46 login ftp password foobar" >> $TMPDIR/.netrc
chmod 600 $TMPDIR/.netrc

$INETD --pidfile=$TMPDIR/inetd.pid $TMPDIR/inetd.conf

# Wait for inetd to write pid and open socket
sleep 2

# Test a passive connection: PASV and IPv4.
#
echo "PASV to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP $TARGET 4711 -4 -v -p -t >$TMPDIR/ftp.stdout

errno=$?
[ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

# Standing control connection?
if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output for passive ftp client?
    exit 1
fi

# Was data transfer successful?
if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
    echo cannot find transfer result for passive ftp client?
    exit 1
fi

# Test an active connection: PORT and IPv4.
#
echo "PORT to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP $TARGET 4711 -4 -v -t >$TMPDIR/ftp.stdout

errno=$?
[ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

# Standing control connection?
if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output for active ftp client?
    exit 1
fi

# Was data transfer successful?
if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
    echo cannot find transfer result for active ftp client?
    exit 1
fi

# Test a passive connection: EPSV and IPv4.
#
echo "EPSV to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
epsv4
dir
STOP
HOME=$TMPDIR $FTP $TARGET 4711 -4 -v -p -t >$TMPDIR/ftp.stdout

errno=$?
[ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

# Standing control connection?
if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output for passive ftp client?
    exit 1
fi

# Was data transfer successful?
if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
    echo cannot find transfer result for passive ftp client?
    exit 1
fi

# Test an active connection: EPRT and IPv4.
#
echo "EPRT to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
epsv4
dir
STOP
HOME=$TMPDIR $FTP $TARGET 4711 -4 -v -t >$TMPDIR/ftp.stdout

errno=$?
[ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

# Standing control connection?
if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output for active ftp client?
    exit 1
fi

# Was data transfer successful?
if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
    echo cannot find transfer result for active ftp client?
    exit 1
fi

# Test a passive connection: EPSV and IPv6.
#
echo "EPSV to $TARGET6 (IPv6) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP $TARGET6 4711 -6 -v -p -t >$TMPDIR/ftp.stdout

errno=$?
[ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

# Standing control connection?
if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output for passive ftp client?
    exit 1
fi

# Was data transfer successful?
if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
    echo cannot find transfer result for passive ftp client?
    exit 1
fi

# Test an active connection: EPRT and IPv6.
#
echo "EPRT to $TARGET6 (IPv6) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP $TARGET6 4711 -6 -v -t >$TMPDIR/ftp.stdout

errno=$?
[ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

if [ $errno != 0 ]; then
    echo running ftp failed? errno $errno
    exit 77
fi

# Standing control connection?
if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
    echo cannot find expected output for active ftp client?
    exit 1
fi

# Was data transfer successful?
if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
    echo cannot find transfer result for active ftp client?
    exit 1
fi

# Availability of IPv4-mapped IPv6 addresses.
#
# These are impossible on OpenBSD, so a flexible test
# is implemented using sysctl(1) as tool.

# Helpers tp catch relevant execution path.
have_sysctl=false
have_address_mapping=false

# Known exceptions:
#
# OpenSolaris is known to allow address mapping
test `uname -s` = 'SunOS' && have_address_mapping=true

if ! $have_address_mapping; then
    # Do we have sysctl(1) available?
    if ! which sysctl >/dev/null 2>&1; then
	echo "Warning: Not testing IPv4-mapped addresses."
    else
	have_sysctl=true
    fi
fi

if ! $have_address_mapping && $have_sysctl; then
    # Extract the present setting of
    #
    #    net.ipv6.bindv6only (Linux)
    # or
    #    net.inet6.ip6.v6only (BSD).
    #
    value_v6only=`sysctl -a 2>/dev/null | grep v6only`
    if test -n "$value_v6only"; then
	value_v6only=`echo $value_v6only | sed 's/^.*[=:] *//'`
	if test "$value_v6only" -eq 0; then
	    # This is the good value.  Keep it.
	    have_address_mapping=true
	else
	    echo "Warning: Address mapping IPv4-to-Ipv6 is disabled."
	    # Set a non-zero value for later testing.
	    value_v6only=2
	fi
    else
	# Simulate a non-mapping answer in cases where "v6only" missed.
	value_v6only=2
    fi
fi

# Test functionality of IPv4-mapped IPv6 addresses.
#
if $have_address_mapping && test -n "$TARGET46" ; then
    # Test a passive connection: EPSV and IPv4-mapped-IPv6.
    #
    echo "EPSV to $TARGET46 (IPv4-as-IPv6) using inetd."
    cat <<-STOP |
	rstatus
	dir
	STOP
    HOME=$TMPDIR $FTP $TARGET46 4711 -6 -v -p -t >$TMPDIR/ftp.stdout

    errno=$?
    [ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

    if [ $errno != 0 ]; then
	echo running ftp failed? errno $errno
	exit 77
    fi

    if [ $errno != 0 ]; then
	echo running ftp failed? errno $errno
	exit 77
    fi

    # Standing control connection?
    if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
	echo cannot find expected output for passive ftp client?
	exit 1
    fi

    # Was data transfer successful?
    if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
	echo cannot find transfer result for passive ftp client?
	exit 1
    fi

    # Test an active connection: EPRT and IPvIPv6.
    #
    echo "EPRT to $TARGET46 (IPv4-as-IPv6) using inetd."
    cat <<-STOP |
	rstatus
	dir
	STOP
    HOME=$TMPDIR $FTP $TARGET46 4711 -6 -v -t >$TMPDIR/ftp.stdout

    errno=$?
    [ -z "$VERBOSE" ] || cat $TMPDIR/ftp.stdout

    if [ $errno != 0 ]; then
	echo running ftp failed? errno $errno
	exit 77
    fi

    if [ $errno != 0 ]; then
	echo running ftp failed? errno $errno
	exit 77
    fi

    # Standing control connection?
    if ! grep 'FTP server status' $TMPDIR/ftp.stdout; then
	echo cannot find expected output for active ftp client?
	exit 1
    fi

    # Was data transfer successful?
    if ! grep '226 Transfer complete.' $TMPDIR/ftp.stdout; then
	echo cannot find transfer result for active ftp client?
	exit 1
    fi
else
    # The IPv4-as-IPv6 tests were not performed.
    echo 'Skipping two tests of IPv4 mapped as IPv6.'
fi

exit 0
