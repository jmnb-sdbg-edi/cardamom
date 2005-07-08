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
INCLUDE_DIRS = -I./src
CXXFLAGS = -c -W -Wall $(INCLUDE_DIRS)
LDFLAGS = --shared

ifeq ($(profile),1)
   CXXFLAGS += -pg
   LD_FLAGS += -pg
endif #trace

ifeq ($(debug),1)
   CXXFLAGS += -g
endif # debug

ifeq (,$(optimize))
   CXXFLAGS += -O2
else
   CXXFLAGS += -O$(optimize)
endif # optimize


SRCS = $(wildcard ./src/ctools/time/*.cc)
OBJS = $(patsubst %.cc,%.o,$(SRCS))
DEPS = $(patsubst %.o,%.d,$(OBJS))

TARGET = ./lib/libCTools.so
COMPILE = $(CXX) $(CXXFLAGS)

all: $(TARGET)

$(TARGET): $(DEPS) $(OBJS)
	@echo $(SRCS)
	@echo $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS)


%.d: %.cc
	$(CXX) $(INCLUDE_DIRS) -M $< > $@
	$(CXX) $(INCLUDE_DIRS) -M $< | sed s/\\.o/.d/ > $@

%.o: %.cc
	$(COMPILE) -o $@  $<


clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

# depend: $(DEPS)
# 	@echo "Depdendencies Updated."

-include $(DEPS)
