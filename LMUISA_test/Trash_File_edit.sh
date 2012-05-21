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
cat $input
ils $Trash

##
## action to test
##
echo test2 >> $input
icd $Trash
iput -f $input

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: iput -f should not succeed."
fi

echo
echo ... After
ils
rm $input
iget $input
cat $input
echo
