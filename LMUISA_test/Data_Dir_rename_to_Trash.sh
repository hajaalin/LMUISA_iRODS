echo $0

export input=`basename $0`.dir
mkdir $input
echo test1 > $input/test1.txt
echo test2 > $input/test2.txt
echo test3 > $input/test3.txt
mkdir $input/sub1
echo test1 > $input/sub1/s1test1.txt
echo test2 > $input/sub1/s1test2.txt
mkdir $input/sub2
echo test1 > $input/sub2/s2test1.txt
echo test2 > $input/sub2/s2test2.txt

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

