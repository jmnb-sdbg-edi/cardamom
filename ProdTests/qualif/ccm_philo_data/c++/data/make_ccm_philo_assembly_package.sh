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

PACK_TMP_DIR=/tmp/_DinnerPack_tmp


# clean tmp directory
rm -fr $PACK_TMP_DIR
mkdir $PACK_TMP_DIR
mkdir $PACK_TMP_DIR/meta-inf


# copy xml files
echo Copy descriptors...
cp @config.install.dir@/share/test_ccm_philo_c++/Dinner.cad $PACK_TMP_DIR/meta-inf
cp @config.install.dir@/share/test_ccm_philo_c++/*.* $PACK_TMP_DIR


# compy idl file
echo Copy idl...
cp @config.install.dir@/idl/Philo.idl $PACK_TMP_DIR

# make links for libraries
ln -s @config.install.dir@/lib/c++/libcdmwObserver.so $PACK_TMP_DIR/libcdmwObserver.so
ln -s @config.install.dir@/lib/c++/libcdmwPhilosopher.so $PACK_TMP_DIR/libcdmwPhilosopher.so
ln -s @config.install.dir@/lib/c++/libcdmwRegistrator.so $PACK_TMP_DIR/libcdmwRegistrator.so
ln -s @config.install.dir@/lib/c++/libcdmwForkManager.so $PACK_TMP_DIR/libcdmwForkManager.so

echo Create assembly archive

OLD_DIR=`pwd`
cd $PACK_TMP_DIR
zip -r $OLD_DIR/Dinner.aar *
cd $OLD_DIR

rm -fr $PACK_TMP_DIR

