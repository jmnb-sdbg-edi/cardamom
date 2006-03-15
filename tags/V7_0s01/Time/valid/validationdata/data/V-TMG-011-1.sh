#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. All rights reserved.
# 
# Copyright (C) SELEX-SI 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

echo "========================================================="
echo "=      CCM C++ Code Generation Qualification Test       ="
echo "========================================================="
echo ""

VALIDATION_HOME=@config.install.dir@/share
RESULTS_DIR=$VALIDATION_HOME/V-TMG-011-1-results

if [ -d "$RESULTS_DIR" ]; then
   echo "Cleanup results directory"
   rm -rf $RESULTS_DIR
fi
mkdir -p $RESULTS_DIR

cd $VALIDATION_HOME

cp -f V-TMG-011-data/V-TMG-011-1.xml

echo "===== Executing code generation ====="

cdmw_code_generator.sh \
   --xml=$VALIDATION_HOME/V-TMG-011-data/V-TMG-011-1.xml \
   --dir=$RESULTS_DIR \
   --lang=cpp --validate

echo "===== Check the created file ====="

file_list="$RESULTS_DIR/SessionClientHome_impl.cpp \
$RESULTS_DIR/SessionServerHome_impl.cpp \
$RESULTS_DIR/SessionClientHome_impl.hpp \
$RESULTS_DIR/SessionServerHome_impl.hpp \
$RESULTS_DIR/CCM_ClockClient_SessionContext_impl.cpp \
$RESULTS_DIR/CCM_ClockServer_SessionContext_impl.cpp \
$RESULTS_DIR/SessionClockClient_impl.cpp \
$RESULTS_DIR/SessionClockServer_impl.cpp \
$RESULTS_DIR/CCM_ClockClient_SessionContext_impl.hpp \
$RESULTS_DIR/CCM_ClockServer_SessionContext_impl.hpp \
$RESULTS_DIR/SessionClockClient_impl.hpp \
$RESULTS_DIR/SessionClockServer_impl.hpp \
$RESULTS_DIR/ClockCatalog_impl.cpp \
$RESULTS_DIR/ClockClient.cpf \
$RESULTS_DIR/ClientHome.cpf \
$RESULTS_DIR/ClockServer.cpf \
$RESULTS_DIR/ServerHome.cpf \
$RESULTS_DIR/ClientHome.ccd \
$RESULTS_DIR/ServerHome.ccd \
$RESULTS_DIR/ClockCatalog_impl.hpp"



all_is_generated="true"

for file in $file_list
do
if test -f $file
then
    echo "the file $file is generated"
else
    echo "##### the file $file is not generated #####"
    all_is_generated=false
fi
done
echo ""

echo "===== Global test result ====="

if test "$all_is_generated" = false
then
    echo "    The test is NOT OK"
else
    echo "    The test is OK"
fi
