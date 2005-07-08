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

# Name of the target produced by the CSC, if the file 
# contains a '.a' the produced target is a library
# else an executable.
TARGET=


# List of sources files.
SRCS_CPP=


# List of all idl files producing server code ( skeleton+stub ).
SRCS_IDL_SVR=


# List of all idl files producing client code ( only stub ).
SRCS_IDL_CLT=


# List of CSCs which the current CSC depends on.
# This list includes CSC names only.
CSC_DEPEND_LIB=ccmcif ccmcontainer ccmcomponentserver ccmcommon lifecycle eventsupport eventinterface cdmwinit repositoryinterface naminginterface tracelibrary platforminterface idl orbsupport osthreads ossupport common


# List of test CSCs which the current CSC depends on.
# This list includes path names from CDMW_RUNTIME_ROOT.
CSC_TEST_DEPEND=


# Defines the main xml for the code generation
CODEGEN_MAIN_XML=


# List of files to install into
# <installation_dir>/share by make install.
# Paths can be Absolute or Relative.
# Relative paths shall be defined from the "src" directory.
DATA_FILES=


# List of additionnal object files for link.
# these files will be placed before the libraries
# on the link command.
OTHER_OBJECTS_CSC=


# Name of runtime/export subdirectory where .h and .hpp 
# are stored.
# These files will be installed in
# <installation_dir>/include/c++ subdir.
# If empty, no .h or .hpp will be copied.
CSC_CPP_EXPORT=


# List of additionnal option for the link (as "-L... -lCosEvent")
LDFLAGS_CSC= -lxerces-c1_3


# List of additionnal option for the compilation (as "-g -I...")
CPPFLAGS_CSC=


# List of files to install from prod subdir into 
# <installation_dir>/include/c++
SRCS_CPP_EXPORT=
