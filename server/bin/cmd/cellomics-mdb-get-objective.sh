#!/bin/sh
echo "Select Objective from asnProtocol" | mdb-sql -HFp $1 |sed -e '/^$/ d'
