#! /bin/bash

echo ""

echo "UML2oWFN compiler for soundness analysis tests"
echo "[strong termination and safeness, LoLA, taskfiles]"
echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a safe -p taskfile -f lola -o"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a safe -p taskfile -f lola -o &> /dev/null
result1=$?
if [ ! -f $testdir/soundness/check_TestSuite_soundness.sh ] ; then
  result1=1
  echo "[fail] no shell script generated"
fi
if [ ! -f $testdir/soundness/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.fin.task -o ! -f $testdir/soundness/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.safe.task ] ; then
  result2=1
  echo "[fail] task files not generated"
fi
rm -f $testdir/soundness/*.task
rm -f $testdir/soundness/*.lola
rm -f $testdir/soundness/*.sh

echo ""
echo "[strong termination, LoLA, no taskfiles]"
echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -f lola -o"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -f lola -o &> /dev/null
result2=$?
if [ ! -f $testdir/soundness/check_TestSuite_soundness.sh ] ; then
  result2=1
  echo "[fail] no shell script generated"
fi
if [ -f $testdir/soundness/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.fin.task -o -f $testdir/soundness/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.safe.task ] ; then
  result2=1
  echo "[fail] task files generated"
fi
rm -f $testdir/soundness/*.lola
rm -f $testdir/soundness/*.sh

echo ""
echo "[strong termination and safeness, LoLA, taskfiles, orJoin termination semantics]"
echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a safe -a orJoin -p taskfile -f lola -o"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a safe -a orJoin -p taskfile -f lola -o &> /dev/null
result3=$?
if [ ! -f $testdir/soundness/check_TestSuite_soundness.sh ] ; then
  result3=1
  echo "[fail] no shell script generated"
fi
rm -f $testdir/soundness/*.task
rm -f $testdir/soundness/*.lola
rm -f $testdir/soundness/*.sh

echo ""
echo "[strong termination and safeness, LoLA, taskfiles, weak data-flow termination semantics]"
echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a safe -a removePinsets -p taskfile -f lola -o"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a safe -a removePinsets -p taskfile -f lola -o &> /dev/null
result4=$?
if [ ! -f $testdir/soundness/check_TestSuite_soundness.sh ] ; then
  result4=1
  echo "[fail] no shell script generated"
fi
rm -f $testdir/soundness/*.task
rm -f $testdir/soundness/*.lola
rm -f $testdir/soundness/*.sh

echo ""
echo "[Woflan (TPN)]"
echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a wfNet -f tpn -o"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter -a soundness -a wfNet -f tpn -o &> /dev/null
result5=$?
rm -f $testdir/soundness/*.tpn

if test \( $result1 -eq 0 -a $result2 -eq 0 -a $result3 -eq 0 -a $result4 -eq 0 -a $result5 -eq 0 \)
then
  result=0
else
  result=1
fi

exit $result
