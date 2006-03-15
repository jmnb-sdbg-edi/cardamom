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

#include <iostream>

#include <ccm_ft_timers/CCM_ServerWithFacet_impl.hpp>

#include <Foundation/ossupport/OS.hpp>


//////////////
#include <Time/clockservice/StateTransferConfig.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Time/clockservice/CosClockService.skel.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <ccm_ft_timers/TimeoutHandler.hpp>

//
// IDL:thalesgroup.com/CcmFtTest/CCM_ServerWithFacet:1.0
//
CcmFtTest::CCM_ServerWithFacet_impl::CCM_ServerWithFacet_impl()
{
   m_expectedClientHostname = CORBA::string_dup("");
}

CcmFtTest::CCM_ServerWithFacet_impl::~CCM_ServerWithFacet_impl()
{
    std::cout << "   ... CCM_ServerWithFacet_impl::~CCM_ServerWithFacet_impl called!" << std::endl;
}



//
// attribute expectedClientHostname
//
char*
CcmFtTest::CCM_ServerWithFacet_impl::expectedClientHostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedClientHostname);
}

void
CcmFtTest::CCM_ServerWithFacet_impl::expectedClientHostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedClientHostname =  CORBA::string_dup(a);
}


//
// facet serverHostInfo
//
CcmFtTest::CCM_HostInfo_ptr 
CcmFtTest::CCM_ServerWithFacet_impl::get_serverHostInfo ()
    throw (CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::get_serverHostInfo()" << std::endl; 
    return CcmFtTest::CCM_HostInfo::_duplicate(this);
}

char*
CcmFtTest::CCM_ServerWithFacet_impl::get_hostname()
    throw (CORBA::SystemException)
{
    std::string hostname = Cdmw::OsSupport::OS::get_hostname();
    std::cout << "   ... CCM_ServerWithFacet_impl::get_hostname()  " << hostname << std::endl; 
    return CORBA::string_dup(hostname.c_str());
}    

char*
CcmFtTest::CCM_ServerWithFacet_impl::get_currentTime()
    throw (CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::get_currentTime()  "  << std::endl; 



    // get NamingInterface to object_groups (for reading)
    Cdmw::CommonSvcs::Naming::NamingInterface objGroupsNamingInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_CLOCK_GROUP_NAME.c_str());

    // get reference to Database ObjectGroup from NamingInterface
    CORBA::Object_var clock_obj;
        std::cout << "     Client: retrieve group reference of Database " << std::endl;
        clock_obj =
            objGroupsNamingInterface.resolve (CLOCK_GROUP_NAME);
    if (CORBA::is_nil (clock_obj.in ()))
      {
        std::cerr << "ERROR >> Unable to resolve clock" << std::endl;
      }

       CosClockService::PeriodicExecution::Executor_var executor_ =
            CosClockService::PeriodicExecution::Executor::_narrow( clock_obj.in() );
    if (CORBA::is_nil (executor_.in ()))
      {
        std::cerr << "ERROR >> Unable to resolve Executor" << std::endl;
      }



            Cdmw::clock::qualif::TimeoutHandler handler(executor_.in());
            CosClockService::PeriodicExecution::Periodic_var periodic=
                handler._this();

            CosClockService::PeriodicExecution::Controller_var controller =
                executor_->enable_periodic_execution(periodic.in());
            TimeBase::TimeT period = 10000000; // 100000 * 100 nsec =  1000 msec
            TimeBase::TimeT offset = 10000000;
            CORBA::Any any;

            const unsigned long max_executions = 10;
            controller->start(period, offset, max_executions, any);
            std::cout << "==== TIMER Scheduled... : "<< max_executions << "  executions ==== "  << std::endl;

          //while (controller->executions () < 100 ) {
          while (controller->executions () < max_executions ) {
                Cdmw::OsSupport::OS::sleep(1000);
            }

    return CORBA::string_dup("current time");
}    

//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
CcmFtTest::CCM_ServerWithFacet_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::Set_session_context called!" << std::endl;
   
    m_session_context = ::CcmFtTest::CCM_ServerWithFacet_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
CcmFtTest::CCM_ServerWithFacet_impl::ccm_activate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::Ccm_activate called!" << std::endl;

   std::cout << "   ...  Clock Service Initalization  ... " << REPOSITORY_CLOCK_GROUP_NAME.c_str() << std::endl;

////////////////////////////////////////////////////////////////
#if 0

    // get NamingInterface to object_groups (for reading)
    Cdmw::CommonSvcs::Naming::NamingInterface objGroupsNamingInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_CLOCK_GROUP_NAME.c_str());

    // get reference to Database ObjectGroup from NamingInterface
    CORBA::Object_var clock_obj;
        std::cout << "     Client: retrieve group reference of Database " << std::endl;
        clock_obj =
            objGroupsNamingInterface.resolve (CLOCK_GROUP_NAME);
    if (CORBA::is_nil (clock_obj.in ()))
      {
	std::cerr << "ERROR >> Unable to resolve clock" << std::endl;
      }

       CosClockService::PeriodicExecution::Executor_var executor_ =
            CosClockService::PeriodicExecution::Executor::_narrow( clock_obj.in() );
    if (CORBA::is_nil (executor_.in ()))
      {
	std::cerr << "ERROR >> Unable to resolve Executor" << std::endl;
      }


        std::cout << "    ********************************************* " << std::endl;

            for (int i = 0; i < 1000; ++i) {
                std::cout << " current time" <<  std::endl;

                std::cout << "************************ [" << i << "]: "
                          << executor_->current_time() / 10
                          << " usec ***************" << std::endl;
            }
#endif
////////////////////////////////////////////////////////////////
    // get reference to Database ObjectGroup from NamingInterface
    CORBA::Object_var obj;
#if 0
    try
    {
        std::cout << "     Client: retrieve group reference of Database " << CLOCK_GROUP_NAME << std::endl;
        obj =
            objGroupsNamingInterface.resolve (CLOCK_GROUP_NAME);

        CosClockService::PeriodicExecution::Executor_var executor_ =
            CosClockService::PeriodicExecution::Executor::_narrow( obj.in() );
         
    if (CORBA::is_nil (executor_.in ()))
      {
	std::cerr << "ERROR >> Unable to resolve Executor" << std::endl;
      }

        std::cout << "     >> Client:  CURRENT TIME " << executor_-> current_time() << std::endl;
        std::cout << "     Client: handler " << std::endl;
             Cdmw::clock::valid::TimeoutHandler handler(executor_.in());
        std::cout << "     Client:  executor " << std::endl;

        std::cout << "     Client:  Periodic executor " << std::endl;
            CosClockService::PeriodicExecution::Periodic_var periodic=
                handler._this();
    if (CORBA::is_nil (periodic.in ()))
      {
	std::cerr << "ERROR >> Periodic is null " << std::endl;
      }
        std::cout << "     Client:  Periodic executor setting " << std::endl;


        std::cout << "     >> Client:  register the newly created object with the Clock Service " << std::endl;
            // Now we register the newly created object with the Clock Service
            CosClockService::PeriodicExecution::Controller_var controller =
                executor_->enable_periodic_execution(periodic.in());
        std::cout << "     Client:  registation  object with the Clock Service " << std::endl;
            TimeBase::TimeT period = 10000000; // 100000 * 100 nsec =  1000 msec
            TimeBase::TimeT offset = 10000000;
            CORBA::Any any;

            std::cout << ">> Programming timer with:"
                      << "\n\tPeriod: " << period
                      << "\n\toffset: " << offset
                      << std::endl;

            controller->start(period, offset, 0, any);
            while (true) {
                sleep(1);
            }

    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::NotFound raised!" << std::endl;
    }
        catch (const CORBA::Exception& e)
        {
            std::cerr << "UNEXPECTED CORBA EXCEPTION !! :\n"
                      << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                      << std::endl;
        }

#endif

  
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void
CcmFtTest::CCM_ServerWithFacet_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::Ccm_passivate called!" << std::endl;
} 



//
// ccm_remove function : it destroys the component (call by the container)
void
CcmFtTest::CCM_ServerWithFacet_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::Ccm_remove called!" << std::endl;
}


