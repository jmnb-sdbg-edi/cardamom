/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "ftmanager/FaultDetector_impl.hpp"
#include "FaultTolerance/ftmonitoring/InitUtils.hpp"
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include "Foundation/ossupport/OS.hpp"
#include <sstream>
#include <vector>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>



namespace Cdmw {
namespace FT {
namespace FaultManagement {



class HostPrinter
{
public:
    HostPrinter(std::ostream & os): m_os(os) { }         
    void operator()(const std::pair< Cdmw::OrbSupport::type_traits< ::CdmwFT::FaultManagement::MonitoringId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< std::string >::_var_type > & x)
    {
        m_os <<"MonitoringID : "<<  x.first << "\n"
             <<"\thost : " << x.second<<"\n" << std::endl;
    }
 private:
    std::ostream & m_os;
};



//function for the find_if to recover all the consumer
bool HostTrueFunction(const std::pair< ::FT::FaultNotifier::ConsumerId,  std::string >&elt)
{
    return true;    
}

struct is_present 
    : std::binary_function<std::pair< ::FT::FaultNotifier::ConsumerId,  std::string>, std::string, bool>
{
    bool operator()(const std::pair< ::FT::FaultNotifier::ConsumerId,  std::string>&elt, std::string s_location) const
    {        
        bool result = false;
        if (s_location == elt.second)
        {	
            result = true;
        }	
        return result;
    }    
};


FaultDetector_impl::FaultDetector_impl(CORBA::ORB_ptr orb,
                                       PortableServer::POA_ptr poa,
                                       CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_ptr monitoringLibraryInterface)
    throw( CORBA::SystemException )
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_fault_notifier(::FT::FaultNotifier::_nil()),
      m_monitoringLibraryInterface(CdmwFT::MonitoringLibrary::MonitoringLibraryInterface::_duplicate(monitoringLibraryInterface)),
      m_simpleMonitoringObserverObj(NULL),
      is_monitoring(false)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if( CORBA::is_nil( orb ) ) {
        CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMNilObjectReference,
                            CORBA::COMPLETED_NO );
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    CdmwFT::MonitoringLibrary::Locations_var locations = new CdmwFT::MonitoringLibrary::Locations();
    
    m_interrogatingSimpleMonitoring 
        = m_monitoringLibraryInterface->create_interrogating_simple_monitoring(locations.in());

    m_monitoringIdStorageHome = new MonitoringIdStorageHome(MONITORING_ID_DATA_STORE);

    m_hostStorageHome = new HostStorageHome(HOST_DATA_STORE);

} 


FaultDetector_impl::~FaultDetector_impl()
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
	// create a list to get all the consumer 
      std::list<HostStorageObject> resultHost;

	// realise a find if to get all the consumer
      m_hostStorageHome->find_if(HostTrueFunction,
                                 std::back_inserter(resultHost));
      
	  // for each consumer, remove the data
      std::list<HostStorageObject>::iterator iterator1;
      for (iterator1 = resultHost.begin();
           iterator1 !=resultHost.end();
           iterator1++)
      {
          (*iterator1).remove();          
      }	
}

//
// IDL:thalesgroup.com/FT/PullMonitorable/is_alive:1.0
//
CORBA::Boolean FaultDetector_impl::is_alive ()
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_LOG_FUNCTION_RETURN(true);
    return true;
}

//
// IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector/start_monitoring:1.0
//
CdmwFT::FaultManagement::MonitoringId FaultDetector_impl::start_monitoring (
    ::FT::PullMonitorable_ptr monitorable_object,
    const ::FT::Location& location,
    const ::FT::FaultMonitoringIntervalAndTimeoutValue & timeout )
throw( CORBA::SystemException,
       ::FT::ObjectNotFound )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::ULong local_id = 0;  
    try {
        // when the supervision call start_monitoring on the fault detector, 
        // the monitorable object is nil
        if (CORBA::is_nil(monitorable_object))  {
            
            bool host_already_present = false;
            
            std::string s_location = std::string(location[0].id);
            CDMW_INFO(FTLogger::GetLogger(),"Start monitoring of the location '" <<s_location << "'");
            
            //check if the location is arleady present on the map
            
            std::vector<HostStorageObject> resultHost;
            m_hostStorageHome->find_if(std::bind2nd(is_present(),s_location),
                                       std::back_inserter(resultHost));
            if (resultHost.size() == 1) {
                host_already_present = true;
                // get the id of the host
                local_id = resultHost[0].get_oid();
            } else if (resultHost.size() > 1) {
                throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO);
            } else {
                // get the current id 
                try {
                    MonitoringIdStorageObject obj = m_monitoringIdStorageHome->find_by_oid("MonitoringID");
                    local_id = obj.get();
                    ++local_id;
                } catch (const Cdmw::FT::NotFoundException&) {
                    // to do
                    m_monitoringIdStorageHome->create("MonitoringID", 0);
                    local_id = 1;                
                } catch(const CORBA::SystemException& ex) {
                    CDMW_WARN(FTLogger::GetLogger(),"Could not find 'MonitoringID' in datastore for location '" <<s_location << "'");
                }
            }


            // if the host is already present on the map
            if ( host_already_present ) {
                if (m_simpleMonitoringObserverObj->status_is(s_location.c_str()))  {
                    // do noting besause the supervision set the host at HOST_RESPONDING when 
                    // the host is added
                } else {
                    try {        
                        CDMW_DEBUG(FTLogger::GetLogger(),"Add location '" << s_location << "'");
                        m_interrogatingSimpleMonitoring->add_location(s_location.c_str());    
                    } catch(const CdmwFT::MonitoringLibrary::AlreadyExists &) {
                        CDMW_ERROR(FTLogger::GetLogger(),"start_monitoring -> exception MonitoringLibrary::AlreadyExists");
                        //never here
                    }
                }
            } else { 
                // if the location is added for the first time
                try {        
                    CDMW_DEBUG(FTLogger::GetLogger(),"Add location '" << s_location << "'");
                    m_interrogatingSimpleMonitoring->add_location(s_location.c_str());    
                } catch(const CdmwFT::MonitoringLibrary::AlreadyExists &) {
                    CDMW_ERROR(FTLogger::GetLogger(),"start_monitoring -> exception MonitoringLibrary::AlreadyExists");    
                    //never here    
                } catch(const CORBA::SystemException& ex) {
                    CDMW_WARN(FTLogger::GetLogger(),"start_monitoring -> caught exception '" << ex << "'");
                }
                
                // insert the host in the map with the monitoring id
                m_hostStorageHome->create(local_id, s_location);
                MonitoringIdStorageObject obj = m_monitoringIdStorageHome->find_by_oid("MonitoringID");
                obj.set(local_id);
                
                try {            
                    // add_location on the observer to store the status
                    m_simpleMonitoringObserverObj->add_location(s_location.c_str());  
                } catch (const ::FT::MemberAlreadyPresent &) {
                    CDMW_ERROR(FTLogger::GetLogger(),"start_monitoring -> exception ::FT::MemberAlreadyPresent");
                    // catch 
                } catch(const CORBA::SystemException& ex) {
                    CDMW_WARN(FTLogger::GetLogger(),"start_monitoring -> caught exception '" << ex << "'");
                }
                
                // is the monitoring is not already started, start it
                if (!is_monitoring) {        
                    
                    is_monitoring = true;        
                    try {       
                        CDMW_DEBUG(FTLogger::GetLogger(),"Start monitoring of location '" << s_location << "'");
                        m_interrogatingSimpleMonitoring->start_monitoring();
                        
                    } catch (const CdmwFT::MonitoringLibrary::AlreadyStarted &) {
                        CDMW_WARN(FTLogger::GetLogger(),"start_monitoring -> exception MonitoringLibrary::AlreadyStarted");
                        //todo
                    } catch(const CORBA::SystemException& ex) {
                        CDMW_WARN(FTLogger::GetLogger(),"start_monitoring -> caught exception '" << ex << "'");
                    }
                }
            }
            
        } else {
            throw CORBA::NO_IMPLEMENT();
        }
    } catch (const CORBA::SystemException& ex) {
        CDMW_ERROR(FTLogger::GetLogger(),"start_monitoring -> caught exception '" << ex << "'");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const ::FT::ObjectNotFound & ex) {
        CDMW_WARN(FTLogger::GetLogger(),"start_monitoring -> caught exception '" << ex << "'");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const CORBA::Exception & ex) {
        CDMW_WARN(FTLogger::GetLogger(),"start_monitoring -> Ignoring unexpected exception '" << ex << "'!");
    }
    CDMW_LOG_FUNCTION_RETURN(local_id);
    return local_id;
        
}

//
// IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector/stop_monitoring:1.0
//
void FaultDetector_impl::stop_monitoring (
    CdmwFT::FaultManagement::MonitoringId monitoring_id )
throw( CORBA::SystemException,
       CdmwFT::FaultManagement::InvalidMonitoringId )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"monitoring_id="<<monitoring_id );
    std::string s_location;
    
    // find the host 
	try
	{
		HostStorageObject obj = m_hostStorageHome->find_by_oid(monitoring_id);
        s_location = obj.get();
        obj.remove();        
	}
	catch(const Cdmw::FT::NotFoundException&)
	{
        CdmwFT::FaultManagement::InvalidMonitoringId ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
	}

    CDMW_INFO(FTLogger::GetLogger(),"Stop monitoring of the location '" <<s_location << "'");   
    try
    { 
        // remove the host on the observer
        m_simpleMonitoringObserverObj->remove_location(s_location.c_str()); 
    }
    catch(const ::FT::MemberNotFound& ex)
    {
        CDMW_WARN(FTLogger::GetLogger(),"stop_monitoring -> Could not remove location  '" <<s_location << "' ex=" << ex);
    }
    
    // is the monitoring map is empty, stop the monitoring
    
    if (m_hostStorageHome->size() == 0)
    {
        is_monitoring = false; 
        try
        {            
            CDMW_DEBUG(FTLogger::GetLogger(),"stop the monitorng of hosts");
            m_interrogatingSimpleMonitoring->stop_monitoring();
            m_interrogatingSimpleMonitoring->reset_locations();
        }
        catch (const CdmwFT::MonitoringLibrary::NotStarted &)
        {
            CDMW_ERROR(FTLogger::GetLogger(),"stop_monitoring -> exception  MonitoringLibrary::NotStarted");
        }
    }
    
}


//
// IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultManagerDetector/set_fault_notifier:1.0
//
void FaultDetector_impl::set_fault_notifier (
    ::FT::FaultNotifier_ptr fault_notifier )
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_DEBUG(FTLogger::GetLogger(),"set the fault notifer and create the simple monitoring observer");
	// Set the fault notifier
    if( CORBA::is_nil( fault_notifier ) ) {
        CORBA::BAD_PARAM ex (OrbSupport::BAD_PARAMNilObjectReference,
                             CORBA::COMPLETED_NO );
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    m_fault_notifier = ::FT::FaultNotifier::_duplicate( fault_notifier );


    // for the first time, create the SimpleMonitoringObserver_impl
    if (m_simpleMonitoringObserverObj == NULL)
    {   
        m_simpleMonitoringObserverObj 
        = new Cdmw::FT::SimpleMonitoringObserver_impl(m_orb.in(),
                                                      m_poa.in(),
                                                      m_fault_notifier.in());
    
        m_interrogatingSimpleMonitoring->attach_observer(m_simpleMonitoringObserverObj);
    }
}


void FaultDetector_impl::activate()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_DEBUG(FTLogger::GetLogger(),"activate is called on the fault detector");
	// create a list to get all the consumer 
      std::list<HostStorageObject> resultHost;

	// realise a find if to get all the consumer
      m_hostStorageHome->find_if(HostTrueFunction,
                                 std::back_inserter(resultHost));
      
	  // for each consumer, remove the data
      std::list<HostStorageObject>::iterator iterator1;
      for (iterator1 = resultHost.begin();
           iterator1 !=resultHost.end();
           iterator1++)
      {
			std::string loc = (*iterator1).get();
            try
            {   
                CDMW_INFO(FTLogger::GetLogger(),"add location to monitor"<<loc.c_str());
                m_interrogatingSimpleMonitoring->add_location(loc.c_str());    
            }
            catch(const CdmwFT::MonitoringLibrary::AlreadyExists &)
            {
                CDMW_WARN(FTLogger::GetLogger(),"activate -> exception MonitoringLibrary::AlreadyExists");            
                //never here    
            }

      }	
      if (m_hostStorageHome->size() != 0)
          m_interrogatingSimpleMonitoring->start_monitoring();
}

void FaultDetector_impl::dumpHostDataStore()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    std::ostringstream oss;
    oss << "Dump Host datastore"<<std::endl;
    m_hostStorageHome->for_each(::Cdmw::FT::FaultManagement::HostPrinter(oss));
    CDMW_INFO(FTLogger::GetLogger(),oss.str());
    
}
    

}  // End namespace FaultManagement
}  // End namespace FT
}  // End namespace Cdmw

