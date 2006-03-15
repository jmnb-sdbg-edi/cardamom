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

if [ $# -lt 1 ];
then
    echo "Usage :"
    echo "$0 validation_scenario"
    exit
fi


if [ -f $1.sh ];
then
    echo "Executing scenario $1"
    . $1.sh
    exit
elif [ ! -f $1.xml ];
then
    echo "Invalid scenario"
    exit
fi


echo "========================================================================="
echo "=                      CDMW Validation Test                            ="
echo "========================================================================="
echo ""
echo "The Cdmw Daemon must have been started with start_daemon_verbosely.sh"
echo "on each host used by the scenario."
echo "The Platform Supervision must have been started with start_supervision.sh"
echo "Hit a key when ready."
read FOO
echo "========================================================================="

SCENARIO=$1
SCENARIO_XML=$1.xml

HOSTS="@hostname1val@ @hostname2val@"
ROOT_DIR=$PWD

# Temporary files where to store results
RESULTS_FILE="CdmwTestResult.$$"
TESTS_FILE="CdmwValidationTests.output"
GLOBAL_TESTS_FILE="GlobalCdmwValidationTests.output"

EXEC_TIMEOUT=30
CLEANUP_TIMEOUT=5

CORE="no"
ADMIN_ERROR="no"

# We clean any previous tests results
rm -f $GLOBAL_TESTS_FILE

# We run the scenario

echo "Define system $SCENARIO"
define_system.sh $SCENARIO_XML

echo "Start system"
start_system.sh

if [ $? -eq "0" ];
then
    echo "Waiting a while for the execution of the scenario"
    echo "Hit a key when ready to stop."
    read FOO
else
    ADMIN_ERROR="yes"
fi

for HOST in $HOSTS; do
    # We check if the test has not produced a core dump

    CORE_TEMP=`rsh $HOST find $ROOT_DIR -name core.*`

    if [ "$CORE_TEMP" ]; then
        CORE="yes"
        echo "### the scenario $SCENARIO has produced a core dump on host $HOST ###" >> $RESULTS_FILE
    fi

done

echo "Stop system $SCENARIO"
stop_system.sh
sleep $CLEANUP_TIMEOUT


echo "=============================================================================="
echo "The Platform Supervision and the Cdmw Daemon on each host must now be stopped"
echo "Hit a key when ready."
read FOO


#store results
if [ ! -d $SCENARIO-results ];
then
    mkdir  $SCENARIO-results
fi


#create the global test file result
for HOST in $HOSTS; do
    # We check if the test has not produced a core dump

    rcp $HOST:$ROOT_DIR/$TESTS_FILE execution_$HOST.output
    cat execution_$HOST.output >> $GLOBAL_TESTS_FILE

    # keep the execution output
    mv execution_$HOST.output $SCENARIO-results

done

NB_ERR1=`grep -w -c FAILED $GLOBAL_TESTS_FILE`
NB_ERR2=`grep -w -c FAILURE $GLOBAL_TESTS_FILE`
NB_ERR=`expr $NB_ERR1 + $NB_ERR2`

if [ "$NB_ERR" != "0" ]; then
    echo "### the validation scenario has produced $NB_ERR errors ###" >> $RESULTS_FILE
elif [ "$ADMIN_ERROR" != "no" ]; then
    echo "### the platform admin has reported an error ###" >> $RESULTS_FILE
elif [ "$CORE" = "no" ]; then
    echo "### the validation scenario is OK ###" >> $RESULTS_FILE
fi

# Now we can display all the results
echo "==========================================================="
cat $RESULTS_FILE
echo "==========================================================="

mv $RESULTS_FILE $SCENARIO-results/scenario.result

