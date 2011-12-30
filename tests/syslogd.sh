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
IU_TESTDIR	If set, use this as testing dir. Unless
		NOCLEAN is also set, any created \$IU_TESTDIR
		will be erased after the test.
REMOTE_LOGHOST	Add this host as a receiving loghost.

HERE
	exit 0
fi

# Keep any external assignment of testing directory.
# Otherwise a randomisation is included.
#
: ${IU_TESTDIR:=$PWD/iu_syslog.XXXXXX}

if [ ! -d $IU_TESTDIR ]; then
	IU_DO_CLEANDIR=yes
	IU_TESTDIR=$(mktemp -d $IU_TESTDIR)
fi

# Erase the testing directory.
#
clean_testdir () {
	if test -z "${NOCLEAN+no}" && test "$IU_DO_CLEANDIR" = "yes"; then
		rm -fR $IU_TESTDIR
	fi
	test -f "$PID" && ps "$(cat $PID)" >/dev/null 2>&1 \
		&& kill -9 "$(cat $PID)"
}

# Clean artifacts as execution stops.
#
trap clean_testdir EXIT HUP INT QUIT TERM


CONF=$IU_TESTDIR/syslog.conf
PID=$IU_TESTDIR/syslogd.pid
OUT=$IU_TESTDIR/messages
: ${SOCKET:=$IU_TESTDIR/log}

# Test at this port.
# Standard is syslog at 514/udp.
PROTO=udp
PORT=514

# For testing of critical lengths for UNIX socket names,
# we need a well defined base directory; choose "/tmp/".
IU_TEN=0123456789
IU_TWENTY=${IU_TEN}${IU_TEN}
IU_FORTY=${IU_TWENTY}${IU_TWENTY}
IU_EIGHTY=${IU_FORTY}${IU_FORTY}

# This good name base consumes twentythree chracters.
IU_GOOD_BASE=/tmp/$(date +%y-%m-%d)_socket_iu

# Add a single character to violate the size condition.
IU_BAD_BASE=/tmp/X$(date +%y-%m-%d)_socket_iu

IU_OS=$(uname -s)
if test "${IU_OS}" = "Linux" || test "${IU_OS}" = "GNU/kFreeBSD" \
    || test "${IU_OS}" = "SunOS"; then
	# Aim at the boundary of 108 characters.
	IU_GOOD_BASE=${IU_GOOD_BASE}_lnx
	IU_BAD_BASE=${IU_BAD_BASE}_lnx
fi

# Establish largest possible socket name.  The long
# name consists of 103 or 107 non-NUL characters,
# where the excessive string contains 104 or 108.
# BSD allocates only 104, whereas GLIBC and Solaris
# admits 108 characters in "sun_path", including NUL.
IU_LONG_SOCKET=${IU_GOOD_BASE}${IU_EIGHTY}
IU_EXCESSIVE_SOCKET=${IU_BAD_BASE}${IU_EIGHTY}

# All messages intended for post-detection are
# to be uniformly tagged.
TAG="syslogd-test"

# The executables under test.

IU_SYSLOGD=./src/syslogd$EXEEXT
IU_LOGGER=./src/logger$EXEEXT

# Step out of `tests/', should the invokation
# have been made there.
#
[ -d ../src ] && cd ..

if [ $VERBOSE ]; then
    set -x
    $IU_SYSLOGD --version
    $IU_LOGGER --version
fi

if [ ! -x $IU_SYSLOGD ]; then
	echo "Missing executable 'syslogd'. Failing."
	exit 77
fi

if [ ! -x $IU_LOGGER ]; then
	echo "Missing executable 'logger'. Failing."
	exit 77
fi

# Remove old messages.
rm -f $OUT $PID

# Full testing needs a superuser.  Report this.
if [ $(id -u) -ne 0 ]; then
	cat <<-EOT
	WARNING!!
	Disabling INET server tests since you seem
	to be underprivileged.
	EOT
else
	# Is the INET port already in use? If so,
	# skip the test in its entirety.
	netstat -na | grep -q -E "^$PROTO(4|6|46)?.*[^0-9]$PORT[^0-9]"
	if [ $? -eq 0 ]; then
		echo "The INET port $PORT/$PROTO is already in use."
		echo "No reliable test is possible."
		exit 77
	fi
fi

# A minimal, catch-all configuration.
#
cat > $CONF <<-EOT
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
	cat >> $CONF <<-EOT
		# Forwarding remotely
		*.*	@$REMOTE_LOGHOST
	EOT
fi

# Attempt to start the server after first
# building the desired option list.
#
## Base configuration.
IU_OPTIONS="--rcfile=$CONF --pidfile=$PID --socket=$SOCKET"
IU_OPTIONS="$IU_OPTIONS -a $IU_LONG_SOCKET -a $IU_EXCESSIVE_SOCKET"

## Enable INET service when running as root.
if [ $(id -u) -eq 0 ]; then
	IU_OPTIONS="$IU_OPTIONS --ipany --inet --hop"
fi
## Bring in additional options from command line.
## Disable kernel messages otherwise.
if [ -c /dev/klog ]; then
	: OPTIONS=${OPTIONS:=--no-klog}
fi
IU_OPTIONS="$IU_OPTIONS $OPTIONS"

$IU_SYSLOGD $IU_OPTIONS

# Wait a moment in order to avoid an obvious
# race condition with the server daemon on
# slow systems.
#
sleep 1

# Test to see whether the service got started.
#
if [ ! -r $PID ]; then
	echo "The service daemon never started. Failing."
	exit 1
fi

# Declare the number of implemented tests,
# as well as an exit code.
#
TESTCASES=0
SUCCESSES=0
EXITCODE=1

# Check that the excessively long UNIX socket name was rejected.
TESTCASES=$((TESTCASES + 1))
if grep -q "UNIX socket name too long.*${IU_BAD_BASE}" $OUT; then
	SUCCESSES="$((SUCCESSES + 1))"
fi

# Send messages on two sockets: IPv4 and UNIX.
#
TESTCASES=$((TESTCASES + 2))
$IU_LOGGER -h $SOCKET -p user.info -t $TAG "Sending BSD message. (pid $$)"
$IU_LOGGER -h $IU_LONG_SOCKET -p user.info -t $TAG "Sending via long socket name. (pid $$)"

if [ $(id -u) -eq 0 ]; then
	TESTCASES=$((TESTCASES + 2))
	$IU_LOGGER -4 -h 127.0.0.1 -p user.info -t $TAG "Sending IPv4 message. (pid $$)"
	$IU_LOGGER -6 -h "[::1]" -p user.info -t $TAG "Sending IPv6 message. (pid $$)"
fi

# Detection of registered messages.
#
SUCCESSES="$((SUCCESSES + $(grep $TAG $OUT | wc -l) ))"

if [ -n "${VERBOSE+yes}" ]; then
	grep $TAG $OUT
fi

echo "Registered $SUCCESSES successes out of $TESTCASES."

if [ "$SUCCESSES" -eq "$TESTCASES" ]; then
	echo "Successful testing."
	EXITCODE=0
else
	echo "Failing some tests."
fi

# Remove the daemon process.
[ -r $PID ] && kill "$(cat $PID)"

exit $EXITCODE
