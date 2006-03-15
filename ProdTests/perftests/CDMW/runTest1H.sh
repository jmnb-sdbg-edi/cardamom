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
#!/bin/sh
#
# runTest1H.sh 
#
#

cd `dirname $0`

thisdir=$PWD
topdir=$thisdir/../../..
rscriptdir=$topdir/../RScripts
source $topdir/Testparams.sh

function scriptnam2typ1H(){

    SCRIPTNAM=$1
    TESTDIR=${SCRIPTNAM##$PREFSCRIPTNAM}
    TESTNAM=
    #### check for test name ############
    for (( i=0;i<$NTYP;i++)); do
	test $TESTDIR == ${testdirs[$i]} && TESTNAM=${testtyps[$i]}
    done
    
    echo TEST TYPE is $TESTNAM ...
    test -z "$TESTNAM" && echo "not in myname" && exit
}


function cleanres(){
    echo rm -f *$RES_FIX
    rm -f *$RES_FIX
}

function cpall1H(){
    local resdir=$1
    echo "putting results in $resdir"
    mv *$RES_FIX  $resdir
    cp $FILEPARA  $resdir
}


function start_test1H(){
    echo start_test1H
    local outdir=$1
    local delay=$2

    $MONITOR start  $outdir $delay
    OKTEST=0
    ${thisdir}/$EXETEST1H
    OKTEST=$?

    $MONITOR stop $outdir $delay
}


function printUsage(){
    echo
    echo "Usage: $0 -rd [path] -name [testname] -ts [timestamp]"
}

### input params: ###
function readParams(){
    #command line prefixs
    CMDL_FREFDIR="-rd"
    CMDL_TESTDIR="-name"
    CMDL_SUBDIR="-ts"
    CMDL_HELP="-h"

    declare -a arr=($@)
    for (( ipar=0; ipar< $#; ipar++ )); do
        test ${arr[$ipar]} == $CMDL_FREFDIR && PREFDIR=${arr[$(($ipar +1))]}
        test ${arr[$ipar]} == $CMDL_TESTDIR && TESTDIR=${arr[$(($ipar +1))]}
        test ${arr[$ipar]} == $CMDL_SUBDIR &&  SUBDIR=${arr[$(($ipar +1))]}
        test ${arr[$ipar]} == $CMDL_HELP && printUsage && exit
    done
    RESTOPDIR=$PREFDIR/$TESTDIR/$SUBDIR
    echo RESTOPDIR is $RESTOPDIR
}

###############################################################################

function InvocationLB_writecfg(){
    echo writecfg $NITER_LB $1
    local delay=$1
    cat > $FILEPARA <<EOF
$NITER_LB
$delay
EOF
}

function NotificationSMG_writecfg(){
    echo writecfg $1
    local niter=$1

    cat > $FILEPARA <<EOF
$niter
EOF
}

function Invocation_writecfg(){
    echo writecfg $1 $2 $3
    local delay=$1
    local typ=$2
    local measuretyp=$3

    cat > $FILEPARA <<EOF
$NITER
$delay
$typ
NONE
0
$measuretyp
$FILE_PREFIX
EOF
}



function InvocationRT_writecfg(){
    echo writecfg $delay
    cat > $FILEPARA <<EOF
$NITER
$delay
$FILE_PREFIX
EOF
}


function Marshalling_writecfg(){
    echo writecfg $delay $typ $datatyp $datasiz $measuretyp
    cat > $FILEPARA <<EOF
$NITER
$delay
$typ
$datatyp
$datasiz
$measuretyp
$FILE_PREFIX
EOF
}

function Marshalling_CCM_writecfg(){
    echo writecfg $delay $typ $datatyp $datasiz 
    cat > $FILEPARA <<EOF
$NITER
$delay
$typ
$datatyp
$datasiz
$FILE_PREFIX
EOF
}

function InvocationCCM_writecfg(){
    echo writecfg $delay $typ
    local measuretyp=$1
    cat > $FILEPARA <<EOF
$NITER
$delay
$typ
NONE
0
$measuretyp
$FILE_PREFIX
EOF
}

function DispatcherC_writecfg(){
    echo writecfg CLIENT
    cat > $FILEPARA <<EOF
$NITER
$FILE_PREFIX
EOF
}

function DispatcherS_writecfg(){
    echo writecfg $nserver
    echo $nserver > $FILEPARA_SERVER
}




function Parallelism_writecfg(){
    echo writecfg $nthread
    cat > $FILEPARA <<EOF
$NITER_PARALLELISM
$nthread
$FILE_PREFIX
EOF
}

function TimeInvocation_writecfg(){
    echo writecfg $1
    local delay=$1

    cat > $FILEPARA <<EOF
$NITER
$delay
$FILE_PREFIX
EOF
}

function TimeParallelism_writecfg(){
    echo writecfg $nthread
    cat > $FILEPARA <<EOF
$NSTART
$NITER_PARALLELISM
$nthread
$FILE_PREFIX
EOF
}

function TimerInvocation_writecfg(){
    echo writecfg $1 $2
    local delay=$1
    local measuretyp=$2

    cat > $FILEPARA <<EOF
$NITER_TIMER
$delay
$measuretyp
$FILE_PREFIX
EOF
}

function ControlledInvocation_writecfg(){
    echo writecfg $1
    local delay=$1

    cat > $FILEPARA <<EOF
$NITER
$delay
$FILE_PREFIX
EOF
}

function TimerPrecision_writecfg(){
    echo writecfg $1
    local delay=$1

    cat > $FILEPARA <<EOF
$NITER_TIMER
$DELTAT_TIMER
$delay
$FILE_PREFIX
EOF
}

function TimerTroughput_writecfg(){

   local deltat=$1
   cat > $FILEPARA <<EOF
$NITER_TIMER
$deltat
$FILE_PREFIX
EOF
}

###############################################################################

function do_InvocationLB(){
    for delay in $DELAYS; do
            SUBSUBDIR="CR-${delay}"
            #RESDIR=$RESTOPDIR
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            InvocationLB_writecfg ${delay}
            start_test1H $RESDIR $mondelay

            cpall1H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_NotificationSMG(){
            SUBSUBDIR="${smg-exec}"
            #RESDIR=$RESTOPDIR/$SUBSUBDIR
            RESDIR=$RESTOPDIR
            mkdir -p $RESDIR

            NotificationSMG_writecfg $NITER_SMG
            start_test1H $RESDIR $mondelay

            cpall1H $RESDIR
            sleep $TEST_DELAY
}

function do_Invocation(){
    for delay in $DELAYS; do
	for typ in $TYPES; do
            SUBSUBDIR="${typ}-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            Invocation_writecfg $delay $typ TIME_HR
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
        done
    done
}


function do_Invocation_CPU(){
    for delay in $DELAYS; do
	for typ in $TYPES; do
            SUBSUBDIR="${typ}-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            Invocation_writecfg $delay $typ TIME_TK
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
        done
    done
}

# Collocated Invocation for A-012 (CR, T = 0,10,100,500)
function do_InvocationCollocated(){

    for delay in $COLL_DELAYS; do
	for typ in $COLL_TYPES; do
            SUBSUBDIR="COLLOCATED_${typ}-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            Invocation_writecfg $delay $typ TIME_HR
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
        done
    done
    do_InvocationCollocated_step2 # execute other step for A-012 test
}

# Simple Invocation test for A-012 (CR, T = 0,10,100,500)
function do_InvocationCollocated_step2(){
    local CURDIR=`pwd` # current A-012 directory
    local INVDIR=$topdir/CDMW_Marshalling/c++/run #directory for invocation test A-010
    cd $INVDIR
    for delay in $COLL_DELAYS; do
	for typ in $COLL_TYPES; do
            SUBSUBDIR="${typ}-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            Invocation_writecfg $delay $typ TIME_HR
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
        done
    done
    cd $CURDIR # restore A-012 directory
}

function do_InvocationRT(){
    for delay in $DELAYS; do
        SUBSUBDIR="RT-${delay}"
        RESDIR=$RESTOPDIR/$SUBSUBDIR
        mkdir -p $RESDIR

        InvocationRT_writecfg
        start_test1H $RESDIR $mondelay

        # test run not OK
        if [ $OKTEST == 129 ]; then
          echo "Removing previous data..."
          rm -rf $RESDIR/*
          echo "Running the same test..."
	  start_test1H $RESDIR $mondelay
	fi

        cpall1H $RESDIR
	sleep $TEST_DELAY

    done
}



function do_Marshalling(){
    for delay in $DELAYS; do
	for typ in $TYPES; do
	    for datatyp in $DATATYPES; do
		if [ $datatyp == "LONG" ] || [ $datatyp == "FLOAT" ]; then
		    DATASIZES=$DATASIZ4
		else
		    DATASIZES=$DATASIZ8
		fi
		for datasiz in $DATASIZES; do
		    SUBSUBDIR="${typ}-${delay}-${datatyp}-${datasiz}"
		    RESDIR=$RESTOPDIR/$SUBSUBDIR
		    mkdir -p $RESDIR

		    Marshalling_writecfg
		    start_test1H $RESDIR $mondelay

                    # test run not OK
		    if [ $OKTEST == 129 ]; then
			echo "Removing previous data..."
			rm -rf $RESDIR/*
			echo "Running the same test..."
			start_test1H $RESDIR $mondelay
		    fi

		    
		    cpall1H $RESDIR
		    sleep $TEST_DELAY
		done
	    done
	done
    done
}


function do_Marshalling_CCM(){
    for delay in $CCM_DELAYS; do
	for typ in $CCM_TYPES; do
	    for datatyp in $DATATYPES; do
		if [ $datatyp == "LONG" ] || [ $datatyp == "FLOAT" ]; then
		    DATASIZES=$DATASIZ4
		else
		    DATASIZES=$DATASIZ8
		fi
		for datasiz in $DATASIZES; do
		    SUBSUBDIR="${typ}-${delay}-${datatyp}-${datasiz}"
		    RESDIR=$RESTOPDIR/$SUBSUBDIR
		    mkdir -p $RESDIR

		    Marshalling_CCM_writecfg
		    start_test1H $RESDIR $mondelay

                    # test run not OK
		    if [ $OKTEST == 129 ]; then
			echo "Removing previous data..."
			rm -rf $RESDIR/*
			echo "Running the same test..."
			start_test1H $RESDIR $mondelay
		    fi

		    
		    cpall1H $RESDIR
		    sleep $TEST_DELAY
		done
	    done
	done
    done
}

function do_InvocationCCM(){
    for delay in $CCM_DELAYS; do
        for typ in $CCM_TYPES; do
	    SUBSUBDIR="${typ}-${delay}"
	    RESDIR=$RESTOPDIR/$SUBSUBDIR
	    mkdir -p $RESDIR

            InvocationCCM_writecfg TIME_HR
	    start_test1H $RESDIR $mondelay
             # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
		    
	    cpall1H $RESDIR
	    sleep $TEST_DELAY
	done
    done
}

function do_InvocationCCM_CPU(){
    for delay in $CCM_DELAYS; do
        for typ in $CCM_TYPES; do
	    SUBSUBDIR="${typ}-${delay}"
	    RESDIR=$RESTOPDIR/$SUBSUBDIR
	    mkdir -p $RESDIR

            InvocationCCM_writecfg TIME_TK
	    start_test1H $RESDIR $mondelay
             # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
		    
	    cpall1H $RESDIR
	    sleep $TEST_DELAY
	done
    done
}

function do_Dispatcher(){

    DispatcherC_writecfg
    for nserver in $NSERVERS; do
        SUBSUBDIR="NSRV$nserver"
        RESDIR=$RESTOPDIR/$SUBSUBDIR
        mkdir -p $RESDIR

        DispatcherS_writecfg
        start_test1H $RESDIR $mondelay

        # test run not OK
        if [ $OKTEST == 129 ]; then
          echo "Removing previous data..."
          rm -rf $RESDIR/*
          echo "Running the same test..."
	  start_test1H $RESDIR $mondelay
	fi


        cpall1H $RESDIR
        cp $FILEPARA_SERVER $RESDIR
	sleep $TEST_DELAY
    done
}

function do_Parallelism(){
    for nthread in $NTHREADS; do
        echo $nthread
        SUBSUBDIR="NTHR$nthread"
        RESDIR=$RESTOPDIR/$SUBSUBDIR
        mkdir -p $RESDIR

        Parallelism_writecfg
        start_test1H $RESDIR $mondelay

        # test run not OK
        if [ $OKTEST == 129 ]; then
          echo "Removing previous data..."
          rm -rf $RESDIR/*
          echo "Running the same test..."
	  start_test1H $RESDIR $mondelay
	fi

	cpall1H $RESDIR
	sleep $TEST_DELAY
    done
}

function do_TimeInvocation(){
    for delay in $DELAYS; do
            SUBSUBDIR="CR-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimeInvocation_writecfg $delay
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_TimeParallelism(){
    for nthread in $NTHREADS; do
        echo $nthread
        SUBSUBDIR="NTHR$nthread"
        RESDIR=$RESTOPDIR/$SUBSUBDIR
        mkdir -p $RESDIR

        TimeParallelism_writecfg
        start_test1H $RESDIR $mondelay

        # test run not OK
        if [ $OKTEST == 129 ]; then
          echo "Removing previous data..."
          rm -rf $RESDIR/*
          echo "Running the same test..."
	  start_test1H $RESDIR $mondelay
	fi

	cpall1H $RESDIR
	sleep $TEST_DELAY
    done
}

function do_ControlledInvocation(){
    for delay in $DELAYS; do
            SUBSUBDIR="CR-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            ControlledInvocation_writecfg $delay
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_TimerInvocation(){
    for delay in $TIMER_DELAYS; do
            SUBSUBDIR="CR-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimerInvocation_writecfg $delay TIME_HR
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_TimerPrecision(){
            #SUBSUBDIR="CR-${delay}"
            #RESDIR=$RESTOPDIR/$SUBSUBDIR
	    RESDIR=$RESTOPDIR
            mkdir -p $RESDIR

            TimerPrecision_writecfg $DELAY_TIMER
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
}

function do_TimerThroughput(){
            SUBSUBDIR="THR-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimerTroughput_writecfg $DELTAT_TIMER
            start_test1H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test1H $RESDIR $mondelay
	    fi
            cpall1H $RESDIR
            sleep $TEST_DELAY
}

###############################################################################

# Script R functions
function cpscript_InvocationLB(){
  cp $rscriptdir/D-010.R $RESTOPDIR 
}

function cpscript_NotificationSMG(){
  cp $rscriptdir/E-010.R $RESTOPDIR 
}

function cpscript_Invocation(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/A-010.R $RESTOPDIR 
}

function cpscript_InvocationRT(){
  cp $rscriptdir/InvocationRT1H.R $RESTOPDIR
  cp $rscriptdir/InvocationRTCore.R $RESTOPDIR
}

function cpscript_Marshalling(){
  cp $rscriptdir/Marshalling*1H.R $RESTOPDIR
  cp $rscriptdir/MarshallingCore.R $RESTOPDIR
}

function cpscript_Marshalling_CCM(){
  cp $rscriptdir/Marshalling*1HCCM.R $RESTOPDIR
  cp $rscriptdir/MarshallingCoreCCM.R $RESTOPDIR
}

function cpscript_Dispatcher(){
  cp $rscriptdir/Dispatcher1H.R $RESTOPDIR
  cp $rscriptdir/DispatcherCore.R $RESTOPDIR
}

function cpscript_Parallelism(){
  cp $rscriptdir/Parallelism1H.R $RESTOPDIR
  cp $rscriptdir/ParallelismCore.R $RESTOPDIR
}

function cpscript_Invocation_CPU(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/A-011.R $RESTOPDIR 
}

function cpscript_InvocationCollocated(){
  cp $rscriptdir/A-012.R $RESTOPDIR 
}

function cpscript_TimeInvocation(){
  cp $rscriptdir/C-010.R $RESTOPDIR 
  cp $rscriptdir/CR0_CompCore.R $RESTOPDIR
}

function cpscript_ControlledInvocation(){
  cp $rscriptdir/C-015.R $RESTOPDIR 
  cp $rscriptdir/CR0_CompCore.R $RESTOPDIR
}

function cpscript_TimeParallelism(){
  cp $rscriptdir/C-030.R $RESTOPDIR 
  cp $rscriptdir/CR0_CompCore.R $RESTOPDIR
}

function cpscript_TimerPrecision(){
  cp $rscriptdir/C-090.R $RESTOPDIR 
}

function cpscript_TimerTroughput(){
  cp $rscriptdir/C-100.R $RESTOPDIR 
}

function cpscript_TimerInvocation(){
  cp $rscriptdir/C-050.R $RESTOPDIR 
}

function cpscript_InvocationCCM(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/B-010.R $RESTOPDIR 
}

function cpscript_InvocationCCM_CPU(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/B-011.R $RESTOPDIR 
}

################   MAIN #########################

MONITOR="$topdir/Monitor.sh"
datecmd="date +%Y%m%d_%H%M"
PREFDIR="."
SUBDIR=`$datecmd`
mondelay=$MONITOR_DELAY


scriptnam2typ1H `basename $0 .sh` 
readParams $@
cleanres

echo Starting test...
do_$TESTNAM

echo Copying R script files...
cpscript_$TESTNAM

echo Moving log files...
mv *.log $RESTOPDIR

echo
echo END TEST $TESTNAM
echo
