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


Welcome to the SPLICE DEMO
=========================

The purpose of SPLICE DEMO is to provide an example for writing software using SPLICE. 

------------------------------------------------------------------------------------------------------------------

To setup and run the splice example on a single host do the following:

   . <splice_install_dir>/release.com

To build SPLICE demo:
	./BUILD

Running the demo:
	./RUN

------------------------------------------------------------------------------------------------------------------

To run the demo using 2 hosts, perform the following steps:

Set up the splice environment on both machines:
	. <splice_install_dir>/release.com

build the demo on both machines:
	./BUILD	

set the SPLICE_DDS_URI to point to splice configuration file on both machines:
	export SPLICE_DDS_URI=file:///<demo_dir>/splice_default.xml

adjust the splice configuration file on both machines:
	adjust the networking/args in <demo_dir>/splice_default.xml to point to <demo_dir>/splice_default_domain.xml

start the SPLICE-DDS daemon on both machines:
	sdds start

navigate to the <demo_dir>/exec directory

start ping on host 1:
	./ping 1000 100 m A B 

start pong on host 2:
	./pong A B

------------------------------------------------------------------------------------------------------------------

CONTENT:
=======
BUILD              	  : Makefile
README.txt         	  : This file
RUN                	  : Script to start the SPLICE-DDS daemon, and run the demo
ping.ccp           	  : Source code for ping
pong.cpp      	          : Source code for pong
pingpong.idl       	  : Interface description files
splice_default.xml 	  : Splice configuration file
splice_default_domain.xml : Splice domain configuration file
bld/             	  : Contains generated files
exec/              	  : Contains generated executables

------------------------------------------------------------------------------------------------------------------

