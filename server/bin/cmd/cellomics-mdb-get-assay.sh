#!/bin/sh
echo "Select AssayMod from asnProtocol" | mdb-sql -HFp $1 |sed -e '/^$/ d'
