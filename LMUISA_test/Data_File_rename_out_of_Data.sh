echo $0

export input=`basename $0`.txt

echo test > $input

# go to the iRODS collection created for the test run
icd $Data
ipwd

# import input file in iRODS
iput $input

echo
echo Before...
ils

##
## action to test
##
#imv $input /LMUISA_b1/home/hajaalin/test/$input.renamed
imv $input $Test/$input.renamed

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: imv $input should not succeed out of Data."
fi

echo
echo ... After
ils
echo
