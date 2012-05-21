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
irm $input

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: irm $input should not succeed."
fi

echo
echo ... After
ils
echo
