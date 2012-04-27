#!/bin/sh
mdbout=`echo "Select AddUser from asnProtocol" | mdb-sql -HFp $1`
# |sed -e '/^$/ d'
printf "%s" $mdbout
