#
# IDENTIFICATION   = 1000000117109
# REVISION         = 1
# DATE             = 22-JUL-2005 05:04:51
# DESCRIPTION      =  
# SECURITY         = UNCLASSIFIED  
# STATUS           = RELEASED
# 

#
# Copyright THALES NEDERLAND B.V. and/or its suppliers
#
# THIS SOFTWARE SOURCE CODE AND ANY EXECUTABLE DERIVED THEREOF ARE PROPRIETARY
# TO THALES NEDERLAND B.V. AND/OR ITS SUPPLIERS, AS APPLICABLE, AND SHALL NOT
# BE USED IN ANY WAY OTHER THAN BEFOREHAND AGREED ON BY THALES NEDERLAND B.V.,
# NOR BE REPRODUCED OR DISCLOSED TO THIRD PARTIES WITHOUT PRIOR WRITTEN
# AUTHORIZATION BY THALES NEDERLAND B.V. AND/OR ITS SUPPLIERS, AS APPLICABLE.
#

#
# This example build is valid for OpenFusion TAO only
#

TAO_COMP=`which tao_idl 2> /dev/null`
if [ -z "$TAO_COMP" -o -z "$TAO_ROOT" ]
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

echo "Compiling IDL with TOA IDL compiler"
tao_idl -I../ -I$SPLICE_HOME/include/dcps/C++/CCPP -Sc -Sp -Sd -Sv ../pingpong.idl

echo "Compiling IDL with SPLICE IDL compiler"
idlpp -l cpp ../pingpong.idl

echo "Compiling IDL Typed Interfaces with TOA IDL compiler"
tao_idl -I../ -I$SPLICE_HOME/include/dcps/C++/CCPP -Sc -Sp -Sd -Sv pingpongDcps.idl

echo "Compiling TAO User Types support code"
g++ -g -DDDS_OpenFusion_1_4_1 -I./ -I$TAO_ROOT/include -c pingpongC.cpp

echo "Compiling SPLICE User Types support code"
g++ -g -DDDS_OpenFusion_1_4_1 -c pingpongSplDcps.cpp \
	-I./ -I$TAO_ROOT/include -I$SPLICE_HOME/include/dcps/C++/CCPP/OpenFusion \
	-I$SPLICE_HOME/include/dcps/C++/CCPP

echo "Compiling TAO Typed Interfaces support code"
g++ -g -DDDS_OpenFusion_1_4_1 -c pingpongDcpsC.cpp \
	-I./ -I$TAO_ROOT/include -I$SPLICE_HOME/include/dcps/C++/CCPP/OpenFusion \
	-I$SPLICE_HOME/include/dcps/C++/CCPP

echo "Compiling Application Foo Types support code"
g++ -g -DDDS_OpenFusion_1_4_1 -c pingpongDcps_impl.cpp \
	-I./ -I$TAO_ROOT/include -I$SPLICE_HOME/include/dcps/C++/CCPP/OpenFusion \
	-I$SPLICE_HOME/include/dcps/C++/CCPP

echo "Compiling PING"
g++ -g -DDDS_OpenFusion_1_4_1 -c ../ping.cpp \
	-I./ -I$TAO_ROOT/include -I$SPLICE_HOME/include/dcps/C++/CCPP/OpenFusion \
	-I$SPLICE_HOME/include/dcps/C++/CCPP

echo "Compiling PONG"
g++ -g -DDDS_OpenFusion_1_4_1 -c ../pong.cpp \
	-I./ -I$TAO_ROOT/include -I$SPLICE_HOME/include/dcps/C++/CCPP/OpenFusion \
	-I$SPLICE_HOME/include/dcps/C++/CCPP

echo "Linking PING"
g++ -o ../exec/ping -L$SPLICE_HOME/lib ping.o pingpongC.o pingpongDcpsC.o pingpongDcps_impl.o pingpongSplDcps.o -L$TAO_ROOT/lib -lTAO -ldcpsccpp -lstdc++

echo "Linking PONG"
g++ -o ../exec/pong -L$SPLICE_HOME/lib pong.o pingpongC.o pingpongDcpsC.o pingpongDcps_impl.o pingpongSplDcps.o -L$TAO_ROOT/lib -lTAO -ldcpsccpp -lstdc++

cd ..
