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

CC=g++

INCL=-I$(ACE_ROOT)/ -I$(TAO_ROOT)/ -I$(TAO_ROOT)/orbsvcs\
	-I./clockservice/c++/src/ -I./clockservice/idl/\
	-I$(CDMW_ROOT)/Foundation/common/c++/incl

FLAGS=-g $(INCL) -Wall -DCDMW_ORB_VDR=tao -DCDMW_NO_ASSERT

IDL_OBJS=clockservice/idl/Time/clockservice/CdmwClockState.skel.o\
	clockservice/idl/Time/clockservice/CdmwClockState.stub.o\
	clockservice/idl/Time/clockservice/CdmwTime.skel.o\
	clockservice/idl/Time/clockservice/CdmwTime.stub.o\
	clockservice/idl/Time/clockservice/CosClockService.skel.o\
	clockservice/idl/Time/clockservice/CosClockService.stub.o\
	clockservice/idl/Time/clockservice/TimeBase.skel.o\
	clockservice/idl/Time/clockservice/TimeBase.stub.o


OBJS=clockservice/c++/src/ClockCatalog_impl.o\
	clockservice/c++/src/Clock_impl.o\
	clockservice/c++/src/config.o\
	clockservice/c++/src/ControlledClock_impl.o\
	clockservice/c++/src/ControlledExecutor_impl.o\
	clockservice/c++/src/Controller_impl.o\
	clockservice/c++/src/Date.o\
	clockservice/c++/src/Executor_impl.o\
	clockservice/c++/src/IClock.o\
	clockservice/c++/src/Macro.o\
	clockservice/c++/src/Time.o\
	clockservice/c++/src/TimeSpan_impl.o\
	clockservice/c++/src/UTC_impl.o\
	clockservice/c++/src/UTCTimeService_impl.o\
	clockservice/c++/src/Util.o


%.o:%.cpp
	$(CC) $(FLAGS) -c $< -o $@
	
TARGET=libClock.a

all:$(TARGET)
	-mkdir bin lib
	ar -rv lib/$(TARGET) $(OBJS) $(IDL_OBJS) 
	cd clocksvcs && $(MAKE)
	cd clockservice/test && $(MAKE)

#	$(CC) -shared $(OBJS) $(IDL_OBJS) -o lib/$(TARGET)

$(TARGET):$(OBJS) $(IDL_OBJS)

clean:
	-rm -rfv $(TARGET) $(OBJS) bin lib
	cd clocksvcs && $(MAKE) clean
	cd clockservice/test && $(MAKE) clean

