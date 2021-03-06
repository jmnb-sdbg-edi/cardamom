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

SELF=`basename $0`

if [ "$#" = "0" ]; then

cat >&2 <<EOF
Usage : $SELF program [arg1 ...]
 
EOF
    exit 1
fi

DIRECTORY="`dirname $0`"
PROGRAM="$1"

SITE_CONFIG="$DIRECTORY/../site.mk"

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"

# ORB name
#ORB="`cat $SITE_CONFIG | grep -w ORB |cut -d= -f2`"
#ORB_CONFIG="$DIRECTORY/../$ORB.mk"
#ORB_JARS="`cat $ORB_CONFIG | grep -w ORB_JARS:= |cut -d= -f2`"
#ORB_JAR_PATH_VAR="`cat $ORB_CONFIG | grep -w ORB_JAR_PATH:= |cut -d= -f2 | sed -e 's/\$(\(.*\))/\1/g'`"
#ORB_JAR_PATH="`cat $SITE_CONFIG | grep -w $ORB_JAR_PATH_VAR |cut -d= -f2`"

# There may be more than one ORB defined in the site.mk file, therefore
# we need to find a better way to determine the current ORB.
TMP_MAKEFILE=`dirname $SITE_CONFIG`/tempmakefile_$$
cat > $TMP_MAKEFILE << _EOF_
include $SITE_CONFIG
include `dirname $SITE_CONFIG`/\$(ORB).mk

get_orb:
	@echo \$(ORB)

get_orb_jars:
	@echo \$(ORB_JARS)

get_orb_jar_path:
	@echo \$(ORB_JAR_PATH)
_EOF_
ORB=`make -f $TMP_MAKEFILE get_orb`
ORB_JARS=`make -f $TMP_MAKEFILE get_orb_jars`
ORB_JAR_PATH=`make -f $TMP_MAKEFILE get_orb_jar_path`
rm -f $TMP_MAKEFILE

# We build the classpath
CP=".:$DIRECTORY/../prod/$ORB/cdmwdemo.jar"

# Jar files from Xerces and Xalan
XERCES_JAR_PATH="`cat $SITE_CONFIG | grep -w XERCES_JAR_PATH |cut -d= -f2`"
XALAN_JAR_PATH="`cat $SITE_CONFIG | grep -w XALAN_JAR_PATH |cut -d= -f2`"
CP=$CP:$XERCES_JAR_PATH/xercesImpl.jar:$XERCES_JAR_PATH/xmlParserAPIs.jar
CP=$CP:$XALAN_JAR_PATH/xalan.jar:$XALAN_JAR_PATH/xml-apis.jar

# Jar files from the ORB
for jar in $ORB_JARS; do
	CP="$CP:$ORB_JAR_PATH/$jar"
done

# Jar files from CDMW
for jar in `ls $CDMW_HOME/lib/java/*.jar $CDMW_HOME/bin/*.jar`; do
    CP="$CP:$jar"
done


# set library path for java native library
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CDMW_HOME/lib/c++
LIBPATH=$LIBPATH:$CDMW_HOME/lib/c++


shift

exec java -Xbootclasspath/p:$CP -Djacorb.implname=StandardImplName cdmw.validation.$PROGRAM $*

