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
# runTest2H.sh 
#
#
###### check thisdir and topdir:
cd  `dirname $0`
thisdir=$PWD
cd ../../..
topdir=$PWD
#topdir=$thisdir/../../..
top2this=${thisdir##$topdir}
rscriptdir=$topdir/../RScripts


echo top2this=$top2this
###### check rthisdir and rtopdir:
rtopdir=$topdir
source $topdir/Testparams.sh
test -n "$REMOTETOPDIR" && rtopdir=$REMOTETOPDIR
rthisdir=$rtopdir$top2this

echo thisdir=$thisdir 
echo topdir=$topdir
echo rthisdir=$rthisdir 
echo rtopdir=$rtopdir
echo
######
MONITOR="$topdir/Monitor.sh"
RMONITOR="$rtopdir/Monitor.sh"

datecmd="date +%Y%m%d_%H%M"
PREFDIR="."
SUBDIR=`$datecmd`
mondelay=$MONITOR_DELAY




function scriptnam2typ2H(){

    SCRIPTNAM=$1
    TESTDIR=${SCRIPTNAM##$PREFSCRIPTNAM}
    TESTNAM=
    #### check for test name ############
    for (( i=0;i<$NTYP;i++)); do
	test $TESTDIR == ${testdirs2[$i]} && TESTNAM=${testtyps[$i]}
    done
    
    echo TEST TYPE is $TESTNAM ...
    test -z "$TESTNAM" && echo "not in myname" && exit
}


function cleanres(){
    echo rm -f *$RES_FIX
    rm -f *$RES_FIX
    echo remote rm -f $rthisdir/*$RES_FIX
    rsh $REMOTEHOST rm -f $rthisdir/*$RES_FIX < /dev/null
    echo remote rm -f $rthisdir/*.mon
    rsh $REMOTEHOST rm -f $rthisdir/*.mon  < /dev/null
}

function cpall2H(){
    cd $thisdir
    local resdir=$1
    
    echo "putting remote results in $resdir"
    # Temporary fix - uncomment the following line to enable remote copying of results, RAM and CPU data
    rcp $REMOTEHOST:$rthisdir/*$RES_FIX $resdir
    #rcp $REMOTEHOST:$rthisdir/$CPUmon $resdir/$REMOTE_MONPREF$CPUmon
    #rcp $REMOTEHOST:$rthisdir/$RAMmon $resdir/$REMOTE_MONPREF$RAMmon
    #echo remote rm -f $rthisdir/*$RES_FIX
    #rsh $REMOTEHOST rm -f $rthisdir/*$RES_FIX < /dev/null
    #echo remote rm -f $rthisdir/*.mon
    #rsh $REMOTEHOST rm -f $rthisdir/*.mon  < /dev/null

    ## Overwrite remote Monitor output (*.dat)
    echo "putting local results in $resdir"
    mv *$RES_FIX  $resdir
    cp $FILEPARA  $resdir

}


function start_test2H(){
    echo start_test2H
    local outdir=$1
    local delay=$2

    $MONITOR start  $outdir $delay 
    ## dont ask me why but i must use &
    # Temporary fix - uncomment the following line to enable CPU and RAM monitoring on the remote host
    #rsh $REMOTEHOST $RMONITOR start  $rthisdir $delay &
    
    OKTEST=0
    echo executing ${thisdir}/$EXETEST2H
    ${thisdir}/$EXETEST2H
    OKTEST=$?

    $MONITOR stop $outdir $delay 
    # Temporary fix - uncomment the following line to enable CPU and RAM monitoring on the remote host
    #rsh $REMOTEHOST $RMONITOR stop  $rthisdir $delay < /dev/null
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
    rcp $FILEPARA_SERVER $REMOTEHOST:$rthisdir
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

function ControlledInvocation_writecfg(){
    echo writecfg $1
    local delay=$1

    cat > $FILEPARA <<EOF
$NITER
$delay
$FILE_PREFIX
EOF
}

function TimerInvocation_writecfg(){
    echo writecfg $1
    local delay=$1
    local measuretyp=$2

    cat > $FILEPARA <<EOF
$NITER
$delay
$measuretyp
$FILE_PREFIX
EOF
}

function TimerPrecision_writecfg(){
    echo writecfg $1 $2
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
            start_test2H $RESDIR $mondelay

            cpall2H $RESDIR
            sleep $TEST_DELAY
    done	    
}

function do_NotificationSMG(){
            SUBSUBDIR="${smg-exec}"
            #RESDIR=$RESTOPDIR/$SUBSUBDIR
            RESDIR=$RESTOPDIR
            mkdir -p $RESDIR

            NotificationSMG_writecfg $NITER_SMG
            start_test2H $RESDIR $mondelay

            cpall2H $RESDIR
            sleep $TEST_DELAY
}

function do_Invocation(){
    for delay in $DELAYS; do
	for typ in $TYPES; do
            SUBSUBDIR="${typ}-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            Invocation_writecfg $delay $typ TIME_HR
            start_test2H $RESDIR $mondelay

	    # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
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
            start_test2H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previuos data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
            sleep $TEST_DELAY
        done
    done
}


function do_InvocationRT(){
    for delay in $DELAYS; do
        SUBSUBDIR="RT-${delay}"
        RESDIR=$RESTOPDIR/$SUBSUBDIR
        mkdir -p $RESDIR

        InvocationRT_writecfg
        start_test2H $RESDIR $mondelay

        # test run not OK
	if [ $OKTEST == 129 ]; then
	    echo "Removing previous data..."
	    rm -rf $RESDIR/*
	    echo "Running the same test..."
	    start_test2H $RESDIR $mondelay
	fi
	
        cpall2H $RESDIR
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
		    start_test2H $RESDIR $mondelay

	            # test run not OK
		    if [ $OKTEST == 129 ]; then
			echo "Removing previous data..."
			rm -rf $RESDIR/*
			echo "Running the same test..."
			start_test2H $RESDIR $mondelay
		    fi
		    
		    cpall2H $RESDIR
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
		    start_test2H $RESDIR $mondelay

	            # test run not OK
		    if [ $OKTEST == 129 ]; then
			echo "Removing previous data..."
			rm -rf $RESDIR/*
			echo "Running the same test..."
			start_test2H $RESDIR $mondelay
		    fi
		    
		    cpall2H $RESDIR
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

        InvocationCCM_writecfg $delay $typ TIME_HR
	    start_test2H $RESDIR $mondelay
             # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previuos data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
		    
	    cpall2H $RESDIR
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

            InvocationCCM_writecfg $delay $typ TIME_TK
	    start_test2H $RESDIR $mondelay
             # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
		    
	    cpall2H $RESDIR
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
        start_test2H $RESDIR $mondelay

	# test run not OK
	if [ $OKTEST == 129 ]; then
	    echo "Removing previous data..."
	    rm -rf $RESDIR/*
	    echo "Running the same test..."
	    start_test2H $RESDIR $mondelay
	fi

        cpall2H $RESDIR
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
        start_test2H $RESDIR $mondelay

	# test run not OK
	if [ $OKTEST == 129 ]; then
	    echo "Removing previous data..."
	    rm -rf $RESDIR/*
	    echo "Running the same test..."
	    start_test2H $RESDIR $mondelay
	fi

	cpall2H $RESDIR
	sleep $TEST_DELAY
    done
}

function do_TimeInvocation(){
    for delay in $DELAYS; do
            SUBSUBDIR="TI-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimeInvocation_writecfg $delay
            start_test2H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_ControlledInvocation(){
    for delay in $DELAYS; do
            SUBSUBDIR="TI-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            ControlledInvocation_writecfg $delay
            start_test2H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
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
        start_test2H $RESDIR $mondelay

        # test run not OK
        if [ $OKTEST == 129 ]; then
          echo "Removing previous data..."
          rm -rf $RESDIR/*
          echo "Running the same test..."
	  start_test2H $RESDIR $mondelay
	fi

	cpall2H $RESDIR
	sleep $TEST_DELAY
    done
}

function do_TimerInvocation(){
    for delay in $TIMER_DELAYS; do
            SUBSUBDIR="TI-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimerInvocation_writecfg $delay TIME_HR
            start_test2H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_TimerPrecision(){
    for delay in $TIMER_DELAYS; do
            SUBSUBDIR="TI-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimerPrecision_writecfg $delay TIME_HR
            start_test2H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
            sleep $TEST_DELAY
    done
}

function do_TimerThroughput(){
            SUBSUBDIR="THR-${delay}"
            RESDIR=$RESTOPDIR/$SUBSUBDIR
            mkdir -p $RESDIR

            TimerTroughput_writecfg $DELTAT_TIMER
            start_test2H $RESDIR $mondelay

            # test run not OK
	    if [ $OKTEST == 129 ]; then
		echo "Removing previous data..."
		rm -rf $RESDIR/*
		echo "Running the same test..."
		start_test2H $RESDIR $mondelay
	    fi
            cpall2H $RESDIR
            sleep $TEST_DELAY
}

###############################################################################

# Script R functions
function cpscript_NotificationSMG(){
  cp $rscriptdir/E-020.R $RESTOPDIR 
}

function cpscript_InvocationLB(){
  cp $rscriptdir/D-020.R $RESTOPDIR 
}

function cpscript_Invocation(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/A-020.R $RESTOPDIR 
}

function cpscript_InvocationRT(){
  cp $rscriptdir/InvocationRT2H.R $RESTOPDIR
  cp $rscriptdir/InvocationRTCore.R $RESTOPDIR
}

function cpscript_Marshalling(){
  cp $rscriptdir/Marshalling*2H.R $RESTOPDIR
  cp $rscriptdir/MarshallingCore.R $RESTOPDIR
}

function cpscript_Marshalling_CCM(){
  cp $rscriptdir/Marshalling*2HCCM.R $RESTOPDIR
  cp $rscriptdir/MarshallingCoreCCM.R $RESTOPDIR
}

function cpscript_Dispatcher(){
  cp $rscriptdir/Dispatcher2H.R $RESTOPDIR
  cp $rscriptdir/DispatcherCore.R $RESTOPDIR
}

function cpscript_Parallelism(){
  cp $rscriptdir/Parallelism2H.R $RESTOPDIR
  cp $rscriptdir/ParallelismCore.R $RESTOPDIR
} 

function cpscript_Invocation_CPU(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/A-021.R $RESTOPDIR 
}


function cpscript_TimeInvocation(){
  cp $rscriptdir/C-020.R $RESTOPDIR 
  cp $rscriptdir/CR0_CompCore.R $RESTOPDIR
}

function cpscript_ControlledInvocation(){
  cp $rscriptdir/C-025.R $RESTOPDIR 
  cp $rscriptdir/CR0_CompCore.R $RESTOPDIR
}

function cpscript_TimeParallelism(){
  cp $rscriptdir/C-040.R $RESTOPDIR 
  cp $rscriptdir/CR0_CompCore.R $RESTOPDIR
}

function cpscript_TimerPrecision(){
  cp $rscriptdir/C-090.R $RESTOPDIR 
}

function cpscript_TimerThroughput(){
  cp $rscriptdir/C-080.R $RESTOPDIR 
}

function cpscript_TimerInvocation(){
  cp $rscriptdir/C-060.R $RESTOPDIR 
}

function cpscript_InvocationCCM(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/B-020.R $RESTOPDIR 
}

function cpscript_InvocationCCM_CPU(){
  cp $rscriptdir/CR0-OW0_CompCore.R $RESTOPDIR
  cp $rscriptdir/B-021.R $RESTOPDIR 
}


################   MAIN #########################

cd $thisdir

scriptnam2typ2H `basename $0 .sh` 
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
