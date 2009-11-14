#!/bin/sh

if [ $VERBOSE ]; then
    set -x
    traceroute --version
fi

if [ `id -u` != 0 ]; then
    echo "traceroute needs to run as root"
    exit 77
fi

traceroute 127.0.0.1; errno=$?

exit $errno
