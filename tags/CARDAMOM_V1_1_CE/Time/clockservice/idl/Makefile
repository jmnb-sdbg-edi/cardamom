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
INCL=-I/tools/exec/ACE_wrappers/TAO -I/tools/exec/ACE_wrappers/TAO/orbsvcs/orbsvcs
FLAGS= $(INCL) -o Time/clockservice -hs .skel.hpp -hc .stub.hpp  -cs .skel.cpp -ss .stub.cpp

all:
	-mkdir Time/clockservice
	tao_idl $(FLAGS) *.idl

clean:
	rm -rfv Time/clockservice/*
