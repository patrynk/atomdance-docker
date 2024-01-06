#!/bin/bash

. ../MasterTest.sh

CleanFiles corr.in acorr.gnu
TESTNAME='Atomic Correlation test'
Requires netcdf
INPUT="-i corr.in"
cat > corr.in <<EOF
parm ../tz2.parm7
trajin ../tz2.nc
atomiccorr out acorr.gnu
EOF
RunCpptraj "$TESTNAME"
DoTest acorr.gnu.save acorr.gnu
EndTest

exit 0

