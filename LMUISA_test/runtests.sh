#!/bin/sh
export IRODSZONE=`getIrodsEnvValue.sh irodsZone`
export IRODSUSER=`getIrodsEnvValue.sh irodsUserName`

echo "Running tests as user '$IRODSUSER' on zone '$IRODSZONE'."

# create unique id for this test run
export testID=test_`date +%s`
echo "testID: " $testID

mkdir $testID
cd $testID
pwd

# create a new iRODS collection in Data,Trash and test for this ID
export Data=/$IRODSZONE/home/$IRODSUSER/Data/Test/$testID
export Trash=/$IRODSZONE/home/$IRODSUSER/Trash/$testID
export Test=/$IRODSZONE/home/$IRODSUSER/test/$testID

imkdir -p $Data
imkdir $Trash
imkdir -p $Test

for f in $@
do
  echo "Processing file $f ..."
  sleep 1

  log=`basename $f`	
  ../$f 2>&1  | tee $log.out
done
