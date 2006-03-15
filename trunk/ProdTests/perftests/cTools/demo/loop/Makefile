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

CXX = g++
INCLUDE_DIRS = -I../../src
CXXFLAGS = -c -g -W -Wall $(INCLUDE_DIRS)
LDFLAGS  = -L../../lib -lCTools

ifeq ($(debug),1)
   CXXFLAGS += -g
endif # debug

ifeq (,$(optimize))
   CXXFLAGS += -O2
else
   CXXFLAGS += -O$(optimize)
endif # optimize

SRCS = demo.cc
OBJS = demo.o
TARGET = demo

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) demo.cc
	$(CXX) $(LDFLAGS) -o demo demo.o

clean:
	$(RM) *.o demo
