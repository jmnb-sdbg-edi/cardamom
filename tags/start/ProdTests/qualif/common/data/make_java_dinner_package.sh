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


ORB_JAVA="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_JAVA |cut -d= -f2`"

PACK_TMP_DIR=/tmp/`$CDMW_RUNTIME_ROOT/bin/whoami.sh`_DinnerPack_tmp
DINNER_DIR=$CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo
VALIDATION_DATA_DIR=$CDMW_RUNTIME_ROOT/validation/data


# clean tmp directory
rm -fr $PACK_TMP_DIR
mkdir $PACK_TMP_DIR
mkdir $PACK_TMP_DIR/meta-inf

# copy xml files
echo Copy descriptors...
cp $VALIDATION_DATA_DIR/Q-029-data/Dinner.cad $PACK_TMP_DIR/meta-inf
cp $DINNER_DIR/observercomponent/data/*.* $PACK_TMP_DIR
cp $DINNER_DIR/philosophercomponent/data/*.* $PACK_TMP_DIR
cp $DINNER_DIR/registratorcomponent/data/*.* $PACK_TMP_DIR
cp $DINNER_DIR/forkmanagercomponent/data/*.* $PACK_TMP_DIR

# copy idl file
echo Copy idl...
cp $CDMW_RUNTIME_ROOT/ccmcif/java/test/idl/Philo.idl $PACK_TMP_DIR

# make links for libraries
ln -s $DINNER_DIR/observercomponent/prod/$ORB_JAVA/cdmwccmciftest.jar $PACK_TMP_DIR/libObserver.jar
ln -s $DINNER_DIR/philosophercomponent/prod/$ORB_JAVA/cdmwccmciftest.jar $PACK_TMP_DIR/libPhilosopher.jar
ln -s $DINNER_DIR/registratorcomponent/prod/$ORB_JAVA/cdmwccmciftest.jar $PACK_TMP_DIR/libRegistrator.jar
ln -s $DINNER_DIR/forkmanagercomponent/prod/$ORB_JAVA/cdmwccmciftest.jar $PACK_TMP_DIR/libForkManager.jar

echo Create assembly archive

OLD_DIR=`pwd`
cd $PACK_TMP_DIR
zip -r $OLD_DIR/JavaDinner.aar *
cd $OLD_DIR

rm -fr $PACK_TMP_DIR

