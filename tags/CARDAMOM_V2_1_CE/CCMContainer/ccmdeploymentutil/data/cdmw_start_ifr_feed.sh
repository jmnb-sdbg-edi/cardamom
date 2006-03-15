#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
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


# output IOR file
IOR_FILE="IFRepository.IOR"


EXEC=exec

if test "`echo -e xxx`" = "xxx"
then
  echo="echo -e"
else
  echo=echo
fi

#----------------------------------------------------------------------
#  This shell script launches the IFR feed  
#----------------------------------------------------------------------

usage()
{
cat >&2 <<EOF

Usage: $0  [options] [ifr_options] <file ....>
options:
    --help
            Print this help
    --ifr-ior IOR
            name of IFR IOR file
            
    ifr_options
            ifr tool options

    <file ....>
            name of idl files

Description:
      This program starts the IFR feed.
EOF
}

while_exit=0

while [ $# -gt 0 -a $while_exit = "0" ]
do
    case $1 in
	    --help)
		    usage $0
		    exit 0
		    ;;    
	    --ifr-ior)
		    shift        
		    if [ $# -gt 0 ]
		    then
			  IOR_FILE="$1"
              shift
            fi
            ;; 
        *)
		    while_exit="1"
            ;;
    esac
done

# wait IFR Service is started by testing ior creation
sleep 1
until [ -s $IOR_FILE ]; do
  sleep 1
done

# start IFR feed
$echo "starting the Interface Repository feed utility ..."

#case @config.SystemMngt.cpp.orb@ in
#   TAO)
#       $echo $EXEC tao_ifr -v -ORBInitRef InterfaceRepository=file://$IOR_FILE -I. -I../idl $@
#       $EXEC tao_ifr -v -ORBInitRef InterfaceRepository=file://$IOR_FILE -I. -I../idl $@     
#	    ;;
#
#	ORBacusCPP)
#		;;
#						
#   *)
#		echo " invalid ORB constraint...";
#	    exit 1;
#        ;;
#esac
#


OpenCCM_HOMEDIR=@config.OpenCCM.install.dir@
export OpenCCM_HOMEDIR

PATH=@config.install.dir@/bin:${OpenCCM_HOMEDIR}/bin:${PATH}
export PATH
 
args="org.objectweb.ccm.scripts.IR3Feed -ORBInitRef"
args="$args InterfaceRepository=file:$IOR_FILE -D__OpenCCM_IFR__ -I. -I@config.install.dir@/idl -I$TAO_ROOT/tao -I$TAO_ROOT/Tao_idl -I$TAO_ROOT $@"

BOOTJARFILES=${OpenCCM_HOMEDIR}/lib/OpenCCM.jar:@config.OpenORB.install.dir@/lib/openorb-1.3.0.jar:@config.OpenORB.install.dir@/lib/openorb_tools-1.3.0.jar:@config.OpenORB.install.dir@/lib/avalon-framework.jar:@config.OpenORB.install.dir@/lib/excalibur-configuration.jar:@config.OpenORB.install.dir@/lib/logkit.jar:@config.J2SDK.install.dir@/jre/lib/rt.jar:@config.J2SDK.install.dir@/jre/lib/i18n.jar:${OpenCCM_HOMEDIR}/lib/log4j-1.1.3.jar:${OpenCCM_HOMEDIR}/lib/velocity-1.2-dep-modified.jar:${OpenCCM_HOMEDIR}/lib/jta_1.0.1.jar:${OpenCCM_HOMEDIR}/lib/velocity-1.2-modified.jar:${OpenCCM_HOMEDIR}/lib/commons-collections.jar:${OpenCCM_HOMEDIR}/lib/jdbc2_0-stdext.jar:${OpenCCM_HOMEDIR}/lib/jdom.jar:${OpenCCM_HOMEDIR}/lib/xerces.jar:${OpenCCM_HOMEDIR}/lib/openorb_ots-1.3.0.jar:${OpenCCM_HOMEDIR}/lib/dtdparser114.jar:${OpenCCM_HOMEDIR}/lib/zeus.jar

JVM=@config.J2SDK.install.dir@/jre/../bin/java
        
OPT="-Djava.compiler=NONE"

$EXEC $JVM $OPT -Xbootclasspath:${BOOTJARFILES} $args

