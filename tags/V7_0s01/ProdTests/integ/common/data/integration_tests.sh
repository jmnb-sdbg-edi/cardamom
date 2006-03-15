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

# Install executables
echo "Install executables..."
./install.sh

# Start Tests
echo "Starting Integration Tests..."
SCENARII="integ_local_repository.xml
          integ_local_repository_err1.xml
          integ_local_repository_err2.xml
          integ_local_2.xml
          integ_local_3.xml
          integ_local_5.xml
          integ_local_FT.xml
          integ_local_naminginterface_java.xml
          integ_deployment.xml
          integ_event_deployment.xml
          java_integ_deployment.xml
          java_integ_event_deployment.xml"

# Temporary files where to store results
RESULTS_FILE="CdmwIntegrationTests.results"
TESTS_FILE="CdmwIntegrationTests.output"
ALL_TESTS_FILE="CdmwAllIntegrationTests.output"

# Log files
RESULTS_LOG="CdmwIntegrationTests.results_log"
EXEC_LOG="CdmwIntegrationTests.execution_log"

INIT_TIMEOUT=8
EXEC_TIMEOUT=30
CLEANUP_TIMEOUT=5

CORE="no"
ADMIN_ERROR="no"

site_file=$CDMW_RUNTIME_ROOT/site.mk
CXX_VER="`cat $site_file | grep CXX_VER | cut -d= -f2`"
CXX_NAME="`cat $site_file | grep CXX_NAME | cut -d= -f2`"
ORB_CPP="`cat $site_file | grep ORB_CPP | cut -d= -f2`"
PLATFORM="`$CDMW_RUNTIME_ROOT/bin/config.guess`"
LD_LIBRARY_PATH=$CDMW_RUNTIME_ROOT/lib/$ORB_CPP/$PLATFORM-$CXX_NAME-$CXX_VER:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

echo "========================================================================="
echo "=                      CDMW Integration Tests                          ="
echo "========================================================================="
echo ""

# We clean any previous tests check files
rm -f $RESULTS_FILE

# We run the test in each ready CSC
# REM : The start and stop of Platform Management is done in the loop because there is
#       no reset_system for the moment
for scenario in $SCENARII; do

    # Start daemon
    echo "Starting the Platform Management Daemon..."
    # don't run on term because we must kill the shell (for the trap), not the term
    ./start_daemon_verbosely.sh &

    DAEMON_PID=$!
    sleep $INIT_TIMEOUT

    # Start Platform Management Supervision
    echo "Starting the Platform Management Supervision..."
    $TERM -ls -sb -sl 900 -e ./start_supervision.sh &
    SUPERVISION_PID=$!
    sleep $INIT_TIMEOUT

    # We clean any previous core files
    rm -f core

    echo "Define system $scenario"
    ./define_system.sh $scenario

    echo "Start system"
    ./start_system.sh

    if [ $? -eq "0" ];
    then
        echo "Waiting a while for the execution of the scenario"
        sleep $EXEC_TIMEOUT

        if [ $scenario = "integ_deployment.xml" ]
        then
            ../exec/cdmw_deployment_tool.sh --repositoryPort=22456 < ccm_integration_commands.txt
        fi
        if [ $scenario = "java_integ_deployment.xml" ]
		then
		    ../exec/cdmw_deployment_tool.sh --repositoryPort=22456 < java_ccm_integration_commands.txt
        fi
        if [ $scenario = "integ_event_deployment.xml" ]
        then
            ../exec/cdmw_deployment_tool.sh --repositoryPort=22456 < ccm_event_integration_commands.txt
        fi
        if [ $scenario = "java_integ_event_deployment.xml" ]
		then
		    ../exec/cdmw_deployment_tool.sh --repositoryPort=22456 < java_ccm_event_integration_commands.txt
        fi

    else
        ADMIN_ERROR="yes"
        echo "### the platform admin has reported an error for scenario $scenario ###" >> $RESULTS_FILE
    fi

    # We check if the test has not produced a core dump
    if [ -f core ]; then
        CORE="yes"
        echo "### the scenario $scenario has produced a core dump ###" >> $RESULTS_FILE
        rm -f core
    fi

    echo "Stop system"
    ./stop_system.sh

    sleep $CLEANUP_TIMEOUT

    kill -9 $SUPERVISION_PID
    # The kill signal on daemon pid will be trapped (see start_daemon_verbosely.sh)
    echo "Kill Platform management daemon"
    kill $DAEMON_PID

    cat $TESTS_FILE >> $ALL_TESTS_FILE
    sleep $CLEANUP_TIMEOUT

done


NB_ERR1=`grep -c FAILED $ALL_TESTS_FILE`
NB_ERR2=`grep -c FAILURE $ALL_TESTS_FILE`
NB_ERR=`expr $NB_ERR1 + $NB_ERR2`

if [ "$NB_ERR" != "0" ]; then
    echo "### the integration tests has produced $NB_ERR errors ###" >> $RESULTS_FILE
elif [ "$ADMIN_ERROR" = "no" -a "$CORE" = "no" ]; then
    echo "### the integration tests are OK ###" >> $RESULTS_FILE
fi

rm -fr ../workDir
rm -fr @config.Ftp.homedir@/cdmw_integration

# Now we can display all the results
echo "==========================================================="
cat $RESULTS_FILE
echo "==========================================================="

mv $ALL_TESTS_FILE $EXEC_LOG
mv $RESULTS_FILE $RESULTS_LOG

echo "Integration Tests done."

