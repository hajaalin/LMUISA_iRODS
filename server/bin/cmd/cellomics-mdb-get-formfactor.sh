#!/bin/sh
echo "Select Name from asnFormFactor" | mdb-sql -HFp $1 |sed -e '/^$/ d'
