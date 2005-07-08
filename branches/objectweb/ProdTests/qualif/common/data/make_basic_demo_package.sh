#!/bin/sh
#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI.
#* 
#* It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
#* All rights reserved.
#* 
#* CARDAMOM is free software; you can redistribute it and/or modify it under
#* the terms of the GNU Library General Public License as published by the
#* Free Software Foundation; either version 2 of the License, or (at your
#* option) any later version.
#* 
#* CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
#* License for more details.
#* 
#* You should have received a copy of the GNU Library General
#* Public License along with CARDAMOM; see the file COPYING. If not, write to
#* the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#* =========================================================================== *

# Is CDMW_RUNTIME_ROOT set?
if [ "$CDMW_RUNTIME_ROOT" = "" ]; then
    echo "You must set the CDMW_RUNTIME_ROOT variable before any work !"
    echo "This variable must contain your root directory of the CDMW CSCs"
    exit 1
fi


HOST_TYPE=`$CDMW_RUNTIME_ROOT/bin/config.guess`
ORB_CPP="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_CPP |cut -d= -f2`"
COMPILER_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_NAME |cut -d= -f2`"
COMPILER_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_VER |cut -d= -f2`"
BINARY_TYPE=$HOST_TYPE-$COMPILER_NAME-$COMPILER_VER

PACK_TMP_DIR=/tmp/`$CDMW_RUNTIME_ROOT/bin/whoami.sh`_BasicDemoPack_tmp
BASIC_DEMO_DIR=$CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration
VALIDATION_DATA_DIR=$CDMW_RUNTIME_ROOT/validation/data


# clean tmp directory
rm -fr $PACK_TMP_DIR
mkdir $PACK_TMP_DIR
mkdir $PACK_TMP_DIR/meta-inf

# copy xml files
echo Copy descriptors...
cp $VALIDATION_DATA_DIR/Q-028-data/BasicDemo.cad $PACK_TMP_DIR/meta-inf
cp $BASIC_DEMO_DIR/data/*.cpf $PACK_TMP_DIR
cp $BASIC_DEMO_DIR/clientcomponent/data/*.* $PACK_TMP_DIR
cp $BASIC_DEMO_DIR/servercomponent/data/*.* $PACK_TMP_DIR

# copy idl file
echo Copy idl...
cp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/idl/Basic.idl $PACK_TMP_DIR

# make links for libraries
ln -s  $BASIC_DEMO_DIR/clientcomponent/prod/$ORB_CPP/$BINARY_TYPE/libClientBasicDemo.so $PACK_TMP_DIR/libClientBasicDemo.so
ln -s $BASIC_DEMO_DIR/servercomponent/prod/$ORB_CPP/$BINARY_TYPE/libServerBasicDemo.so $PACK_TMP_DIR/libServerBasicDemo.so

echo Create assembly archive

OLD_DIR=`pwd`
cd $PACK_TMP_DIR
zip -r $OLD_DIR/BasicDemo.aar *
cd $OLD_DIR

rm -fr $PACK_TMP_DIR

