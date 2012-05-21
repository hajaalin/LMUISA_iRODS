echo $0

export input=`basename $0`.dir
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
imv $input $input.renamed

if [ $? -ne 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should succeed within Data."
fi

echo
echo ... After
ils
echo

