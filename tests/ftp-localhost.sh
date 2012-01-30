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

# FIXME: Better test coverage!
#
# Implemented: anonymous-only in inetd-mode.
#
# Wanted:  * standalone-mode
#          * underprivileged mode.

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

# Portability fix for SVR4
PWD="${PWD:-`pwd`}"

# Acting user and target user
#
USER="`id -u -n`"
FTPUSER=${FTPUSER:-ftp}

if [ ! -x $FTP ]; then
    echo "No FTP client '$FTP' present.  Skipping test" >&2
    exit 77
elif [ ! -x $FTPD ]; then
    echo "No FTP server '$FTPD' present.  Skipping test" >&2
    exit 77
elif [ ! -x $INETD ]; then
    echo "No inetd superserver '$INETD' present.  Skipping test" >&2
    exit 77
fi

if [ $VERBOSE ]; then
    set -x
    $FTP --version | head -1
    $FTPD --version | head -1
    $INETD --version | head -1
fi

if [ `id -u` != 0 ]; then
    echo "ftpd needs to run as root" >&2
    exit 77
fi

if id -u "$FTPUSER" > /dev/null; then
    :
else
    echo "anonymous ftpd needs a '$FTPUSER' user" >&2
    exit 77
fi

FTPHOME="`eval echo ~"$FTPUSER"`"
if test ! -d "$FTPHOME"; then
    save_IFS="$IFS"
    IFS=:
    set -- `grep "^$FTPUSER:" /etc/passwd`	# Existence is known above.
    IFS="$save_IFS"
    if test ! -d "$6"; then
	echo "The user '$FTPUSER' must have a home directory." >&2
	exit 77
    fi
    FTPHOME="$6"
fi

if test -d "$FTPHOME" && test -r "$FTPHOME" && test -x "$FTPHOME"; then
    :	# We have full access to anonymous' home directory.
else
    echo "Insufficient access for $FTPUSER's home directory." >&2
    exit 77
fi

# Note that inetd changes directory to / when --debug is not given so
# all paths must be absolute for things to work.

TMPDIR=`mktemp -d $PWD/tmp.XXXXXXXXXX`

posttesting () {
    test -f "$TMPDIR/inetd.pid" && test -r "$TMPDIR/inetd.pid" \
	&& kill -9 "`cat $TMPDIR/inetd.pid`"
    rm -rf "$TMPDIR"
}

trap posttesting 0 1 2 3 15

# locate_port  port
#
# Test for IPv4 as well as for IPv6.
locate_port () {
    if [ "`uname -s`" = "SunOS" ]; then
	netstat -na -finet -finet6 -Ptcp |
	grep "\.$1[^0-9]" >/dev/null 2>&1
    else
	netstat -na |
	grep "^$2\(4\|6\|46\)\{0,1\}.*[^0-9]$1[^0-9]" >/dev/null 2>&1
    fi
}

# Find an available port number.  There will be some
# room left for a race condition, but we try to be
# flexible enough for running copies of this script.
#
if test -z "$PORT"; then
    for PORT in 4711 4713 4717 4725 4741 4773 none; do
	test $PORT = none && break
	if locate_port $PORT; then
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

cat <<EOT > "$TMPDIR/inetd.conf"
$PORT stream tcp4 nowait $USER $PWD/$FTPD ftpd -A -l
$PORT stream tcp6 nowait $USER $PWD/$FTPD ftpd -A -l
EOT

cat <<EOT > "$TMPDIR/.netrc"
machine $TARGET login $FTPUSER password foobar
machine $TARGET6 login $FTPUSER password foobar
machine $TARGET46 login $FTPUSER password foobar
EOT

chmod 600 $TMPDIR/.netrc

$INETD --pidfile="$TMPDIR/inetd.pid" "$TMPDIR/inetd.conf"

# Wait for inetd to write pid and open socket
sleep 2

# Test evaluation helper
#
# test_report  errno output_file hint_msg
#
test_report () {
    test -z "${VERBOSE+yes}" || cat "$2"

    if [ $1 != 0 ]; then
	echo "Running '$FTP' failed with errno $1." >&2
	exit 77
    fi

    # Did we get access?
    if grep 'Login failed' "$2" >/dev/null 2>&1; then
	echo "Failed login for access using '$3' FTP client." >&2
	exit 1
    fi

    # Standing control connection?
    if grep 'FTP server status' "$2" >/dev/null 2>&1; then
	:
    else
	echo "Cannot find server status for '$3' FTP client?" >&2
	exit 1
    fi

    # Was data transfer successful?
    if grep '226 Transfer complete.' "$2" >/dev/null 2>&1; then
	:
    else
	echo "Cannot find transfer result for '$3' FTP client?" >&2
	exit 1
    fi
}

# Test a passive connection: PASV and IPv4.
#
echo "PASV to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP "$TARGET" $PORT -4 -v -p -t >$TMPDIR/ftp.stdout 2>&1

test_report $? "$TMPDIR/ftp.stdout" "PASV/$TARGET"

# Test an active connection: PORT and IPv4.
#
echo "PORT to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP "$TARGET" $PORT -4 -v -t >$TMPDIR/ftp.stdout 2>&1

test_report $? "$TMPDIR/ftp.stdout" "PORT/$TARGET"

# Test a passive connection: EPSV and IPv4.
#
echo "EPSV to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
epsv4
dir
STOP
HOME=$TMPDIR $FTP "$TARGET" $PORT -4 -v -p -t >$TMPDIR/ftp.stdout 2>&1

test_report $? "$TMPDIR/ftp.stdout" "EPSV/$TARGET"

# Test an active connection: EPRT and IPv4.
#
echo "EPRT to $TARGET (IPv4) using inetd."
cat <<STOP |
rstatus
epsv4
dir
STOP
HOME=$TMPDIR $FTP "$TARGET" $PORT -4 -v -t >$TMPDIR/ftp.stdout 2>&1

test_report $? "$TMPDIR/ftp.stdout" "EPRT/$TARGET"

# Test a passive connection: EPSV and IPv6.
#
echo "EPSV to $TARGET6 (IPv6) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP "$TARGET6" $PORT -6 -v -p -t >$TMPDIR/ftp.stdout 2>&1

test_report $? "$TMPDIR/ftp.stdout" "EPSV/$TARGET6"

# Test an active connection: EPRT and IPv6.
#
echo "EPRT to $TARGET6 (IPv6) using inetd."
cat <<STOP |
rstatus
dir
STOP
HOME=$TMPDIR $FTP "$TARGET6" $PORT -6 -v -t >$TMPDIR/ftp.stdout 2>&1

test_report $? "$TMPDIR/ftp.stdout" "EPRT/$TARGET6"

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

if $have_address_mapping; then
    :
else
    # Do we have sysctl(1) available?
    if which sysctl >/dev/null 2>&1; then
	have_sysctl=true
    else
	echo "Warning: Not testing IPv4-mapped addresses." >&2
    fi
fi

if $have_address_mapping; then
    :
elif $have_sysctl; then
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
	    echo "Warning: Address mapping IPv4-to-Ipv6 is disabled." >&2
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
    HOME=$TMPDIR $FTP "$TARGET46" $PORT -6 -v -p -t >$TMPDIR/ftp.stdout 2>&1

    test_report $? "$TMPDIR/ftp.stdout" "EPSV/$TARGET46"

    # Test an active connection: EPRT and IPvIPv6.
    #
    echo "EPRT to $TARGET46 (IPv4-as-IPv6) using inetd."
    cat <<-STOP |
	rstatus
	dir
	STOP
    HOME=$TMPDIR $FTP "$TARGET46" $PORT -6 -v -t >$TMPDIR/ftp.stdout 2>&1

    test_report $? "$TMPDIR/ftp.stdout" "EPRT/$TARGET46"
else
    # The IPv4-as-IPv6 tests were not performed.
    echo 'Skipping two tests of IPv4 mapped as IPv6.'
fi

exit 0
