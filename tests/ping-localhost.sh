#!/bin/sh

if [ $VERBOSE ]; then
    set -x
    ping --version
fi

if [ `id -u` != 0 ]; then
    echo "ping needs to run as root"
    exit 77
fi

ping -c 1 127.0.0.1; errno=$?

exit $errno
