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

Welcome to the CDMW DEMO
=========================

The purpose of CDMW DEMO is to provide an comprehensive example for writing
software based on CDMW runtime middleware. Provided Makefiles may be reused
to write more sofisticated ones that suit needs of CDMW users.


To build CDMW demo:
	make all

Running the demo:
	cd run
	start_demo.sh



CONTENT:
=======
Makefile      : Global makefile
ORBacus410.mk : ORBacus/Java 4.1.0 configuration file
README.txt    : This file
config.mk     : CDMW configuration file for use by demo makefiles
site.mk       : Configuration file used by demo makefiles
data/         : Contains XML data files for starting demo programs
idl/          : Contains 'user-provided' IDL files
run/          : Contains scripts for starting demo programs
src/          : Contains 'user-provided' source files
xml/          : Contains 'user-provided' XML data files for CDMW codegeneration

The following directories are created by the global makefile
generated/    : Contains CDMW generated files
prod/         : Build directory


