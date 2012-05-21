#!/bin/bash
echo $0

export input=`basename $0`.dir
mkdir $input
echo test1 > $input/test1.txt
echo test2 > $input/test2.txt
mkdir $input/sub1
echo test1 > $input/sub1/s1test1.txt
echo test2 > $input/sub1/s1test2.txt

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput -r $input

echo
echo Before...
ils -l
echo iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data/$input%'"
iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data/$input%'"

##
## action to test
##

sleep 120
echo $Data
echo $input
echo iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data/$input%'"
iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data/$input%'"
reps=`iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data/$input%'"`
echo $reps
if [ $reps -ne 8 ]
then
    echo "LMUISA_ERROR: $0: There should be two replicates of all files under $Data."
fi

echo
echo ... After
ils -l
echo

