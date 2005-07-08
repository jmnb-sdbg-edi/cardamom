/* =========================================================================== *
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
 * =========================================================================== */


#ifndef ___HELLO_SERVER_EXECUTOR_IMPL_HPP__
#define ___HELLO_SERVER_EXECUTOR_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"

#include "UserHello.skel.hpp"
#include "DemoFTDCPS.h"


namespace Hello
{
    
//
// IDL:acme.com/Hello/CCM_Server:1.0
//
class ServerExecutor_impl : virtual public User_Server, 
                            virtual public Cdmw::OrbSupport::RefCountLocalObject
{
  public:

    ServerExecutor_impl();
    
    ~ServerExecutor_impl();

    void set_session_context(Components::SessionContext_ptr ctx)
		throw(CORBA::SystemException);
    void ccm_activate()
		throw(CORBA::SystemException);
    void ccm_passivate()
		throw(CORBA::SystemException);
    void ccm_remove()
		throw(CORBA::SystemException);
    
    //
    // IDL:acme.com/Hello/CCM_Server_Executor/name:1.0
    //
    virtual char* name()
        throw(CORBA::SystemException);
        
    virtual void name(const char*)
        throw(CORBA::SystemException);
    
    //
    // IDL:acme.com/Hello/CCM_Server_Executor/executable_name:1.0
    //
    virtual char* executable_name()
        throw(CORBA::SystemException);
        
    virtual void executable_name(const char*)
        throw(CORBA::SystemException);
    
    //
    // IDL:acme.com/Hello/CCM_Server_Executor/splice_port:1.0
    //
	virtual CORBA::Long splice_port ()
        throw(CORBA::SystemException);
    
    virtual void splice_port (CORBA::Long splice_port)
        throw(CORBA::SystemException);

    //
    // IDL:acme.com/Hello/CCM_Server/get_server_facet:1.0
    //
    virtual CCM_Display_ptr get_server_facet()
        throw(CORBA::SystemException);

	// State Replication operations
	DemoFTDCPS::ServerState read_state()
        throw(CORBA::SystemException);
	void write_state(const DemoFTDCPS::ServerState & state)
        throw(CORBA::SystemException);

  private:
  
    CCM_Display_var m_display;
    
    CORBA::String_var m_name;
    CORBA::String_var m_executable_name;
	CORBA::Long       m_splice_port;
	char**            m_argv;
        
	// DCPS-based state replication...
	// Splice DomainParticipantFactory
	DCPS::SpliceDomainParticipantFactory_var m_factory;
	
	// Splice domainParticipant
	DCPS::DomainParticipant_var              m_participant;
	
	// Splice Topic
	DCPS::Topic_var                          m_topic;
	
	// DCPS Subscriber
	DCPS::Subscriber_var                     m_subscriber;
	
	// DCPS Data Reader
	DCPS::DataReader_var                     m_data_reader;
	DemoFTDCPS::ServerStateDataReader_ptr    m_typed_data_reader;
		  
	// DCPS publisher
	DCPS::Publisher_var                      m_publisher;

	// DCPS data writer
	DCPS::DataWriter_var                     m_data_writer;
	DemoFTDCPS::ServerStateDataWriter_ptr    m_typed_data_writer;
    
    
    ServerExecutor_impl(const ServerExecutor_impl&);
    
    void operator=(const ServerExecutor_impl&);
        
};


}; // End of namespace Hello

#endif // ___HELLO_SERVER_EXECUTOR_IMPL_HPP__


