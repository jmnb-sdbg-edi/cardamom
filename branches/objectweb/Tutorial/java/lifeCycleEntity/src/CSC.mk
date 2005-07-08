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

# List of source files.
SRCS_JAVA=


# List of additional jars to append to the classpath.
# Paths can be Absolute or Relative.
# Relative paths shall be defined from 
# prod/<ORB_REF> subdir.
ADDITIONAL_JARS=


# List of all idl files producing server code ( skeleton+stub ).
SRCS_IDL_SVR=


# List of the idl files producing server code ( skeleton+stub )
# with tie classes.
# Intersection between SRCS_IDL_SVR and SRCS_IDL_TIE shall
# be empty.
SRCS_IDL_TIE=


# List of all idl files producing client code ( only stub ).
SRCS_IDL_CLT=


# List of CSCs which the current CSC depends on.
CSC_DEPEND_LIB= tracelibrary  cdmwinit repositoryinterface naminginterface lifecycle platforminterface orbsupport ossupport common repositoryidl lifecycleidl platformidl systemmngtidl



# Defines the main xml for the code generation.
CODEGEN_MAIN_XML= 

# List of files to install into 
# <installation_dir>/share by make install.
# Paths can be Absolute or Relative.
# Relative paths shall be defined from the "src" directory.
DATA_FILES=


# List of files to install into 
# <installation>/bin by make install.
# Paths can be Absolute or Relative.
# Relative paths shall be defined from the "src" directory
BIN_FILES=

