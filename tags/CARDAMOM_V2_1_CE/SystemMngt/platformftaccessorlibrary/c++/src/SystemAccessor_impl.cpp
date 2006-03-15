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
 
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "platformftaccessorlibrary/SystemAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include "platformsystemlibrary/HostContainer.hpp"

#include "SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp"

namespace 
{    
    const int MAX_RETRY_ON_FTMGR = 4;
    const int SLEEP_RETRY_ON_FTMGR_MS = 1000;
}

namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// SystemAccessor_impl class.
// ----------------------------------------------------------------------

SystemAccessor_impl::SystemAccessor_impl (
       CORBA::ORB_ptr orb,
       PortableServer::POA_ptr poa,
       PlatformMngt::System_impl* system,
       CdmwPlatformMngt::ElementRepository_ptr element_repository,
       const std::string& supervisionProcessName)
throw(Cdmw::Exception)
    : ManagedElementAccessor_impl(element_repository),
      HostContainerAccessor_impl(element_repository), 
      RWEntityContainerAccessor_impl(element_repository),
      RWServiceDefContainerAccessor_impl(element_repository),
      m_system(system)
{ 
    m_orb = CORBA::ORB::_duplicate(orb);
    m_poa = PortableServer::POA::_duplicate(poa);
    
    // set System Management location
    ::FT::Location smg_location;
    smg_location.length(2);
            
    smg_location[0].id = OsSupport::OS::get_hostname().c_str();
    smg_location[0].kind = "hostname";    
    smg_location[1].id = supervisionProcessName.c_str();
    smg_location[1].kind = "processname"; 
            
      
    //
    // Get Replication Manager
    //
    CORBA::Object_var rm_obj;
    try 
    {
        // resolve initial reference set by CdmwInterface
        rm_obj = m_orb->resolve_initial_references ("ReplicationManager");
    } 
    catch(const CORBA::ORB::InvalidName&) 
    {
        std::cerr<< "Can't resolve `ReplicationManager'" << std::endl;
        CDMW_THROW2(Cdmw::Exception, Cdmw::Exception::FATAL,
                    "Can't resolve `ReplicationManager'");
    }
    
    if (CORBA::is_nil(rm_obj.in())) 
    {
        std::cerr << "`ReplicationManager' is a nil object reference" << std::endl;
        CDMW_THROW2(Cdmw::Exception, Cdmw::Exception::FATAL,
                    "Invalid ReplicationManager reference" );
    }
    
    try 
    {
        m_replication_mngr = CdmwFT::ReplicationManager::_narrow(rm_obj.in());
    } 
    catch (const CORBA::Exception&) 
    {
        std::cerr << "Obj is not a ReplicationManager" << std::endl;
        CDMW_THROW2(Cdmw::Exception, Cdmw::Exception::FATAL,
                    "Not a ReplicationManager reference" );
    }
    
    // get IOGR as string
    m_replication_manager_IOGR = orb->object_to_string(m_replication_mngr.in());
        
    // set in system object
    m_system->set_replication_mngr_iogr (m_replication_manager_IOGR);
    m_system->set_replication_mngr (m_replication_mngr.in());
            
    // get the fault notifier reference
    m_fault_notifier = m_replication_mngr->get_fault_notifier();
    
    // get the fault detector reference
    m_fault_detector = m_replication_mngr->get_fault_detector();

    // set the fault detector in system host container
    (m_system->get_actual_host_container())->setFaultDetector (m_fault_detector.in());
    
    
    
    // Create the FTConverter servant
    m_ft_converter = new FTConverter_impl (m_system->get_host_container_impl());				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var ft_converter_servant = m_ft_converter;        
    // activate object
    m_ft_converter_obj = m_ft_converter->_this();
    
    // set FTConverter object in System object
    m_system->set_ft_converter_object (m_ft_converter_obj.in());
        
        
    
    // Set FTConverter object in ft group
    ::FT::ObjectGroup_var ft_converter_group;
        
    // get FTConverter Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_FTConverter_group_name().c_str();
        
        ft_converter_group = 
              m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "FTConverter", "group undefined");
    }
        
    // add the FTConverter object in group
    std::cout << "call add_member on ft_converter_group" << std::endl;
    try
    {
        ft_converter_group = m_replication_mngr->add_member(
                                                    ft_converter_group.in(),
                                                    smg_location,
                                                    m_ft_converter_obj.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "FTConverter", "group NotFound");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "FTConverter", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "FTConverter", "group ObjectNotAdded");
    }
       
    
    m_ft_converter_obj_group = 
          CdmwPlatformMngt::FTConverter::_narrow (ft_converter_group.in());
          
      
    //    
    // Create the FT ProcessObserver
    //
        
    // Create the FTProcessObserver servant
    m_ft_process_observer = new FTProcessObserver_impl (m_poa.in(), m_system, 
                                                       element_repository);	
                                                      			             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var ft_process_observer_servant = m_ft_process_observer;        
    // activate object
    m_ft_process_observer_obj = m_ft_process_observer->_this();
    
    // Set FTProcessObserver object in ft group
    ::FT::ObjectGroup_var ft_process_observer_group;
        
    // get FTProcessObserver Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_FTProcessObserver_group_name().c_str();
        
        ft_process_observer_group = 
              m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "FTProcessObserver", "group undefined");
    }
        
    // add the FTProcessObserver object in group
    std::cout << "call add_member on ft_process_observer_group" << std::endl;
    try
    {
        ft_process_observer_group = m_replication_mngr->add_member(
                                                    ft_process_observer_group.in(),
                                                    smg_location,
                                                    m_ft_process_observer_obj.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "FTProcessObserver", "group NotFound");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "FTProcessObserver", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "FTProcessObserver", "group ObjectNotAdded");
    }
                                                    
    m_ft_process_observer_obj_group =
           CdmwPlatformMngt::ProcessObserver::_narrow (ft_process_observer_group.in());
                                                    
    // set ProcessObserver group in System object
    m_system->set_ft_process_observer_group(m_ft_process_observer_obj_group.in());
    
    
    //    
    // Create the FT ServiceBroker
    //
          
    // Create the FTServiceBroker servant
    m_ft_service_broker = new FTServiceBroker_impl (m_poa.in(), m_system, 
                                                    m_system->get_service_binding());				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var ft_service_broker_servant = m_ft_service_broker;        
    // activate object
    m_ft_service_broker_obj = m_ft_service_broker->_this();
    
    // set ReplicationManager global service in FTServiceBroker
    m_ft_service_broker->set_global_service(
                       "ReplicationManager", m_replication_mngr.in());
    
    // Set FTServiceBroker object in ft group
    ::FT::ObjectGroup_var ft_service_broker_group;
        
    // get FTServiceBroker Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_FTServiceBroker_group_name().c_str();
        
        ft_service_broker_group = 
              m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "FTServiceBroker", "group undefined");
    }
        
    // add the FTServiceBroker object in group
    std::cout << "call add_member on ft_service_broker_group" << std::endl;
    try
    {
       ft_service_broker_group = m_replication_mngr->add_member(
                                                    ft_service_broker_group.in(),
                                                    smg_location,
                                                    m_ft_service_broker_obj.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "FTServiceBroker", "group notFound");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "FTServiceBroker", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "FTServiceBroker", "group ObjectNotAdded");
    }
                                                    
    m_ft_service_broker_obj_group =
        CdmwPlatformMngtService::ServiceBroker::_narrow (ft_service_broker_group.in());
                                                    
    // set ServiceBroker group in System object
    m_system->set_ft_service_broker_group(m_ft_service_broker_obj_group.in());
    
    
    
    //    
    // Create the host accessor
    //
              
    // Create the host accessor servant
    m_host_accessor = new HostAccessor_impl (element_repository);				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var host_accessor_servant = m_host_accessor;        
    // activate object
    m_host_accessor_obj_group = m_host_accessor->_this();
        
    // Set Host Accessor object in ft group
    ::FT::ObjectGroup_var host_accessor_group;
            
    // get Host Accessor Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_HostAccessor_group_name().c_str();
            
        host_accessor_group = 
                  m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "HostAccessor", "group undefined");
    }
            
    // add the Host Accessor object in group
        std::cout << "call add_member on host_accessor_group" << std::endl;
    try
    {
        host_accessor_group = m_replication_mngr->add_member(
                                                        host_accessor_group.in(),
                                                        smg_location,
                                                        m_host_accessor_obj_group.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "HostAccessor", "group notFound");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "HostAccessor", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "HostAccessor", "group ObjectNotAdded");
    }
                                                        
    m_host_accessor_obj_group =
            CdmwFTSystMngt::HostAccessor::_narrow (host_accessor_group.in());
            
    //    
    // Create the application accessor
    //
              
    // Create the application accessor servant
    m_application_accessor = new ApplicationAccessor_impl (element_repository);				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var application_accessor_servant = m_application_accessor;        
    // activate object
    m_application_accessor_obj_group = m_application_accessor->_this();
        
    // Set Application Accessor object in ft group
    ::FT::ObjectGroup_var application_accessor_group;
            
    // get Application Accessor Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_ApplicationAccessor_group_name().c_str();
            
        application_accessor_group = 
                  m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "ApplicationAccessor", "group undefined");
    }
            
    // add the Application Accessor object in group
        std::cout << "call add_member on application_accessor_group" << std::endl;
    try
    {
        application_accessor_group = m_replication_mngr->add_member(
                                                        application_accessor_group.in(),
                                                        smg_location,
                                                        m_application_accessor_obj_group.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "ApplicationAccessor", "group undefined");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "ApplicationAccessor", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "ApplicationAccessor", "group ObjectNotAdded");
    }
                                                        
    m_application_accessor_obj_group =
            CdmwFTSystMngt::ApplicationAccessor::_narrow (application_accessor_group.in());
            
    //    
    // Create the process accessor
    //
              
    // Create the process accessor servant
    m_process_accessor = new ProcessAccessor_impl (element_repository);				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var process_accessor_servant = m_process_accessor;        
    // activate object
    m_process_accessor_obj_group = m_process_accessor->_this();
        
    // Set Process Accessor object in ft group
    ::FT::ObjectGroup_var process_accessor_group;
            
    // get Process Accessor Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_ProcessAccessor_group_name().c_str();
            
        process_accessor_group = 
                  m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "ProcessAccessor", "group undefined");
    }
            
    // add the Process Accessor object in group
        std::cout << "call add_member on process_accessor_group" << std::endl;
    try
    {
        process_accessor_group = m_replication_mngr->add_member(
                                                        process_accessor_group.in(),
                                                        smg_location,
                                                        m_process_accessor_obj_group.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "ProcessAccessor", "group notFound");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "ProcessAccessor", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "ProcessAccessor", "group ObjectNotAdded");
    }
                                                        
    m_process_accessor_obj_group =
            CdmwFTSystMngt::ProcessAccessor::_narrow (process_accessor_group.in());
            
    //    
    // Create the entity accessor
    //
              
    // Create the entity accessor servant
    m_entity_accessor = new EntityAccessor_impl (element_repository);				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var entity_accessor_servant = m_entity_accessor;        
    // activate object
    m_entity_accessor_obj_group = m_entity_accessor->_this();
        
    // Set Entity Accessor object in ft group
    ::FT::ObjectGroup_var entity_accessor_group;
            
    // get Entity Accessor Group Id
    try
    {
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id=Configuration::Get_EntityAccessor_group_name().c_str();
            
        entity_accessor_group = 
                  m_replication_mngr->get_object_group_ref_from_name(group_name);
    }
    catch(const ::FT::ObjectGroupNotFound&)
    {
        CDMW_THROW2 (BadParameterException, "EntityAccessor", "group undefined");
    }
            
    // add the Entity Accessor object in group
        std::cout << "call add_member on entity_accessor_group" << std::endl;
    try
    {
        entity_accessor_group = m_replication_mngr->add_member(
                                                        entity_accessor_group.in(),
                                                        smg_location,
                                                        m_entity_accessor_obj_group.in());
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_THROW2 (BadParameterException, "EntityAccessor", "group undefined");
    }
    catch (const ::FT::MemberAlreadyPresent&)
    {
        CDMW_THROW2 (BadParameterException, "EntityAccessor", "group MemberAlreadyPresent");
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        CDMW_THROW2 (BadParameterException, "EntityAccessor", "group ObjectNotAdded");
    }
                                                        
    m_entity_accessor_obj_group =
            CdmwFTSystMngt::EntityAccessor::_narrow (entity_accessor_group.in());
}


SystemAccessor_impl::~SystemAccessor_impl()
{
}

void SystemAccessor_impl::connect_ft_converter()
{
    try
    {
        CosNotifyComm::StructuredPushConsumer_var system_consumer;
    
        // narrow ft converter object group in system consumer                                                        
        system_consumer =
           CosNotifyComm::StructuredPushConsumer::_narrow(m_ft_converter_obj_group.in());
      

        // connect fault converter to fault notifier
        // this connection must be done one time by the first primary
        // (called by associated datastore oninit when add_member of this object is called)
    
        m_fault_notifier->connect_structured_fault_consumer(system_consumer.in());
    }
    catch (...)
    {
        std::cout << "SystemAccessor_impl::connect_ft_converter() Unexpected exception" 
                  << std::endl;
    }    
}

CORBA::ULong SystemAccessor_impl::get_number_of_applications() throw (CORBA::SystemException)
  {
  
   return m_system->get_number_of_applications();
  }

CdmwFTSystMngt::ApplicationAccessor_ptr SystemAccessor_impl::get_application_accessor()
throw (CORBA::SystemException)
{
    CdmwFTSystMngt::ApplicationAccessor_var application_accessor = m_application_accessor_obj_group;
    
    return application_accessor._retn();
}

      

CdmwFTSystMngt::ProcessAccessor_ptr SystemAccessor_impl::get_process_accessor()
throw (CORBA::SystemException)
{
	CdmwFTSystMngt::ProcessAccessor_var process_accessor = m_process_accessor_obj_group;
    
    return process_accessor._retn();
}
      
      

CdmwFTSystMngt::HostAccessor_ptr SystemAccessor_impl::get_host_accessor()
throw (CORBA::SystemException)
{
	CdmwFTSystMngt::HostAccessor_var host_accessor = m_host_accessor_obj_group;
    
    return host_accessor._retn();
}
        

CdmwFTSystMngt::EntityAccessor_ptr SystemAccessor_impl::get_entity_accessor()
throw (CORBA::SystemException)
{
	CdmwFTSystMngt::EntityAccessor_var entity_accessor = m_entity_accessor_obj_group;
    
    return entity_accessor._retn();
}    

char * SystemAccessor_impl::get_system_element_path ()
      throw (CORBA::SystemException)
{
    // return the system element path 
    // (empty string is returned if system is not yet defined)
    
    return m_system->element_path();
}


    
CdmwPlatformMngt::SystemStatus SystemAccessor_impl::get_status(CORBA::String_out status_info)
throw (CORBA::SystemException)
{
	return m_system->get_status(status_info);
}


char * SystemAccessor_impl::get_system_mission() throw (CORBA::SystemException)
{
	return m_system->get_system_mission();
}

void SystemAccessor_impl::set_system_mission(const char * system_mission) throw (CORBA::SystemException)
{
	return m_system->set_system_mission(system_mission);
}

char * SystemAccessor_impl::get_system_name() throw (CORBA::SystemException)
{
	return m_system->get_system_name();
}

void SystemAccessor_impl::set_system_name(const char * system_name) throw (CORBA::SystemException)
{
	 m_system->set_system_name(system_name);
}

char * SystemAccessor_impl::get_master_host() throw (CORBA::SystemException)
{
	return m_system->get_master_host();
}


CdmwPlatformMngt::FunctioningMode SystemAccessor_impl::get_mode(CORBA::String_out mode_info)
throw (CORBA::SystemException)
{
	return m_system->get_mode(mode_info);
}



CdmwPlatformMngt::SystemSnapshot* SystemAccessor_impl::get_snapshot()
throw (CORBA::SystemException)
{
	return m_system->get_snapshot();
}


CdmwPlatformMngt::ElementPaths* SystemAccessor_impl::get_applications (
                          CORBA::ULong how_many,
                          CdmwPlatformMngt::ElementPathsIterator_out applications_iterator)
throw (CORBA::SystemException)
{
    // get the list of application references
    CdmwPlatformMngt::ApplicationsIterator_var applicationsRemainingIt;
    
	CdmwPlatformMngt::Applications_var applications_seq =
	            m_system->get_applications (how_many, applicationsRemainingIt.out());
	            
    // convert each application reference in element path
    
    // create sequence of element path
    CdmwPlatformMngt::ElementPaths_var elements =
                          new CdmwPlatformMngt::ElementPaths();
    
    // set element sequence length 
    size_t seq_len = applications_seq->length();
    elements->length(seq_len);
    
    // fill element sequence
    unsigned int count;
    for (count=0 ; count < seq_len ; count++)
    {
        // Store application path in sequence
        // path is allocated by element_path()
        elements[count] = (applications_seq[count])->element_path();                                           
    }
        
    // Get an iterator on the rest of the elements
    std::list<CdmwPlatformMngt::ElementPath_var> elementsRemaining;
    CdmwPlatformMngt::Application_var application;
    
    // fore each element contained in reference iterator
    while (applicationsRemainingIt->next_one (application.out()))
    { 
        elementsRemaining.push_back (application->element_path());
    }
    
    // create the path iterator
    CdmwPlatformMngt::ElementPathsIterator_var elementsRemainingIt =
              ElementPathsIterator_impl::create (elementsRemaining);

    // Assign the iterator to the processes_iterator
    applications_iterator = elementsRemainingIt._retn();

    return elements._retn();

}





char* SystemAccessor_impl::get_application (const char* application_name)                          
throw (CdmwPlatformMngt::ApplicationNotFound,
       CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application = 
	        m_system->get_application (application_name);
	        
    return application->element_path();
}


CdmwPlatformMngt::SupervisionObserver_ptr 
SystemAccessor_impl::register_observer(
                      const char* observer_name,
                      CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw (CORBA::SystemException)
{
	return m_system->register_observer(observer_name, observer);
}

CdmwPlatformMngt::SupervisionObserver_ptr 
SystemAccessor_impl::register_proxy_observer(
                      const char* observer_name,
                      const char* host_name,
                      CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw (CORBA::SystemException)
{
	return m_system->register_proxy_observer(observer_name, host_name, observer);
}


CdmwPlatformMngt::SupervisionObserver_ptr 
SystemAccessor_impl::unregister_observer(const char* observer_name)
throw (CdmwPlatformMngt::ObserverNotFound,
       CORBA::SystemException)
{
	return m_system->unregister_observer(observer_name);
}


void SystemAccessor_impl::define(const CdmwPlatformMngt::SystemDef& system_def)
throw (CdmwPlatformMngt::DuplicateEntity,
       CosPropertyService::MultipleExceptions,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateHost,
       CdmwPlatformMngt::DuplicateHostEntity,
       CdmwPlatformMngt::HostPropertyExceptions,
       CdmwPlatformMngt::AlreadyDone,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->define(system_def);
}


void SystemAccessor_impl::set_init_graph (
                     const CdmwPlatformMngt::GraphElements& application_graph)                  
throw (CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->set_init_graph(application_graph);
}


void SystemAccessor_impl::set_stop_graph (
                     const CdmwPlatformMngt::GraphElements& application_graph)                  
throw (CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->set_stop_graph(application_graph);
}



void SystemAccessor_impl::set_run_set(const CdmwPlatformMngt::RunSetDef& run_set_def)
throw (CdmwPlatformMngt::DuplicateRunSetElement,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->set_run_set(run_set_def);
}

void SystemAccessor_impl::remove_run_set(const char* run_set_name)
throw (CdmwPlatformMngt::RunSetNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->remove_run_set(run_set_name);
}

void SystemAccessor_impl::remove_all_run_sets()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    m_system->remove_all_run_sets();
}

char* SystemAccessor_impl::add_host(const CdmwPlatformMngt::HostDef& host_def)
throw (CdmwPlatformMngt::HostAlreadyExists,
       CdmwPlatformMngt::DuplicateEntity, 
       CosPropertyService::MultipleExceptions,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    CdmwPlatformMngt::Host_var host = 
	        m_system->add_host(host_def);
	        
    return host->element_path();
	
}

void SystemAccessor_impl::remove_host(const char* host_name)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->remove_host(host_name);
}                  

 
char* SystemAccessor_impl::add_application(
                     const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                     const CdmwPlatformMngt::ApplicationDef& application_def)
throw (CdmwPlatformMngt::ApplicationAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CosPropertyService::MultipleExceptions,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateProcess,
       CdmwPlatformMngt::ProcessHostNotFound,
       CdmwPlatformMngt::DuplicateProcessEntity,
       CdmwPlatformMngt::ProcessPropertyExceptions,
       CdmwPlatformMngt::DuplicateProcessService,
       CdmwPlatformMngt::DuplicateProcessStep,
       CdmwPlatformMngt::DuplicateProcessCommand, 
       CdmwPlatformMngt::DuplicateProcessCommandArg,
       CdmwPlatformMngt::DuplicateEntityCommand,
       CdmwPlatformMngt::DuplicateEntityCommandArg,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application = 
	        m_system->add_application(platform_application_def, application_def);
	        
    return application->element_path();
}

void SystemAccessor_impl::remove_application(const char* application_name)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::ApplicationStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->remove_application(application_name);
} 

void SystemAccessor_impl::start (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->start(startup_kind);
}
    

void SystemAccessor_impl::resume()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->resume();
}


void SystemAccessor_impl::acknowledge()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->acknowledge();
}


void SystemAccessor_impl::stop (CORBA::Boolean emergency)
throw (CORBA::SystemException)      
{
	m_system->stop(emergency);
}


void SystemAccessor_impl::cold_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->cold_restart(startup_kind);
}

void SystemAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
m_system->get_attribute(attr_name, ret_attr, flag);
}

void SystemAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{

m_system->set_attribute(attr_name , attr, flag) ;
}

void SystemAccessor_impl::hot_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
	m_system->hot_restart(startup_kind);
}
             

} // End namespace PlatformMngt
} // End namespace Cdmw

