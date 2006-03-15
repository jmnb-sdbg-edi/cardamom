#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI. All rights reserved.
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

CUR_DIR=`pwd`
PREFIX=run_performance_test_
SCRIPTNAME=`basename $0 .sh`
TESTNAME=${SCRIPTNAME##$PREFIX}
ln -sfv ../../../runTest1H.sh $CUR_DIR/runtest-$TESTNAME.sh
./runtest-$TESTNAME.sh -rd ../../../.. -name results -ts $TESTNAME-results
rm -f $CUR_DIR/runtest-$TESTNAME.sh
