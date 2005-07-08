/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"

#ifndef INCL_PLATFORMMNGT_USERPROCESSBEHAVIOUR_TEST_HPP 
#define INCL_PLATFORMMNGT_USERPROCESSBEHAVIOUR_TEST_HPP 


using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;

static  int INITIAL = 0;
static  int INITIALISED = 1;
static  int RUNNING = 2;
static  int STOPPED = 3;


class UserProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour {
  
public :
  
  UserProcessBehaviour()
    :state(INITIAL), currentStep(1)
  {
    // do nothing
  };
  
  ~UserProcessBehaviour()
    throw()
  {
  }
  
  virtual CORBA::ULong nb_steps() 
    throw(CORBA::SystemException)
  {
    return 2;
  };
  
  
  int getCurrentStep()
  {
    return currentStep;
  };
  
  int getState()
  {
    return state;
  };
  
  virtual void next_step()
    throw(CdmwPlatformMngt::Process::InvalidStep, CORBA::SystemException)
  {
    if (currentStep == nb_steps()) {
      throw CdmwPlatformMngt::Process::InvalidStep();
    } else {
      currentStep++;
      // do initialisations for this step
      std::cout<<"Initialisation done for step "<< currentStep<<std::endl;;
    }
  };
  
  virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startupKind)
    throw(CdmwPlatformMngt::Process::BadOrder, CORBA::SystemException)
  {
    std::cout<<"Initialisation Requested"<<std::endl;
    state = INITIALISED;
  };
  
  
  virtual void run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun,
	  CdmwPlatformMngt::Process::AlreadyDone,
	  CORBA::SystemException) 
  {
    std::cout<<"Run Requested"<<std::endl;
    state = RUNNING;
  };
  
  virtual void stop()
    throw(CORBA::SystemException)
  {
    std::cout<<"Stop Requested"<<std::endl;
    state = STOPPED;
  };
  
  
private:
  int state;
  CORBA::ULong  currentStep;
  
  
  
};

#endif

