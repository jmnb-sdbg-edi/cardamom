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

SHELL = /bin/sh

include ../site.mk
# Imported variables
#   
#   CDMW_HOME
#   RM
#   CXX
#   CXXLD
#   IDLMKDEPEND
#   IDLMKINCLUDE
#   ORB
#   IDL2CXX
#   
#   CPPFLAGS_ORB
#   CXXFLAGS_ORB
#   IDLFLAGS_ORB
#   LDFLAGS_ORB
#   LIBS_ORB
#   
#   CPPFLAGS_CDMW
#   CXXFLAGS_CDMW
#   LDFLAGS_CDMW
#   IDLFLAGS_CDMW
#   LIBS_CDMW
#   
# NOTA: The variables below includes CDMW/ORB/OS specific variables
#   CPPFLAGS
#   CXXFLAGS
#   LDFLAGS
#   IDLFLAGS
#   LIBS
include ../config.mk

#Append to CPPFLAGS and IDLFLAGS
override CPPFLAGS += -I. -I../include -I../generated \
	$(foreach d, $(shell find $(CDMW_HOME)/include/c++ -name TAO\* -prune -o -type d -print), -I$(d)) \
        -I$(CDMW_HOME)/include/c++/TAOOpenFusion
override IDLFLAGS += -I../idl -I../generated

ALL_LIBS=$(LIBS)


# Exported variables
#   CORBA_ALL_IDL,
#   CORBA_CL_ALL_SRC,
#   CORBA_SRV_ALL_SRC,
#   CORBA_CL_ALL_OBJS,
#   CORBA_SRV_ALL_OBJS,
#   CORBA_CL_ALL_HDRS, and
#   CORBA_SRV_ALL_HDRS
include idl_var.mk


vpath %.idl ../idl:../generated
vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

GREEK_SERVER_NAME=greek_school_component_server
FRENCH_SERVER_NAME=french_school_component_server
OBSERVER_SERVER_NAME=observer_component_server

LIB_PHILOSOPHER_NAME=libPhilosopher.so
LIB_FORK_MANAGER_NAME=libForkManager.so
LIB_OBSERVER_NAME=libObserver.so
LIB_REGISTRATOR_NAME=libRegistrator.so


# user provided src

LIB_PHILOSOPHER_SRCS=\
    CCM_PhilosopherHome_impl.cpp CCM_Philosopher_impl.cpp StatusInfo_impl.cpp

LIB_FORK_MANAGER_SRCS=\
    CCM_Fork_impl.cpp CCM_ForkHome_impl.cpp CCM_ForkManager_impl.cpp

LIB_OBSERVER_SRCS=\
    CCM_ObserverHome_impl.cpp CCM_Observer_impl.cpp StatusInfo_impl.cpp

LIB_REGISTRATOR_SRCS=\
    CCM_RegistratorHome_impl.cpp CCM_Registrator_impl.cpp 


# generated src

GENERATED_PHILOSOPHER_SRCS=\
    CCM_Philosopher_SessionContext_impl.cpp \
    SessionPhilosopher_impl.cpp SessionPhilosopherHome_impl.cpp
    
GENERATED_FORK_MANAGER_SRCS=\
    CCM_ForkManager_SessionContext_impl.cpp \
    SessionForkManager_impl.cpp SessionForkHome_impl.cpp  \
    Fork_impl.cpp

GENERATED_OBSERVER_SRCS=\
    CCM_Observer_SessionContext_impl.cpp  \
    SessionObserver_impl.cpp  SessionObserverHome_impl.cpp \
    StatusInfoConsumer_impl.cpp

GENERATED_REGISTRATOR_SRCS=\
    CCM_Registrator_SessionContext_impl.cpp \
    SessionRegistrator_impl.cpp SessionRegistratorHome_impl.cpp


# servers src

GREEK_SERVER_SRCS=\
    main_GreekSchoolComponentServer.cpp GreekSchoolCdmwInterface.cpp \
    $(GENERATED_PHILOSOPHER_SRCS) $(GENERATED_FORK_MANAGER_SRCS)

FRENCH_SERVER_SRCS=\
    main_FrenchSchoolComponentServer.cpp FrenchSchoolCdmwInterface.cpp \
    $(GENERATED_PHILOSOPHER_SRCS) $(GENERATED_FORK_MANAGER_SRCS) \
    $(GENERATED_REGISTRATOR_SRCS)

OBSERVER_SERVER_SRCS=\
    main_ObserverComponentServer.cpp ObserverCdmwInterface.cpp \
    $(GENERATED_OBSERVER_SRCS)



.SUFFIXES:
+.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h



LIB_PHILOSOPHER_OBJ = $(LIB_PHILOSOPHER_SRCS:.cpp=.o)
LIB_FORK_MANAGER_OBJ = $(LIB_FORK_MANAGER_SRCS:.cpp=.o)
LIB_OBSERVER_OBJ = $(LIB_OBSERVER_SRCS:.cpp=.o)
LIB_REGISTRATOR_OBJ = $(LIB_REGISTRATOR_SRCS:.cpp=.o)

GREEK_SERVER_OBJ = $(GREEK_SERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
FRENCH_SERVER_OBJ = $(FRENCH_SERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
OBSERVER_SERVER_OBJ = $(OBSERVER_SERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)



# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$ $(LIB_PHILOSOPHER_SRCS) $(LIB_FORK_MANAGER_SRCS) $(LIB_OBSERVER_SRCS) \
       $(LIB_REGISTRATOR_SRCS) $(FRENCH_SERVER_SRCS) $(GREEK_SERVER_SRCS) \
       $(OBSERVER_SERVER_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)

.PHONY: all
all: depend $(LIB_PHILOSOPHER_NAME) $(LIB_FORK_MANAGER_NAME) $(LIB_OBSERVER_NAME) $(LIB_REGISTRATOR_NAME) $(FRENCH_SERVER_NAME) $(GREEK_SERVER_NAME) $(OBSERVER_SERVER_NAME)
	@echo "Create Dinner package"
	$(CDMW_DEMO_HOME)/run/make_assembly_package.sh

.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(FRENCH_SERVER_NAME)
.PHONY: $(GREEK_SERVER_NAME)
.PHONY: $(OBSERVER_SERVER_NAME)
.PHONY: $(LIB_PHILOSOPHER_NAME)
.PHONY: $(LIB_FORK_MANAGER_NAME)
.PHONY: $(LIB_OBSERVER_NAME)
.PHONY: $(LIB_REGISTRATOR_NAME)



#.cpp.o:
%.o:%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<



$(FRENCH_SERVER_NAME): $(FRENCH_SERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(FRENCH_SERVER_OBJ)  $(ALL_LIBS)

$(GREEK_SERVER_NAME): $(GREEK_SERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(GREEK_SERVER_OBJ)  $(ALL_LIBS)

$(OBSERVER_SERVER_NAME): $(OBSERVER_SERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(OBSERVER_SERVER_OBJ)  $(ALL_LIBS)

$(LIB_PHILOSOPHER_NAME): $(LIB_PHILOSOPHER_OBJ)
	@echo 
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_PHILOSOPHER_OBJ) $(ALL_LIBS)

$(LIB_FORK_MANAGER_NAME): $(LIB_FORK_MANAGER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_FORK_MANAGER_OBJ) $(ALL_LIBS)

$(LIB_OBSERVER_NAME): $(LIB_OBSERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_OBSERVER_OBJ) $(ALL_LIBS)

$(LIB_REGISTRATOR_NAME): $(LIB_REGISTRATOR_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_REGISTRATOR_OBJ) $(ALL_LIBS)



-include $(DEPENDS)
#Generated rules use the following variables:
# CXX,
# CPPFLAGS,
# CXXFLAGS,
# IDL2CXX,
# IDLFLAGS, and
# IDLMKINCLUDE
include idl_rules.mk

clean::
	-$(RM) *.o core
	-$(RM) $(CORBA_CL_ALL_SRC) $(CORBA_CL_ALL_HDRS)
	-$(RM) $(CORBA_SRV_ALL_SRC) $(CORBA_SRV_ALL_HDRS)
	-$(RM) $(DEPENDS)

cleanall: clean
	-$(RM) $(GREEK_SERVER_NAME) $(FRENCH_CLIENT_NAME) $(OBSERVER_CLIENT_NAME) \
	       $(LIB_PHILOSOPHER_NAME) $(LIB_FORK_MANAGER_NAME) \
	       $(LIB_OBSERVER_NAME) $(LIB_REGISTRATOR_NAME)

# -----------------------------------------------


