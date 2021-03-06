#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=statesReduction
DIR=$testdir/$SUBDIR
FIONA=fiona

# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/06-03-23_BPM06_shop_sect_3.output.owfn.ig.og
rm -f $DIR/06-03-23_BPM06_shop_sect_3.output.owfn.R.ig.og
rm -f $DIR/06-03-23_BPM06_shop_sect_3.output.owfn.og
rm -f $DIR/06-03-23_BPM06_shop_sect_3.output.owfn.R.og
rm -f $DIR/06-03-23_BPM06_shop_sect_6.output.owfn.ig.og
rm -f $DIR/06-03-23_BPM06_shop_sect_6.output.owfn.R.ig.og
rm -f $DIR/myCoffee.output.owfn.ig.og
rm -f $DIR/myCoffee.output.owfn.R.ig.og
rm -f $DIR/dke07_shop_sect_3.output.owfn.ig.og
rm -f $DIR/dke07_shop_sect_3.output.owfn.R.ig.og
rm -f $DIR/dke07_shop_sect_6.output.owfn.ig.og
rm -f $DIR/dke07_shop_sect_6.output.owfn.R.ig.og
rm -f $DIR/*.log

result=0
#exit $result


############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
outputPrefix="$builddir/$SUBDIR/06-03-23_BPM06_shop_sect_3.owfn.output"
cmd1="$FIONA $owfn -t eqR -t IG -o $outputPrefix"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`
fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?
    
    if [ $equivalent -ne 0 ]
    then
        echo   ... the two graphs do not characterize the same strategies
    fi
    result=`expr $result + $equivalent`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
# ==> OG
############################################################################

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
outputPrefix="$builddir/$SUBDIR/06-03-23_BPM06_shop_sect_3.owfn.output"
cmd1="$FIONA $owfn -t eqR -t OG -o $outputPrefix"


echo running $cmd1
OUTPUT=`$cmd1 2>&1`
fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        equivalent=$?

    if [ $equivalent -ne 0 ]
    then
        echo   ... the two graphs do not characterize the same strategies
    fi
    result=`expr $result + $equivalent`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
outputPrefix="$builddir/$SUBDIR/06-03-23_BPM06_shop_sect_6.owfn.output"
cmd1="$FIONA $owfn -t eqR -t IG -o $outputPrefix"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`
fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        equivalent=$?
    
    if [ $equivalent -ne 0 ]
    then
        echo   ... the two graphs do not characterize the same strategies
    fi
    
    result=`expr $result + $equivalent`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################

owfn="$DIR/myCoffee.owfn"
outputPrefix="$builddir/$SUBDIR/myCoffee.owfn.output"
cmd1="$FIONA $owfn -t eqR -t IG -o $outputPrefix"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`
fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        equivalent=$?
    
    if [ $equivalent -ne 0 ]
    then
        echo   ... the two graphs do not characterize the same strategies
    fi
    
    result=`expr $result + $equivalent`
fi
############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################

owfn="$DIR/dke07_shop_sect_3.owfn"
outputPrefix="$builddir/$SUBDIR/dke07_shop_sect_3.owfn.output"
cmd1="$FIONA $owfn -t eqR -t IG -o $outputPrefix"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`
fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        equivalent=$?
    
    if [ $equivalent -ne 0 ]
    then
        echo   ... the two graphs do not characterize the same strategies
    fi
    
    result=`expr $result + $equivalent`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################

owfn="$DIR/dke07_shop_sect_6.owfn"
outputPrefix="$builddir/$SUBDIR/dke07_shop_sect_6.owfn.output"
cmd1="$FIONA $owfn -t eqR -t IG -o $outputPrefix"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`
fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        equivalent=$?
    
    if [ $equivalent -ne 0 ]
    then
        echo   ... the two graphs do not characterize the same strategies
    fi

    result=`expr $result + $equivalent`
fi

############################################################################

############################################################################

echo

exit $result
