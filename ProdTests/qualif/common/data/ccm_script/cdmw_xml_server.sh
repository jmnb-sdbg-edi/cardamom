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

VALIDATION_JAR_PATH=$PWD
echo "*******************************"
echo $VALIDATION_JAR_PATH
echo "*******************************"


__CLASSPATH__=""

# add jars
for jar in `ls $VALIDATION_JAR_PATH/lib/jars/*.jar`
do
    __CLASSPATH__=$__CLASSPATH__$jar:
done

# set library path for java native libraries
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$VALIDATION_JAR_PATH/lib
LIBPATH=$LIBPATH:$VALIDATION_JAR_PATH/lib

exec java -Xbootclasspath/p:$__CLASSPATH__ cdmw.xmlutils.XMLServer $*

