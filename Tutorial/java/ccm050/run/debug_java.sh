# /bin/sh!
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

DIRECTORY="`dirname $0`"

CDMW_HOME=/home/Perco/NotSaved/Gomez/CARDAMOM_install_dir
CDMW_SRC=/home/Perco/Users/Gomez/CARDAMOM

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"

# set classpath 
CP=""
for jar in `ls $CDMW_HOME/bin/*.jar`
do
    CP=$CP$jar:
done
for jar in `ls $CDMW_HOME/lib/java/*.jar`
do
    CP=$CP$jar:
done

ORBACUS_JAVA_JAR_PATH=/tools/exec/JOB-4.1.0p/lib
for jar in `ls $ORBACUS_JAVA_JAR_PATH/*.jar`
do
    CP=$CP$jar:
done

XERCES_JAR_PATH=/tools/exec/xerces-2.0.1/
if [ "xxx$XERCES_JAR_PATH" != "xxx" ]
then 
    for jar in `ls $XERCES_JAR_PATH/*.jar`
    do
        CP=$CP$jar:
    done
fi
    
    
for jar in `ls $DIRECTORY/../$HOST_TYPE/*.jar`
do
    CP=$CP$jar:
done


# set library path for java native library
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CDMW_HOME/lib/c++

# set sourcepath
SOURCEPATH=$CDMW_SRC/Event/eventchannelmanager/java/src:\
$CDMW_SRC/Event/eventinterface/java/src:\
$CDMW_SRC/Event/eventsupport/java/src


echo "java -Xbootclasspath/p:$CP \
          -Djava.ext.dirs=/tools/exec/j2sdk1.4.1_02/lib \
          -jar ~enoch/Jswat/jswat.jar 'sourcepath $SOURCEPATH; load $*; stop cdmw.event.OrbacusEventChannelFactoryImpl:302;resume'"  

exec java -Xbootclasspath/p:$CP \
          -Djava.ext.dirs=/tools/exec/j2sdk1.4.1_02/lib \
          -jar ~enoch/Jswat/jswat.jar "sourcepath $SOURCEPATH; classpath $CP; load $*; stop cdmw.event.OrbacusEventChannelFactoryImpl:302; resume"  



