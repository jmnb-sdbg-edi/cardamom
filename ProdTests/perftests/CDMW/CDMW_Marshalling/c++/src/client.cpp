/* ========================================================================== *
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
 * ========================================================================= */
 

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "ClientProcessControl.hpp"

namespace 
{
  const int SUCCESS = 0;
  const int FAILURE = 1;
  const int POA_THREAD_POOL_SIZE = 2;
  
}; // End anonymous namespace

int main(int argc, char* argv[])
{    
  int status = SUCCESS;
  
  std::cout << "Start Client" << std::endl;
    
  //number of test
  int niter = 1000;
  
  //delay
  int delay = 0;
  
  //type of invocation
  char invocation_type[32] = "";
  
  //type of sent data
  char data_type[1024] = ""; //LONG, FLOAT, ANY, RECORD
  
  //size of data sent
  int data_size;

  //type of measure
  char measure_type[1024] = ""; //HR_TIME, TH_TIME, TK_TIME

  // output file prefix
  char fileprefix[1024] ="";
  
  
  std::cout << "Reading Configuration File..." << std::endl;
  //read configuration file
  
  FILE * fd = fopen("client.cfg", "r");
  fscanf(fd,"%d",&niter);
  fscanf(fd,"%d",&delay);
  fscanf(fd,"%s",invocation_type);
  fscanf(fd,"%s",data_type);
  fscanf(fd,"%d",&data_size);
  fscanf(fd,"%s",measure_type);
  fscanf(fd,"%s",fileprefix);
  
  fclose(fd);
  
  std::cout << "Sample invocation= " << niter << std::endl;            
  std::cout << "Delay (ms)= " << delay << std::endl;            
  std::cout << "Invocation type = " << invocation_type << std::endl;             
  std::cout << "Type of data= " << data_type << std::endl;            
  std::cout << "Size of data= " << data_size << std::endl;            
  std::cout << "Type of time measure= " << measure_type << std::endl; 
  std::cout << "Prefix of output file names= " << fileprefix << std::endl;
  

  CORBA::ORB_var orb;   // orb reference
  ClockTime *measure=0; // time reading object reference

  
  try 
    {
      // ===================================================
      // Initialize the ORB
      // ===================================================
      Cdmw::OrbSupport::StrategyList orb_strategies;
      orb_strategies.add_OrbThreaded();
      orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
      
      orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
      
      // ===================================================
      // Get the root POA 
      // ===================================================
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
      
      // ===================================================
      // Activate the root POA manager
      // ===================================================
      PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
      poaManager->activate();

      // ===================================================
      // Instantiate time reading object 
      // ===================================================
      std::string measure_str = measure_type;
      if(measure_str.compare(MEASURE_HR)==0)
      {
          measure = new ClockHighResTime;
      }
      else if(measure_str.compare(MEASURE_TH)==0)
      {
          measure = new ClockCpuTime;
      }
      else if(measure_str.compare(MEASURE_TK)==0)
      {
          measure = new ClockCpuTick;
      }
      else // using default
      {
          std::cout << "Warning: non valid measure type specified!"
                    << " Using default!"
                    << std::endl;
          measure = new ClockHighResTime;
      }
      
      
      // ===================================================
      // create the process control for platform management
      // ===================================================
      Cdmw::CdmwInit::ProcessControl_var process_ctrl =
        new Marshalling::ClientProcessControl(orb.in(),
                                              niter,
                                              delay,
                                              invocation_type,
                                              fileprefix,
                                              data_type,
                                              data_size,
                                              measure);
      


      // ===================================================
      // Call generated CdmwInit
      // ===================================================
      Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, process_ctrl.in());
      
        
      // ===================================================
      // wait for CORBA message
      // ===================================================
      orb->run();
      
    } 
  catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex)
    {
      std::cerr << ex.what() << std::endl;
      status = FAILURE;
    }
  catch (const Cdmw::Exception & ex)
    {
      std::cerr << ex.what() << std::endl;
      status = FAILURE;
    } 
  catch(const CORBA::Exception& ex)  
    {        
      std::cerr << ex << std::endl;
      status = FAILURE;
    }
  catch(...)  
    {        
      std::cerr << "unexpected exception" << std::endl;
      status = FAILURE;
    }
  
  
  // ========================================================
  // program stopping
  // ========================================================
  
  // ===================================================
  // Call generated Cdmw cleanup
  // ===================================================
  Cdmw::CdmwInit::CDMW_cleanup(orb.in());
    
  // ===================================================
  // Call ORB cleanup
  // ===================================================
  Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
  
  // ===================================================
  // destroy orb
  // ===================================================
  
  if (!CORBA::is_nil(orb.in()))
    {
      try 
        {
          orb -> destroy();
        }
      catch(const CORBA::Exception& ex)
        {           
          std::cerr << ex << std::endl;
          status = FAILURE;
        }
    }

  delete measure;
  return status;    
}


