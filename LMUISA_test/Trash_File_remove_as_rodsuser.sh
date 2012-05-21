echo $0

export input=`basename $0`.txt
echo test > $input

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput $input
imv $input $Trash

echo
echo Before...
ils $Trash

##
## action to test
##
icd $Trash
irm $input

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: irm should not succeed."
fi

echo
echo ... After
ils $Trash
echo
