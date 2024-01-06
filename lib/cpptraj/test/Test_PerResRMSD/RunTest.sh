#!/bin/bash

. ../MasterTest.sh

# Clean
CleanFiles PerResRMSD.agr cpptraj.in test.dat perresavg.dat center.agr

TESTNAME='Per-residue RMSD tests'
Requires netcdf maxthreads 10

INPUT="-i cpptraj.in"
# Test 1
cat > cpptraj.in <<EOF
parm ../tz2.truncoct.parm7
reference ../tz2.truncoct.nc 1
trajin ../tz2.truncoct.nc  
rmsd :2-11 refindex 0 perres perresout PerResRMSD.agr
EOF
RunCpptraj "Per-Residue RMSD Test."
DoTest PerResRMSD.agr.save PerResRMSD.agr

# Test 2
UNITNAME='Per-Residue RMSD Test with averaging'
CheckFor maxthreads 3
if [ $? -eq 0 ] ; then
  cat > cpptraj.in <<EOF
parm ../tz2.truncoct.parm7
reference ../tz2.truncoct.nc 1
trajin ../tz2.truncoct.nc 2 4 
rmsd R2-11 :2-11 refindex 0 perres perresavg perresavg.dat
EOF
  RunCpptraj "$UNITNAME"
  DoTest perresavg.dat.save perresavg.dat
fi

# Test 3
cat > cpptraj.in <<EOF
parm ../tz2.truncoct.parm7
reference ../tz2.truncoct.nc 1
trajin ../tz2.truncoct.nc  
rmsd :2-11 refindex 0 perres perresout center.agr range 1 perrescenter 
EOF
RunCpptraj "Per-Residue RMSD Test with residue centering."
DoTest center.agr.save center.agr

EndTest

exit 0
