echo $0

export input=experiment--2001_01_01_01_01_03
mkdir $input
echo test > $input/test.txt

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput -r $input

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


