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


# Port numbers
PROPPORT=5015
NAMEPORT=5010
REPPORT=5020

# Is CDMW_START_SIMU_CONF set? The file may initialise REPPORT env variables
if [ -f "$CDMW_START_SIMU_CONF" ]; then
    . $CDMW_START_SIMU_CONF
fi

#----------------------------------------------------------------------
#  This shell script launches the Simulated Repository 
#  with the Name and Property servers.
#----------------------------------------------------------------------
usage()
{
    echo "Usage: $0  [options]"
    echo "option:"
    echo "    --help"
    echo "             Print this help"
    echo "    --name-port port "
    echo "             name service lauched at the specified port"
    echo "             Default is $NAMEPORT"
    echo "    --property-port port "
    echo "             property service lauched at the specified port"
    echo "             Default is $PROPPORT"
    echo "    --repository-port port "
    echo "             Simulated Repository lauched at the specified port"
    echo "             Default is $REPPORT"
    echo "Description:"
    echo "      This program starts the Performance test."
}


while [ $# -gt 0 ]
do
    case $1 in
        --help)
            usage $0
            exit 0
            ;;    
        --repository_port)
            shift        
            if [ $# -gt 0 ]
            then
                REPPORT="$1"
                shift
            fi
            ;;  
        --property_port)
            shift        
            if [ $# -gt 0 ]
            then
                PROPPORT="$1"
                shift
            fi
            ;;  
        --name_port)
            shift        
            if [ $# -gt 0 ]
            then
                NAMEPORT="$1"
                shift
            fi
            ;;  
        *)
            shift
            ;;
  esac
done



###
# TOOLS executables
$CDMW_RUNTIME_ROOT/bin/start_simu.sh --name-port $NAMEPORT --property-port $PROPPORT --repository-port $REPPORT --admin


