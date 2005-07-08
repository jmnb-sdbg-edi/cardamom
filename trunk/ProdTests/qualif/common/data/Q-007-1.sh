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
echo "=           C++ Code Generation Validation Test         ="
echo "========================================================="
echo ""

VALIDATION_HOME=@config.install.dir@/share
RESULTS_DIR=$VALIDATION_HOME/Q-007-1-results

if [ -d "$RESULTS_DIR" ]; then
   echo "Cleanup results directory"
   rm -rf $RESULTS_DIR
fi
mkdir -p $RESULTS_DIR

cd $VALIDATION_HOME

echo "===== Executing code generation ====="

cdmw_code_generator.sh \
   --xml=$VALIDATION_HOME/Q-007-data/Q-007-1.xml \
   --dir=$RESULTS_DIR \
   --lang=cpp --validate

echo "===== Check the created file ====="

file_list="$RESULTS_DIR/CdmwInterface1.cpp \
$RESULTS_DIR/CdmwInterface2.cpp \
$RESULTS_DIR/process1.dtd \
$RESULTS_DIR/process2.dtd \
$RESULTS_DIR/ResourceServantProvider1.hpp \
$RESULTS_DIR/ResourceServantProvider2.hpp \
$RESULTS_DIR/ResourceServantProvider1.cpp \
$RESULTS_DIR/ResourceServantProvider2.cpp \
$RESULTS_DIR/ServiceResourceFactory1_impl.hpp \
$RESULTS_DIR/ServiceResourceFactory2_impl.hpp \
$RESULTS_DIR/ServiceResourceFactory1_impl.cpp \
$RESULTS_DIR/ServiceResourceFactory2_impl.cpp \
$RESULTS_DIR/ServiceResourceFactory1_impl_allocator.hpp \
$RESULTS_DIR/ServiceResourceFactory2_impl_allocator.hpp \
$RESULTS_DIR/SessionResourceFactory1_impl_allocator.hpp \
$RESULTS_DIR/SessionResourceFactory2_impl_allocator.hpp \
$RESULTS_DIR/SessionResourceFactory1_impl.hpp \
$RESULTS_DIR/SessionResourceFactory2_impl.hpp \
$RESULTS_DIR/SessionResourceFactory1_impl.cpp \
$RESULTS_DIR/SessionResourceFactory2_impl.cpp \
$RESULTS_DIR/UnMngEntityResourceFactory1_impl.cpp \
$RESULTS_DIR/UnMngEntityResourceFactory2_impl.cpp \
$RESULTS_DIR/UnMngEntityResourceFactory1_impl.hpp \
$RESULTS_DIR/UnMngEntityResourceFactory2_impl.hpp \
$RESULTS_DIR/UnMngEntityResourceFactory1_impl_allocator.hpp \
$RESULTS_DIR/UnMngEntityResourceFactory2_impl_allocator.hpp \
$RESULTS_DIR/EntityResourceFactory1_impl.hpp \
$RESULTS_DIR/EntityResourceFactory2_impl.hpp \
$RESULTS_DIR/EntityResourceFactory1_impl.cpp \
$RESULTS_DIR/EntityResourceFactory2_impl.cpp \
$RESULTS_DIR/EntityResourceFactory1_impl_allocator.hpp \
$RESULTS_DIR/EntityResourceFactory2_impl_allocator.hpp"

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
