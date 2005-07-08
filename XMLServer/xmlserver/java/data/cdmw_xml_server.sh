#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES 
# and SELEX-SI. 
# 
# It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
# All rights reserved.
# 
# CARDAMOM is free software; you can redistribute it and/or modify it under 
# the terms of the GNU Library General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your 
# option) any later version. 
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
# License for more details. 
# 
# You should have received a copy of the GNU Library General 
# Public License along with CARDAMOM; see the file COPYING. If not, write to 
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

case @config.XMLServer.java.orb@ in
    ORBacusJAVA)
	__CLASSPATH__=@config.ORBacusJAVA.classpath@

	;;

    JacORB)
        __CLASSPATH__=@config.JacORB.classpath@

        ;;

    *)
		echo " invalid ORB constraint...";
		exit 1;
		;;
esac

__CLASSPATH__=$__CLASSPATH__:@config.XalanJAVA.classpath@
__CLASSPATH__=:@config.XercesJAVA.classpath@:$__CLASSPATH__:
__CLASSPATH__=$__CLASSPATH__:@config.FtpJAVA.classpath@


# add CDMW jars
__CLASSPATH__=$__CLASSPATH__:\
@CDMW_FOUNDATION_LIB_JAR_PATH@/lib@config.lowercase.product.name@common.jar:\
@CDMW_FOUNDATION_LIB_JAR_PATH@/lib@config.lowercase.product.name@ossupport.jar:\
@CDMW_FOUNDATION_LIB_JAR_PATH@/lib@config.lowercase.product.name@orbsupport.jar:\
@CDMW_REPOSITORY_LIB_JAR_PATH@/lib@config.lowercase.product.name@repositoryidl.jar:\
@CDMW_REPOSITORY_LIB_JAR_PATH@/lib@config.lowercase.product.name@naminginterface.jar:\
@CDMW_REPOSITORY_LIB_JAR_PATH@/lib@config.lowercase.product.name@repositoryinterface.jar:\
@CDMW_LIFECYCLE_LIB_JAR_PATH@/lib@config.lowercase.product.name@lifecycleidl.jar:\
@CDMW_CONFANDPLUG_LIB_JAR_PATH@/lib@config.lowercase.product.name@cdmwinit.jar:\
@CDMW_SYSTEMMNGT_LIB_JAR_PATH@/lib@config.lowercase.product.name@systemmngtidl.jar:\
@CDMW_SYSTEMMNGT_LIB_JAR_PATH@/lib@config.lowercase.product.name@platforminterface.jar:\
@CDMW_XMLSERVER_LIB_JAR_PATH@/lib@config.lowercase.product.name@xmlserveridl.jar:\
@CDMW_XMLSERVER_LIB_JAR_PATH@/lib@config.lowercase.product.name@xmlserverlib.jar:\
@CDMW_XMLSERVER_LIB_JAR_PATH@/lib@config.lowercase.product.name@xmllibrary.jar:\
@CDMW_XMLSERVER_BIN_JAR_PATH@/xmlserver.jar:\

# add library path for java native libraries
# add library path for java native libraries
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:@CDMW_FOUNDATION_LIB_PATH@
LIBPATH=$LIBPATH:@CDMW_FOUNDATION_LIB_PATH@

exec java -Xbootclasspath/p:$__CLASSPATH__ cdmw.xmlutils.XMLServer $*

