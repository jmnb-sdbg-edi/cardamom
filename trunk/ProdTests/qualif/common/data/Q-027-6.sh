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

echo "========================================================="
echo "=     CCM Java Code Generation Qualification Test       ="
echo "========================================================="
echo ""

VALIDATION_HOME=@config.install.dir@/share
RESULTS_DIR=$VALIDATION_HOME/Q-027-2-results

if [ -d "$RESULTS_DIR" ]; then
   echo "Cleanup results directory"
   rm -rf $RESULTS_DIR
fi
mkdir -p $RESULTS_DIR

cd $VALIDATION_HOME

echo "===== Executing code generation ====="

cdmw_code_generator.sh \
   --xml=$VALIDATION_HOME/Q-027-data/Q-027-2.xml \
   --dir=$RESULTS_DIR \
   --lang=java --validate

