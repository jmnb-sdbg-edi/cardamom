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

# Cardamom installation directory (needed by some Makefile)
CDMW_HOME=/home/$USER/projects/CDMW-Install
# Performance test home directory (needed by Testparams.sh)
PERFTESTS_HOME=/home/$USER/projects/CDMW/ProdTests/perftests
# Add cTools lib to LD_LIBRARY_PATH
LD_LIBRARY_PATH=$PERFTESTS_HOME/cTools/lib:$LD_LIBRARY_PATH

# Needed CARDAMOM tools path
XERCES_ROOT=/tools/exec/xerces-c-src1_3_0
XALAN_ROOT=/tools/exec/xalan-j_2_3_1
XERCES_JAR=/tools/exec/xerces-2.0.1
SAXON_JAR=/tools/exec/saxon-6.5.3
OPENCCM_ROOT=/tools/exec/OpenCCM-0.4p6
FTP_DIR=/var/ftp
R_ROOT=$TOOLS_ROOT/R-1.9.0/bin
PATH=$PATH:$R_ROOT

# export environments variables
export PERFTESTS_HOME
export CDMW_HOME
export LD_LIBRARY_PATH
export TOOLS_ROOT
export XERCES_ROOT
export XALAN_ROOT
export XERCES_JAR
export SAXON_JAR
export OPENCCM_ROOT
export FTP_DIR
export PATH

# test settings
if test -z $PERFTESTS_HOME
then
    echo "Error: environment variables settings in 'env.sh'!"
    echo "PERFTESTS_HOME: $PERFTESTS_HOME"
fi
if test -z $CDMW_HOME
then
    echo "Error: environment variables settings in 'env.sh'!"
    echo "CDMW_HOME: $CDMW_HOME"
fi
if test ! -e $PERFTESTS_HOME/cTools/lib/libCTools.so
then
    echo "Warning: cTools library not compiled!"
    echo "Switch in $PERFTESTS_HOME/cTools, and type"
    echo "'./configure' , and then 'make'"
fi
find $PERFTESTS_HOME -name "*.sh" -exec chmod +x {} \;
