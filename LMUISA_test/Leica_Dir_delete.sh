echo $0

export input0=`basename $0`.dir
export input=$input0/experiment--2001_01_01_01_01_01
mkdir -p $input
echo test > $input/test.txt

# go to the iRODS collection created for the test run
icd $Data
imkdir $input0

# import input file in iRODS
ils
iput -r $input $input0

echo
echo Before...
ils $input0

##
## action to test
##
irm -r $input0/$input

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: irm $input should not succeed."
fi

echo
echo ... After
ils
ils $input
echo

