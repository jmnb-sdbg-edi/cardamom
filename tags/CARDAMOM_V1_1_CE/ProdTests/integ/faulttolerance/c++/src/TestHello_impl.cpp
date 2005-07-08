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

// Cdmw Files
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "faulttolerance/TestHello_impl.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTCurrent.stub.hpp"




namespace Cdmw {
namespace Test {


HelloInterface_impl::HelloInterface_impl(
    CORBA::ORB_ptr orb,
    std::string name )
throw( CORBA::SystemException )
:  m_nb_hello(0),
   m_flags( false),
   m_counter( 0 ),
   m_client_id( "" ),
   m_retention_id( 0 ),
   m_expiration_time( 0 )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process_info = new Cdmw::Test::process_info();
        m_process_info->name = name.c_str();
        m_process_info->processID = Cdmw::OsSupport::OS::get_processId();
}

HelloInterface_impl::~HelloInterface_impl()
throw()
{
}

void HelloInterface_impl::hello ()
throw( CORBA::SystemException )
{
    m_nb_hello++;

    // export the the state to a file
    std::cout
        << m_process_info->name <<"Saving hello interface state"
        << std::endl;    
    std::ofstream os("hello.stt");
    os << m_nb_hello;
    os.close();

    std::cout
        << m_process_info->name <<"<####### HELLO "
        << m_nb_hello <<"  ######>"
        << std::endl;
}

Cdmw::Test::process_info* HelloInterface_impl::who_is_called()
throw( CORBA::SystemException )
{
    std::cout << "####### " << m_process_info->name <<" is called"<< std::endl;

    /******
    Cdmw::Test::process_info info;
        info.name = m_process_info->name;
        info.processID = m_process_info->processID;
    Cdmw::Test::process_info_var process_info;
        process_info = new Cdmw::Test::process_info( info );
    ******/

    Cdmw::Test::process_info_var process_info =
            new Cdmw::Test::process_info();

    process_info->name = m_process_info->name;
    process_info->processID = m_process_info->processID;

    return process_info._retn();
}

int HelloInterface_impl::send_exception(
    int exception_number )
throw( CORBA::SystemException )
{
    CORBA::ULong minor_code = 0;
    CORBA::Object_var obj = m_orb->resolve_initial_references( "FTCurrent" );
    CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow( obj.in() );

    std::cout
        << "client_id :" << ft_current->get_client_id()
        << " retention_id :" << ft_current->get_retention_id()
        << " expiration_time :" << ft_current->get_expiration_time()
        << std::endl;
    
    if( m_flags == false )
    {
        // initialise the counter 
        m_counter = 1;
        m_flags = true;

        // get the information about this request from the FTCurrent
        // and check if it's different from the previous requests
        if( (m_client_id == ft_current->get_client_id()) &&
            (m_retention_id == ft_current->get_retention_id()) )
        {
            std::cout
                << "TEST_FAILED : The client_id and the retention_id are "
                << "the same as the previous request"
                << std::endl;
            return 0;
        }
            
        m_client_id = ft_current->get_client_id();
        m_retention_id = ft_current->get_retention_id();
        m_expiration_time = ft_current->get_expiration_time();

        // send an exception
        switch( exception_number )
        {
            case 1:
                throw CORBA::COMM_FAILURE(
                    minor_code, CORBA::COMPLETED_NO );
            case 2:
                throw CORBA::COMM_FAILURE(
                    minor_code, CORBA::COMPLETED_MAYBE );
            case 3:
                throw CORBA::TRANSIENT(
                    minor_code, CORBA::COMPLETED_NO );
            case 4:
                throw CORBA::TRANSIENT(
                    minor_code, CORBA::COMPLETED_MAYBE );
            case 5:
                throw CORBA::NO_RESPONSE(
                    minor_code, CORBA::COMPLETED_NO );
            case 6:
                throw CORBA::NO_RESPONSE(
                    minor_code, CORBA::COMPLETED_MAYBE );
            case 7:
                throw CORBA::OBJ_ADAPTER(
                    minor_code, CORBA::COMPLETED_NO );
            case 8:
                throw CORBA::OBJ_ADAPTER(
                    minor_code, CORBA::COMPLETED_MAYBE );
        }
        
    }
    else
    {
        m_flags = false;

        // increase the counter 
        m_counter++;

        // check the information about the previous FTRequestServiceContext
        // and the present if there are different, it's not OK, so, the
        // counter is RAZ the client test must be failed
        if( m_client_id != ft_current->get_client_id() )
            m_counter = 0;
        if( m_retention_id != ft_current->get_retention_id() )
            m_counter = 0;
        if( m_expiration_time != ft_current->get_expiration_time() )
            m_counter = 0;

        return m_counter;
    }

    return 0;
}

void HelloInterface_impl::set_nb_hello(unsigned long nb_hello)
{
    m_nb_hello = nb_hello;
}

}; // End namespace Test
}; // End namespace Cdmw

