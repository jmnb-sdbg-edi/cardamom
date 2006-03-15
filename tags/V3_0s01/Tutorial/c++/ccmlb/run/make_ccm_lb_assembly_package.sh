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


if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi

SITE_CONFIG=../../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../../configure.sh' and restart $0 "
    exit 1
fi

# CARDAMOM installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"

HOST_TYPE=`$CDMW_HOME/bin/config.guess`


PACK_TMP_DIR=/tmp/_CcmLbPack_tmp
CURRENT_DIR=`pwd`
PARENT_DIR=`dirname $CURRENT_DIR`

# clean tmp directory
rm -fr $PACK_TMP_DIR
mkdir $PACK_TMP_DIR
mkdir $PACK_TMP_DIR/meta-inf


# copy xml files
echo Copy descriptors...
    cp $PARENT_DIR/data/QualifCcmLb.cad $PACK_TMP_DIR/meta-inf
    cp $PARENT_DIR/generated/*.ccd $PACK_TMP_DIR
    cp $PARENT_DIR/data/*.csd $PACK_TMP_DIR




# compy idl file
echo Copy idl...
    cp $PARENT_DIR/idl/ccm_lb.idl $PACK_TMP_DIR

# make links for libraries
ln -s $PARENT_DIR/$HOST_TYPE/libcdmwQualifCcmLbServer.so $PACK_TMP_DIR/libcdmwQualifCcmLbServer.so
ln -s $PARENT_DIR/$HOST_TYPE/libcdmwQualifCcmLbClient.so $PACK_TMP_DIR/libcdmwQualifCcmLbClient.so


echo Create assembly archive
    cd $PACK_TMP_DIR && zip -r $CURRENT_DIR/QualifCcmLb.aar *


# clean up
rm -rf $PACK_TMP_DIR
