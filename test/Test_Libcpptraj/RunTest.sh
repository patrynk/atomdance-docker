#!/bin/bash
IS_LIBCPPTRAJ='yes'
. ../MasterTest.sh
# Clean
CleanFiles Makefile Test.cpp a.out

TESTNAME='LIBCPPTRAJ linking test.'
echo ""
echo "  CPPTRAJ: $TESTNAME"
if [ -z "$CPPTRAJ_DACDIF" ] ; then
  OUT "  CPPTRAJ: $TESTNAME"
fi

# First determine whether we are part of AmberTools directory
if [ $STANDALONE -eq 0 ] ; then
  IN_AMBERTOOLS=1
else
  IN_AMBERTOOLS=0
fi

# Determine location of config.h, needed for compiler vars.
CONFIG_H=''
if [ "$IN_AMBERTOOLS" -eq 0 ] ; then
  if [ -f '../../config.h' ] ; then
    CONFIG_H='../../config.h'
  fi
else
  if [ -f '../../../config.h' ] ; then
    CONFIG_H='../../../config.h'
  fi
fi
if [ -z "$CONFIG_H" ] ; then
  echo "Warning: File '$CONFIG_H' not found. Cannot run libcpptraj test."
  exit 0
fi

# Check for libcpptraj based on being in AmberTools or not.
LIBCPPTRAJ_DIR=''
if [ "$IN_AMBERTOOLS" -eq 0 ] ; then
  if [ -e '../../lib' ] ; then
    LIBCPPTRAJ_DIR='../../lib'
  fi
else
  if [ -e '../../../../lib' ] ; then
    LIBCPPTRAJ_DIR='../../../../lib'
  fi
fi
if [ -z "$LIBCPPTRAJ_DIR" ] ; then
  echo "Warning: Lib directory '$LIBCPPTRAJ_DIR' does not exist. Cannot run libcpptraj test."
  exit 0
fi

# Make sure libcpptraj library exists. Use '*' to pick up .so, .dylib, etc
if [ ! -f "$LIBCPPTRAJ_DIR/libcpptraj.so" -a ! -f "$LIBCPPTRAJ_DIR/libcpptraj.dylib" ] ; then
  echo "Warning: libcpptraj not found in '$LIBCPPTRAJ_DIR'. Cannot run libcpptraj test."
  exit 0
fi
LIBCPPTRAJ=`ls $LIBCPPTRAJ_DIR/libcpptraj.*`

# Generate Makefile
cat > Makefile <<EOF
# libcpptraj test Makefile, automatically generated.
include $CONFIG_H

LIBCPPTRAJ=-L$LIBCPPTRAJ_DIR -lcpptraj

test_libcpptraj: Test.cpp
	\$(CXX) -I../../src -o a.out Test.cpp \$(LIBCPPTRAJ)
EOF

# Generate test program
cat > Test.cpp <<EOF
#include "Cpptraj.h"

int main(int argc, char **argv) {
  Cpptraj Program;
  return Program.RunCpptraj(argc, argv);
}
EOF
 
# Make the test program
make test_libcpptraj
if [ "$?" -ne 0 ] ; then
  ProgramError "Could not compile with libcpptraj." "libcpptraj"
  exit 1
else
  # Run the test program. Export library to avoid any issues
  export DYLD_FALLBACK_LIBRARY_PATH=$LIBCPPTRAJ_DIR:$DYLD_FALLBACK_LIBRARY_PATH
  export LD_LIBRARY_PATH=$LIBCPPTRAJ_DIR:$LD_LIBRARY_PATH
  VERSION=`./a.out --version`
  STATUS=$?
  echo "$VERSION"
  if [ $STATUS -ne 0 -o -z "$VERSION" ] ; then
    ProgramError "Cannot execute program built with libcpptraj" "libcpptraj"
    exit 1
  else
    # Pseudo DoTest() execution
    ((NUMCOMPARISONS++))
    if [ -z "$CPPTRAJ_DACDIF" ] ; then
      # Standalone pass.
      OUT  "  LIBCPPTRAJ OK."
    else
      # AmberTools pass.
      echo "PASSED"
    fi
  fi
fi

EndTest
exit 0
