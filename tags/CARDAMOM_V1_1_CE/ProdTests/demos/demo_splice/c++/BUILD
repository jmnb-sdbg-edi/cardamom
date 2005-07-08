#
# IDENTIFICATION   = 1000000097677
# REVISION         = 4
# DATE             = 26-JAN-2005 14:29:53
# DESCRIPTION      =  
# SECURITY         = UNCLASSIFIED  
# STATUS           = WORKING
# 
#

#
# Copyright THALES NEDERLAND B.V. and/or its suppliers
#
# THIS SOFTWARE SOURCE CODE AND ANY EXECUTABLE DIRVED THEREOF ARE PROPRIETARY
# TO THALES NEDERLAND B.V. AND/OR ITS SUPPLIERS, AS APPLICABLE, AND SHALL NOT
# BE USED IN ANY WAY OTHER THAN BEFOREHAND AGREED ON BY THALES NEDERLAND B.V.,
# NOR BE REPRODUCED OR DISCLOSED TO THIRD PARTIES WITHOUT PRIOR WRITTEN
# AUTHORIZATION BY THALES NEDERLAND B.V. AND/OR ITS SUPPLIERS, AS APPLICABLE.
#

TAO_COMP=`which tao_idl 2> /dev/null`
if [ -z "$TAO_COMP" -o -z "$TAO_ROOT" -o -z "$ACE_ROOT" ]
then
    echo "Setup the TOA environment first"
    exit
fi

CXX_COMP=`which g++ 2> /dev/null`
if [ -z "$CXX_COMP" ]
then
    echo "Setup the C++ environment first"
    exit
fi

SPLICE_COMP=`which idlpp 2> /dev/null`
if [ -z "$SPLICE_COMP" ]
then
    echo "Setup the SPLICE environment first"
    exit
fi

mkdir -p bld
mkdir -p exec
cd bld

export ORB_INCLUDE=$ACE_ROOT/../include
export ORB_LDLIBS=$ACE_ROOT/../lib

echo "Compiling IDL with TOA IDL compiler"
tao_idl -Sa -St -Sc -Sp -Sd -Sv ../pingpong.idl

echo "Compiling IDL with SPLICE IDL compiler"
idlpp ../pingpong.idl

echo "Compiling TAO User Types support code"
g++ -g -D_TYPECHECK_ -I./ -I$ORB_INCLUDE -c pingpongC.cpp

echo "Compiling SPLICE User Types support code"
g++ -g -D_TYPECHECK_ -I./ -I$ORB_INCLUDE -I$SPLICE_HOME/include/dcps/C++/TAO -c pingpongSplDcps.cpp

echo "Compiling Application Foo Types support code"
g++ -g -D_TYPECHECK_ -I./ -I$ORB_INCLUDE -I$SPLICE_HOME/include/dcps/C++/TAO -c pingpongDcps.cpp

echo "Compiling PING"
g++ -g -D_TYPECHECK_ -I./ -I$ORB_INCLUDE -I$SPLICE_HOME/include/dcps/C++/TAO -c ../ping.cpp

echo "Compiling PONG"
g++ -g -D_TYPECHECK_ -I./ -I$ORB_INCLUDE -I$SPLICE_HOME/include/dcps/C++/TAO -c ../pong.cpp

echo "Linking PING"
g++ -o ../exec/ping -L$SPLICE_HOME/lib ping.o pingpongC.o pingpongDcps.o pingpongSplDcps.o -L$ORB_LDLIBS -lTAO -ldcpscorbac++ -lstdc++

echo "Linking PONG"
g++ -o ../exec/pong -L$SPLICE_HOME/lib pong.o pingpongC.o pingpongDcps.o pingpongSplDcps.o -L$ORB_LDLIBS -lTAO -ldcpscorbac++ -lstdc++

cd ..
