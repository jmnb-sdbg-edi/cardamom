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

#
#$Id: Makefile,v 1.1.1.1 2005-07-08 14:31:03 moghrabi Exp $
#

SHELL = /bin/sh

PWD:=$(shell pwd)



all :: exe

# We Build executable files
.PHONY: exe
exe::
	@$(MAKE) -C $(PWD)/CDMW_InvocationC/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_Marshalling/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_Dispatcher/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_Parallelism/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_Marshalling_CCM/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_TimerInvocation/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_TimeInvocation/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_TimerThroughput/c++ --no-print-directory  all
	@$(MAKE) -C $(PWD)/CDMW_TimeParallelism/c++ --no-print-directory  all

clean::
	@$(MAKE) -C $(PWD)/CDMW_InvocationC/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_Marshalling/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_Dispatcher/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_Parallelism/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_Marshalling_CCM/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_TimerInvocation/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_TimeInvocation/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_TimerThroughput/c++ --no-print-directory  clean
	@$(MAKE) -C $(PWD)/CDMW_TimeParallelism/c++ --no-print-directory  clean

cleanall::
	@$(MAKE) -C $(PWD)/CDMW_InvocationC/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_Marshalling/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_Dispatcher/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_Parallelism/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_Marshalling_CCM/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_TimerInvocation/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_TimeInvocation/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_TimerThroughput/c++ --no-print-directory  cleanall
	@$(MAKE) -C $(PWD)/CDMW_TimeParallelism/c++ --no-print-directory  cleanall



