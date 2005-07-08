#Makefile for clocksvcs
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

TOP=..
CC=g++

INCL=-I$(ACE_ROOT)/ -I$(TAO_ROOT)/ -I$(TAO_ROOT)/orbsvcs\
	-I$(TOP)/clockservice/c++/src/ -I$(TOP)/clockservice/idl/\
	-I$(CDMW_ROOT)/Foundation/common/c++/incl

FLAGS=-g $(INCL) -Wall -Wno-deprecated -DNOT_LOCALISATION=1 -DCDMW_ORB_VDR=tao -DCDMW_NO_ASSERT

LDFLAGS=-L$(TOP)/lib -lClock -L$(ACE_ROOT)/ace -lACE \
	-L$(TAO_ROOT)/tao -lTAO\
	-L$(TAO_ROOT)/orbsvcs/orbsvcs -lTAO_CosProperty

# -lTAO_Valuetype

OBJS=./c++/src/ClockService.o

%.o:%.cpp
	$(CC) $(FLAGS) -c $< -o $@
	
TARGET=clocksvcs

all default:$(TARGET)
	$(CC) $(FLAGS) $(OBJS) $(LDFLAGS) -o $(TOP)/bin/$(TARGET)

$(TARGET):$(OBJS)

clean:
	-rm -rfv $(TARGET) $(OBJS)
