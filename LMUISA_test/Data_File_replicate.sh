echo $0

export input=`basename $0`.txt

echo test > $input

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput $input

echo
echo Before...
ils -l

##
## action to test
##

sleep 120
echo $Data
echo $input
echo iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data' and DATA_NAME like '$input'"
iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data' and DATA_NAME like '$input'"
reps=`iquest "%s" "select count(DATA_ID) where COLL_NAME like '$Data' and DATA_NAME like '$input'"`
echo $reps
if [ $reps -ne 2 ]
then
    echo "LMUISA_ERROR: $0: There should be two replicates of $Data/$input."
fi

echo
echo ... After
ils -l
echo
