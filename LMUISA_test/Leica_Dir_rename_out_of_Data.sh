echo $0

export input=experiment--2001_01_01_01_01_02
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
imv $input $Test/$input.renamed

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should not succeed out of Data."
fi

echo
echo ... After
ils $Test
ils
echo

