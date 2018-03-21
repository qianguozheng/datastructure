#!/bin/sh

set -x

count=0
TARGET="/tmp/dnsmasq.log.0"
while true
do
    #check file exist
    if [ -f "$TARGET" ]; then
        #mv to /media
        count=`expr $count + 1`;
        mv $TARGET /tmp/"$TARGET.$count"
        echo $?
    fi

    sleep 10

done
