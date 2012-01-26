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

# Tests to establish functionality of SYSLOG daemon.
#
# Written by Mats Erik Andersson.


# Is usage explanation in demand?
#
if test "$1" = "-h" || test "$1" = "--help" || test "$1" = "--usage"; then
    cat <<HERE
Test utility for syslogd and logger.

The following environment variables are used:

NOCLEAN		No clean up of testing directory, if set.
VERBOSE		Be verbose, if set.
OPTIONS		Base options to build upon.
IU_TESTDIR	If set, use this as testing dir.  Unless
		NOCLEAN is also set, any created \$IU_TESTDIR
		will be erased after the test.  A non-existing
		directory must be named as a template for mktemp(1).
REMOTE_LOGHOST	Add this host as a receiving loghost.
TARGET		Receiving IPv4 address.
TARGET6		Receiving IPv6 address.

HERE
    exit 0
fi

# Execution control.  Initialise early!
#
do_cleandir=false
do_socket_length=true
do_unix_socket=true

# The UNIX socket name length is preset by the system
# and is also system dependent.
#
# A long name consists of 103 or 107 non-NUL characters,
# whereas the excessive string contains 104 or 108 characters.
# BSD allocates only 104, while Glibc and Solaris admit 108
# characters in "sun_path", a count which includes the final NUL.

iu_socklen_max=104	# BSD flavour!

IU_OS=`uname -s`
if test "$IU_OS" = "Linux" || test "$IU_OS" = "GNU/kFreeBSD" ||
	test "$IU_OS" = "SunOS"; then
    # Aim at the boundary of 108 characters.
    iu_socklen_max=108
fi

# The executables under test.
#
SYSLOGD=../src/syslogd$EXEEXT
LOGGER=../src/logger$EXEEXT

# Step into `tests/', should the invokation
# have been made outside of it.
#
[ -d src ] && [ -f tests/syslogd.sh ] && cd tests/

if [ $VERBOSE ]; then
    set -x
    $SYSLOGD --version | head -1
    $LOGGER --version | head -1
fi

if [ ! -x $SYSLOGD ]; then
    echo "Missing executable '$SYSLOGD'.  Failing." >&2
    exit 77
fi

if [ ! -x $LOGGER ]; then
    echo "Missing executable '$LOGGER'.  Failing." >&2
    exit 77
fi

# For file creation below IU_TESTDIR.
umask 0077

# Keep any external assignment of testing directory.
# Otherwise a randomisation is included.
#
: ${IU_TESTDIR:=$PWD/iu_syslog.XXXXXX}

if [ ! -d "$IU_TESTDIR" ]; then
    do_cleandir=true
    IU_TESTDIR="`mktemp -d "$IU_TESTDIR" 2>/dev/null`" ||
	{
	    echo 'Failed at creating test directory.  Aborting.' >&2
	    exit 77
	}
elif expr X"$IU_TESTDIR" : X"\.\{1,2\}/\{0,1\}$" >/dev/null; then
    # Eliminating directories: . ./ .. ../
    echo 'Dangerous input for test directory.  Aborting.' >&2
    exit 77
fi

# The SYSLOG daemon uses four files.
#
CONF="$IU_TESTDIR"/syslog.conf
PID="$IU_TESTDIR"/syslogd.pid
OUT="$IU_TESTDIR"/messages
: ${SOCKET:=$IU_TESTDIR/log}

# Are we able to write in IU_TESTDIR?
# This could happen with preset IU_TESTDIR.
#
touch "$OUT" || {
    echo 'No write access in test directory.  Aborting.' >&2
    exit 1
}

# Some automated build environments dig deep chroots, i.e.,
# make the paths to this working directory disturbingly long.
# Check SOCKET for this calamity.
#
if test `expr X"$SOCKET" : X".*"` -gt $iu_socklen_max; then
    do_unix_socket=false
    cat <<-EOT >&2
	WARNING! The working directory uses a disturbingly long path.
	We are not able to construct a UNIX socket on top of it.
	Therefore disabling socket messaging in this test run.
	EOT
fi

# Erase the testing directory.
#
clean_testdir () {
    if test -f "$PID" && ps "`cat "$PID"`" >/dev/null 2>&1; then
	kill -9 "`cat "$PID"`"
    fi
    if test -z "${NOCLEAN+no}" && $do_cleandir; then
	rm -r -f "$IU_TESTDIR"
    fi
    if $do_socket_length && test -d "$IU_TMPDIR"; then
	rmdir "$IU_TMPDIR"	# Should be empty.
    fi
}

# Clean artifacts as execution stops.
#
trap clean_testdir EXIT HUP INT QUIT TERM

# Test at this port.
# Standard is syslog at 514/udp.
PROTO=udp
PORT=514

# Receivers for INET sockets.
: ${TARGET:=127.0.0.1}
: ${TARGET6:=[::1]}

# For testing of critical lengths for UNIX socket names,
# we need a well defined base directory; choose $TMPDIR.
IU_TMPDIR=${TMPDIR:=/tmp}

if test ! -d "$IU_TMPDIR"; then
    do_socket_length=false
    cat <<-EOT >&2
	WARNING!  Disabling socket length test since the directory
	"$IU_TMPDIR", for temporary storage, does not exist.
	EOT
else
    # Append a slash if it is missing.
    expr X"$IU_TMPDIR" : X".*/$" >/dev/null || IU_TMPDIR="$IU_TMPDIR/"

    IU_TMPDIR="`mktemp -d "${IU_TMPDIR}iu.XXXXXX" 2>/dev/null`" ||
	{   # Directory creation failed.  Disable test.
	    cat <<-EOT >&2
		WARNING!  Unable to create temporary directory below
		"${TMPDIR:-/tmp}" for socket length test.
		Now disabling this subtest.
		EOT
	    do_socket_length=false
	}
fi

iu_eighty=0123456789
iu_eighty=${iu_eighty}${iu_eighty}
iu_eighty=${iu_eighty}${iu_eighty}
iu_eighty=${iu_eighty}${iu_eighty}

# This good name base will be expanded.
IU_GOOD_BASE=${IU_TMPDIR}/_iu

# Add a single character to violate the size condition.
IU_BAD_BASE=${IU_TMPDIR}/X_iu

if test `expr X"$IU_GOOD_BASE" : X".*"` -gt $iu_socklen_max; then
    # Maximum socket length is already less than prefix.
    echo 'WARNING! Disabling socket length test.  Too long base name' >&2
    do_socket_length=false
fi

if $do_socket_length; then
    # Compute any patching needed to get socket names
    # touching the limit of allowed length.
    iu_patch=''
    iu_pt="$IU_GOOD_BASE"	# Computational helper.

    if test `expr X"$iu_pt$iu_eighty" : X".*"` -le $iu_socklen_max; then
	iu_patch="$iu_patch$iu_eighty" && iu_pt="$iu_pt$iu_eighty"
    fi

    count=`expr X"$iu_pt" : X".*"`
    count=`expr $iu_socklen_max - $count`

    # $count gives the number, and $iu_eighty the characters.
    if test $count -gt 0; then
	iu_patch="$iu_patch`expr X"$iu_eighty" : X"\(.\{1,$count\}\)"`"
    fi

    IU_LONG_SOCKET="$IU_GOOD_BASE$iu_patch"
    IU_EXCESSIVE_SOCKET="$IU_BAD_BASE$iu_patch"
fi

# All messages intended for post-detection are
# to be uniformly tagged.
TAG="syslogd-test"

# Remove old files in use by daemon.
rm -f "$OUT" "$PID" "$CONF"

# Full testing needs a superuser.  Report this.
if [ `id -u` -ne 0 ]; then
    cat <<-EOT >&2
	WARNING!!  Disabling INET server tests since you seem
	to be underprivileged.
	EOT
else
    # Is the INET port already in use? If so,
    # skip the test in its entirety.
    if [ "`uname -s`" = "SunOS" ]; then
	netstat -na -finet -finet6 -P$PROTO |
	grep "\.$PORT[^0-9]" >/dev/null 2>&1
    else
	netstat -na |
	grep "^$PROTO\(4\|6\|46\)\{0,1\}.*[^0-9]$PORT[^0-9]" \
	    >/dev/null 2>&1
    fi
    if [ $? -eq 0 ]; then
	cat <<-EOT >&2
		The INET port $PORT/$PROTO is already in use.
		No reliable test of INET socket is possible.
	EOT
	exit 77
    fi
fi

# A minimal, catch-all configuration.
#
cat > "$CONF" <<-EOT
	*.*	$OUT
	# Test incorrect forwarding.
	*.*	@not.in.existence
	# Recover from missing action field and short selector.
	12345
	*.*
	*.	/dev/null
EOT

# Set REMOTE_LOGHOST to activate forwarding
#
if [ -n "$REMOTE_LOGHOST" ]; then
    # Append a forwarding stanza.
    cat >> "$CONF" <<-EOT
	# Forwarding remotely
	*.*	@$REMOTE_LOGHOST
	EOT
fi

# Attempt to start the server after first
# building the desired option list.
#
## Base configuration.
IU_OPTIONS="--rcfile='$CONF' --pidfile='$PID'"
if $do_unix_socket; then
    IU_OPTIONS="$IU_OPTIONS --socket='$SOCKET'"
else
    # The empty string will disable the standard socket.
    IU_OPTIONS="$IU_OPTIONS --socket=''"
fi
if $do_socket_length; then
    IU_OPTIONS="$IU_OPTIONS -a '$IU_LONG_SOCKET' -a '$IU_EXCESSIVE_SOCKET'"
fi

## Enable INET service when running as root.
if [ `id -u` -eq 0 ]; then
    IU_OPTIONS="$IU_OPTIONS --ipany --inet --hop"
fi
## Bring in additional options from command line.
## Disable kernel messages otherwise.
if [ -c /dev/klog ]; then
    : OPTIONS=${OPTIONS:=--no-klog}
fi
IU_OPTIONS="$IU_OPTIONS $OPTIONS"

# The eval-construct allows white space in file names,
# based on the use of single quotes in IU_OPTIONS.
eval $SYSLOGD $IU_OPTIONS

# Wait a moment in order to avoid an obvious
# race condition with the server daemon on
# slow systems.
#
sleep 1

# Test to see whether the service got started.
#
if [ ! -r "$PID" ]; then
    echo "The service daemon never started.  Failing." >&2
    exit 1
fi

# Declare the number of implemented tests,
# as well as an exit code.
#
TESTCASES=0
SUCCESSES=0
EXITCODE=1

# Check that the excessively long UNIX socket name was rejected.
if $do_socket_length; then
    TESTCASES=`expr $TESTCASES + 1`
    # Messages can be truncated in the message log, so make a best
    # effort to limit the length of the string we are searching for.
    # Allowing 55 characters for IU_BAD_BASE is almost aggressive.
    # A host name of length six would allow 64 characters
    pruned=`expr "UNIX socket name too long.*${IU_BAD_BASE}" : '\(.\{1,82\}\)'`
    if grep "$pruned" "$OUT" >/dev/null 2>&1; then
	SUCCESSES=`expr $SUCCESSES + 1`
    fi
fi

# Send messages on two sockets: IPv4 and UNIX.
#
if $do_unix_socket; then
    TESTCASES=`expr $TESTCASES + 1`
    $LOGGER -h "$SOCKET" -p user.info -t "$TAG" \
	"Sending BSD message. (pid $$)"
fi

if $do_socket_length; then
    TESTCASES=`expr $TESTCASES + 1`
    $LOGGER -h "$IU_LONG_SOCKET" -p user.info \
	-t "$TAG" "Sending via long socket name. (pid $$)"
fi

if [ `id -u` -eq 0 ]; then
    TESTCASES=`expr $TESTCASES + 2`
    $LOGGER -4 -h "$TARGET" -p user.info -t "$TAG" \
	"Sending IPv4 message. (pid $$)"
    $LOGGER -6 -h "$TARGET6" -p user.info -t "$TAG" \
	"Sending IPv6 message. (pid $$)"
fi

# Detection of registered messages.
#
COUNT=`grep "$TAG" "$OUT" | wc -l`
SUCCESSES=`expr $SUCCESSES + $COUNT`

if [ -n "${VERBOSE+yes}" ]; then
    cat <<-EOT
	---------- Successfully detected messages. ----------
	`grep "$TAG" "$OUT"`
	---------- Full message log for syslogd. ------------
	`cat "$OUT"`
	-----------------------------------------------------
	EOT
fi

echo "Registered $SUCCESSES successes out of $TESTCASES."

if [ "$SUCCESSES" -eq "$TESTCASES" ]; then
    echo "Successful testing."
    EXITCODE=0
else
    echo "Failing some tests."
fi

# Remove the daemon process.
[ -r "$PID" ] && kill "`cat "$PID"`"

exit $EXITCODE
