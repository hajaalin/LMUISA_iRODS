echo $0

export input=`basename $0`.txt

echo test > $input

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput $input

echo
echo Before...
ils

##
## action to test
##
imv $input $Trash/$input

if [ $? -ne 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should succeed to Trash."
fi

echo
echo ... After
ils $Trash
echo

