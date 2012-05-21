echo $0

export input=experiment--2001_01_01_01_01_06
mkdir $input
echo test > $input/test1.txt
echo test > $input/test2.txt

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
imv $input/test1.txt $Test/

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should not succeed out of Data."
fi

echo
echo ... After ...
ils $input

##
## action to test
##
imv $input/test2.txt $Test/test2.txt.renamed

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should not succeed out of Data."
fi

echo
echo ... And after
ils $input
echo
