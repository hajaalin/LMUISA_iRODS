echo $0

export input=/home/hajaalin/Data/test/experiment--2011_04_20_07_15_22

# go to the iRODS collection created for the test run
icd $Data

# import input file in iRODS
iput -r $input

echo
echo Before...
ils $Data/experiment--2011_04_20_07_15_22
ils /$IRODSZONE/home/$IRODSUSER/Conversions/experiment--2011_04_20_07_15_22_cellomics
irm -r /$IRODSZONE/home/$IRODSUSER/Conversions/experiment--2011_04_20_07_15_22_cellomics

##
## action to test
##
sleep 180
ils /$IRODSZONE/home/$IRODSUSER/Conversions/experiment--2011_04_20_07_15_22

if [ $? -eq 0 ]
then
    echo "LMUISA_ERROR: $0: Conversions not found."
fi

echo
echo ... After
ils /$IRODSZONE/home/$IRODSUSER/Conversions/experiment--2011_04_20_07_15_22_cellomics
echo
