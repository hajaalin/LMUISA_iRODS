echo $0

export input=experiment--2001_01_01_01_01_01
mkdir $input
echo test > $input/test.txt

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput -r $input

echo
echo Before...
ils $input

##
## action to test
##
imv $input/test.txt $Trash

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should not succeed from Leica folder."
fi

echo
echo ... After
ils $input
ils $Trash
echo


