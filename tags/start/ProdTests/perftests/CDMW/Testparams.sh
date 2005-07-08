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

# Testparams.sh: parameters definitons for tests suite.
#
# ACRONYMS list
# RD	Result Data
# PS	Parameter Space
# CR	Call and Return (synchronous)
# OW	One Way (asynch)

FILEPARA="client.cfg"                   # configuration data read by the client
FILEPARA_SERVER="server.cfg"            # configuration data read by the server (only for Dispatcher bench.)
CPUmon="CPUmonitor.mon"                 # CPU data file produced by the test suite
RAMmon="RAMmonitor.mon"                 # RAM data file produced by the test suite
EXETEST1H="start_single_test1H.sh"      # 1 host: script executing the test for 1 point in the PS
EXETEST2H="start_single_test2H.sh"      # 2 host: script executing the test for 1 point in the PS
RES_FIX=".dat"                          # suffix of RD files (used for moving RD files in the proper dir. when the test is done)

if  test -z $PERFTESTS_HOME
then
    echo "Please, first execute (or edit) the 'env.sh' script"
    echo "before launch other tests suite scripts!"
    exit -1
fi

PREFSCRIPTNAM="runtest-"		# prefix of simbolic links calling this script

MONITOR_DELAY=1				# delay=1sec (for monitoring CPU and RAM)
TEST_DELAY=10				# delay=10sec (between the test run for a sinlge point in the PS and the next one)

######## REMOTE HOST DEFINITIONS ############
REMOTEHOST=cardamom09
REMOTETOPDIR=				# remote host directory (at the moment must be empty: is assumed equal to local one)
REMOTE_MONPREF="REMOTE_"		# prefix to be assigned to monitor files when copied locally (to avoid name conflicts)

# define parameters' range
NSTART=10				# warm-up stage samples (to be discarded when saving result data)
NITER=2000				# number of iterations (note: this is a limitation - same number of samples for each kind of test)


DELAYS="0"				# delays between invocations (msec); used in Invocation, InvocationRT, Marshalling
CCM_DELAYS="0"				# delays between invocations (msec); used in Marshalling CCM
TIMER_DELAYS="10 50 100 500"            # period for Periodic Timer (msec); used in Timer Invocation

##TYPES="CR OW OW_TRANSPORT OW_SERVER OW_TARGET"		# Invocation policies (CR and OW requests); used in Invocation, Marshalling
TYPES="CR OW"		# Invocation policies (CR and OW requests); used in Invocation, Marshalling
CCM_TYPES="CR OW"		# Invocation policies (CR and OW requests); used Marshalling CCM

#DATATYPES="LONG FLOAT ANY RECORD"	# data types for Marshalling bench.
DATATYPES="ANY RECORD"	# data types for Marshalling bench.

#DATASIZ8="8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536"	# data size for Marshalling bench.
#DATASIZ4="4 $DATASIZ8"			# used for long and float data types (see above)
DATASIZ8="32768 65536"	# data size for Marshalling bench.
DATASIZ4="$DATASIZ8"			# used for long and float data types (see above)

NSERVERS="2 3 4 5 6 7 8 9 10 12 14 16 18 20 25 30"		# number of servants (used for Dispatcher bench.)

NITER_PARALLELISM=5000			# number of iterations (note: only for Parallelism bench.)
NITER_TIMER=5000			# number of iterations (note: only for Timer Invocation and Timer
                                        # Throughput benchmarks)
NTHREADS="1 2 3 4 5 6 7 8 9 10 12 14 16 18 20 25 30"		# number of threads (used for Parallelism bench.)
FILE_PREFIX=""				# prefix for RD files (note: must be empty for compatibility with R scripts)
##############

#### TEST TYPES: these definitions are used everywhere!
# name of functions called by this script (one for each kind of test)
TESTYP="Invocation"                    # A-010 (TESTDIR), A-020 (TESTDIR2)
TESTYP="$TESTYP InvocationRT"
TESTYP="$TESTYP Marshalling"
TESTYP="$TESTYP Dispatcher"
TESTYP="$TESTYP Parallelism"
TESTYP="$TESTYP Marshalling_CCMServer" 
TESTYP="$TESTYP TimeInvocation"
TESTYP="$TESTYP TimeParallelism"
TESTYP="$TESTYP TimerInvocation"       # C-090 (TESTDIR)
TESTYP="$TESTYP TimerTroughput"        # C-100 (TESTDIR)
TESTYP="$TESTYP Invocation_CPU"        # A-011 (TESTDIR), A-021 (TESTDIR2)
TESTYP="$TESTYP InvocationCollocated"  # A-012 (TESTDIR)
TESTYP="$TESTYP InvocationCCM"         # B-010 (TESTDIR), B-020 (TESTDIR2)
TESTYP="$TESTYP InvocationCCM_CPU"     # B-011 (TESTDIR), B-021 (TESTDIR2)


TESTDIR="A-010"	# names of output directories for RD files (1 host)
TESTDIR="$TESTDIR A-015"
TESTDIR="$TESTDIR A-030"
TESTDIR="$TESTDIR A-050"
TESTDIR="$TESTDIR A-070"
TESTDIR="$TESTDIR B-030"
TESTDIR="$TESTDIR C-010"
TESTDIR="$TESTDIR C-030"
TESTDIR="$TESTDIR C-090"
TESTDIR="$TESTDIR C-100"
TESTDIR="$TESTDIR A-011"
TESTDIR="$TESTDIR A-012"
TESTDIR="$TESTDIR B-010"
TESTDIR="$TESTDIR B-011"

TESTDIR2="A-020" # names of output directories for RD files (2 hosts)
TESTDIR2="$TESTDIR2 A-025"
TESTDIR2="$TESTDIR2 A-040"
TESTDIR2="$TESTDIR2 A-060"
TESTDIR2="$TESTDIR2 A-080"
TESTDIR2="$TESTDIR2 B-040"
TESTDIR2="$TESTDIR2 C-020"
TESTDIR2="$TESTDIR2 C-040"
TESTDIR2="$TESTDIR2 NONE" # The test is for only 1 Host   
TESTDIR2="$TESTDIR2 NONE" # The test is for only 1 Host
TESTDIR2="$TESTDIR2 A-021"
TESTDIR2="$TESTDIR2 NONE" # The test is for only 1 Host
TESTDIR2="$TESTDIR2 B-020"
TESTDIR2="$TESTDIR2 B-021"

declare -a testtyps=($TESTYP) # list converted to array
declare -a testdirs=($TESTDIR)
declare -a testdirs2=($TESTDIR2)

NTYP=${#testtyps[@]}
NDIR=${#testdirs[@]}
NDIR2=${#testdirs2[@]}			# check arrays' size compatibility
test $NTYP -ne $NDIR && echo badly configured && exit
test $NTYP -ne $NDIR2 && echo badly configured && exit
###############################


