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

ROOT_DIR=@config.install.dir@
DATA_DIR=$ROOT_DIR/data-to-inspect
REMOTE_HOST=

mkdir -p $DATA_DIR

echo "Installing inspected data for Q-016"

SCENARIO_DATA=Q-016-inspected-data
rm -rf $DATA_DIR/$SCENARIO_DATA
mkdir -p $DATA_DIR/$SCENARIO_DATA

if [ -r $ROOT_DIR/Q-007-1-results/SessionResourceFactory1_impl.cpp ]; then
   cp $ROOT_DIR/Q-007.xml $DATA_DIR/$SCENARIO_DATA
   cp $ROOT_DIR/Q-007-1-results/SessionResourceFactory1_impl.cpp $DATA_DIR/$SCENARIO_DATA
else
   echo "Error : Expected generated files from Q-007-1.sh are missing."
   echo "        Part of inspected data for Q-016 will not be available."
fi

if [ -r $ROOT_DIR/Q-007-3-results/SessionResourceFactory1Impl.java ]; then
   cp $ROOT_DIR/Q-007-3-results/SessionResourceFactory1Impl.java $DATA_DIR/$SCENARIO_DATA
else
   echo "Error : Expected generated files from Q-007-3.sh are missing."
   echo "        Part of inspected data for Q-016 will not be available."
fi

echo "Installation finished"

