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

#include <sstream>
#include <fstream>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Assert.hpp"

#include <faulttoleranceserver/TestHello_impl.hpp>
#include <FaultTolerance/idllib/CdmwFTCurrent.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>

namespace Cdmw
{
namespace Test
{


HelloInterface1_impl::HelloInterface1_impl(CORBA::ORB_ptr orb, const std::string& name, const std::string& hostname_file_location)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_nb_hello(0),
      m_flags(false),
      m_counter(0),
      m_client_id(""),
      m_retention_id(0),
      m_expiration_time(0),
      m_hostname_file_location(hostname_file_location)
{ 
    m_process_info = new Cdmw::Test::process_info();
    m_process_info->name = name.c_str();
    m_process_info->processID = Cdmw::OsSupport::OS::get_processId();
}


HelloInterface1_impl::~HelloInterface1_impl()
    throw()
{
}

void HelloInterface1_impl::hello ()
     throw(CORBA::SystemException)                  
{
    m_nb_hello++;
    std::cout << m_process_info->name << " : "  << m_process_info->processID  << " : "  <<"<####### HELLO " << m_nb_hello <<"  ######>" << std::endl;
}

Cdmw::Test::process_info* HelloInterface1_impl::who_is_called()
    throw(CORBA::SystemException)
{
//     std::cout<< "*****************************************************************"<<std::endl;
    std::cout<<m_process_info->name <<" is called"<<std::endl;
//     std::cout<< "*****************************************************************"<<std::endl;

    Cdmw::Test::process_info_var process_info;
    Cdmw::Test::process_info info;
    info.name = m_process_info->name;
    info.processID = m_process_info->processID;
    process_info = new Cdmw::Test::process_info(info);

    return process_info._retn();
 
    
}

int HelloInterface1_impl::send_exception(int exception_number)
    throw(CORBA::SystemException)
{
    CORBA::ULong minor_code = 0;
    CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");
    CdmwFT::Current_var  ft_current = CdmwFT::Current::_narrow(obj.in());

    std::cout<<std::endl<<"client_id :"<<ft_current->get_client_id()<<" retention_id :"<<ft_current->get_retention_id()
             <<" expiration_time :"<<ft_current->get_expiration_time()<<std::endl<<std::endl;

    if (exception_number == 11)
    {
        std::cout<<"notify error"<<std::endl;
        Cdmw::PlatformMngt::PlatformInterface::Notify_fatal_error(m_process_info->name, "fatal error");
    }
    
    if (exception_number == 9)
    {
        throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_NO);
        //throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_NO);
    }
    
    if (exception_number == 10)
    {

        std::cout<< "##########"<<m_process_info->name <<" is called##########"<<std::endl;

        unsigned long nb_assert = 0;
        std::string absolute_file_path;
        try
        {
            absolute_file_path = Cdmw::OsSupport::OS::get_absolute_path(".");
        }
        catch (const Cdmw::BadParameterException& )
        {
            std::cerr << "##########"<< m_process_info->name 
                      << "get_absolute_path raises BadParameterException"
                      << std::ends;
        }
        catch (const Cdmw::PermissionDeniedException& )
        {
            std::cerr << "##########"<< m_process_info->name 
                      << "get_absolute_path raises PermissionDeniedException"
                      << std::ends;
        }
        catch (const Cdmw::NotExistException& )
        {
            std::cerr << "##########"<< m_process_info->name 
                      << "get_absolute_path raises NotExistException"
                      << std::ends;
        }
        catch (const Cdmw::InternalErrorException& )
        {
            std::cerr << "##########"<< m_process_info->name 
                      << "get_absolute_path raises InternalErrorException"
                      << std::ends;
        }
        std::ostringstream oss;
        oss << "/net/" << m_hostname_file_location << absolute_file_path 
            << "hello.validation.stt" << std::ends;
        std::cout << absolute_file_path << std::endl;
        std::cout << oss.str() << std::endl;

        std::ifstream ifs(oss.str().c_str());
        if (ifs.good())
        {
            ifs >> nb_assert;       
            std::cout << "##########"  << m_process_info->name 
                      <<" nb assert= " << nb_assert << std::endl;
            ifs.close();
        }
        
        
        if (nb_assert > 1)
        {
            std::ofstream ofs(oss.str().c_str());
            ofs <<"0";
            std::cout << "##########"  << m_process_info->name 
                      <<" nb assert stored= 0" << std::endl;
            ofs.close();
        } else {
            std::ofstream ofs(oss.str().c_str());
            nb_assert ++;
            ofs <<nb_assert;
            std::cout << "##########"  << m_process_info->name 
                      <<" nb assert stored= " << nb_assert << std::endl;
            ofs.close();
            std::cout<< "##########"<<m_process_info->name <<" abort => COMM_FAILURE is raised"<<std::endl;
            CDMW_ASSERT(0);
        }
        
    }
    
    if (m_flags == false)
    {
        // initialise the counter 
        m_counter = 1;
        m_flags = true;
        // get the information about this request from the FTCurrent
        // and check if it's different from the previous requests
        if ((m_client_id == ft_current->get_client_id()) && (m_retention_id == ft_current->get_retention_id()))
        {
            std::cout<<"TEST_FAILED : The client_id and the retention_id are not the same as the previous request"<<std::endl;
            return 0;
        }
            
        m_client_id = ft_current->get_client_id();
        m_retention_id = ft_current->get_retention_id();
        m_expiration_time = ft_current->get_expiration_time();
        // send an exception
        
        switch (exception_number) 
        {
            case 1:
                {
                    std::cout<<"throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_NO);
                }
            case 2:
                {
                    std::cout<<"throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_MAYBE);
                }
            case 3:
                {
                    std::cout<<"throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_NO);
                }
            case 4:
                {
                    std::cout<<"throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_MAYBE);
                }
            case 5:
                {
                    std::cout<<"throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_NO);
                }
            case 6:
                {
                    std::cout<<"throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_MAYBE);
                }
            case 7:
                {
                    std::cout<<"throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_NO);
                }
            case 8:
                {
                    std::cout<<"throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_MAYBE);
                }
        }
        
    } else {
        m_flags = false;
        // increase the counter 
        m_counter++;
        // check the information about the previous FTRequestServiceContext and the present
        // if there are different, it's not OK, so, the counter is RAZ
        // the client test must be failed
        if (m_client_id != ft_current->get_client_id())
            m_counter = 0;
        if (m_retention_id != ft_current->get_retention_id())
            m_counter = 0;
        if (m_expiration_time != ft_current->get_expiration_time())
            m_counter = 0;
        return m_counter;
    }

    return 0;
}

::FT::FTRequestServiceContext *
HelloInterface1_impl::request_identification()
    throw(CORBA::SystemException)
{
    CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");
    CdmwFT::Current_var  ft_current = CdmwFT::Current::_narrow(obj.in());

    std::cout<<"client_id :"<<ft_current->get_client_id()
             <<" retention_id :"<<ft_current->get_retention_id()
             <<" expiration_time :"<<ft_current->get_expiration_time()<<std::endl;


    FT::FTRequestServiceContext_var identificator = new FT::FTRequestServiceContext();
    identificator->client_id = ft_current->get_client_id();
    identificator->retention_id = ft_current->get_retention_id();
    identificator->expiration_time = ft_current->get_expiration_time();
    
    
    return identificator._retn();
    
    
}

int 
HelloInterface1_impl::group_version_identification()
    throw(CORBA::SystemException)
{
    CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");
    CdmwFT::Current_var  ft_current = CdmwFT::Current::_narrow(obj.in());
    
    std::cout<<"group_version : "<<ft_current->get_object_group_ref_version()<<std::endl;    
    return ft_current->get_object_group_ref_version();

}

FT::ObjectGroup*
HelloInterface1_impl::get_objectgroup(::FT::ObjectGroupId id)
    throw(CORBA::SystemException)
{
    CdmwFT::Location::GroupRepository_var group_repository;
    ::FT::ObjectGroup_var group_ref = ::FT::ObjectGroup::_nil();
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTGroupRepository");        
        group_repository = CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "ERROR: Impossible to resolve FTGroupRepository! Name is invalid!"
                  << std::endl;
    }  
    group_ref = group_repository->get_object_group_from_gid(id);
    return group_ref._retn();
    

}


HelloInterface2_impl::HelloInterface2_impl(CORBA::ORB_ptr orb, std::string name)
    throw(CORBA::SystemException)
    : m_nb_hello(0)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process_info = new Cdmw::Test::process_info();
    m_process_info->name = name.c_str();
    m_process_info->processID = Cdmw::OsSupport::OS::get_processId();

}


HelloInterface2_impl::~HelloInterface2_impl()
    throw()
{
}

void HelloInterface2_impl::hello ()
     throw(CORBA::SystemException)                  
{
    m_nb_hello++;
    std::cout <<  m_process_info->name << " : "  << m_process_info->processID  << " : "  <<"<####### HELLO " << m_nb_hello <<"  ######>" << std::endl;
}

Cdmw::Test::process_info* HelloInterface2_impl::who_is_called()
    throw(CORBA::SystemException)
{

    std::cout<< m_process_info->name <<" is called"<<std::endl;
    Cdmw::Test::process_info_var process_info;
    Cdmw::Test::process_info info;
    info.name = m_process_info->name;
    info.processID = m_process_info->processID;
    process_info = new Cdmw::Test::process_info(info);

    return process_info._retn(); 
    
}



HelloInterface3_impl::HelloInterface3_impl(CORBA::ORB_ptr orb, std::string name)
    throw(CORBA::SystemException)
    : m_nb_hello(0)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process_info = new Cdmw::Test::process_info();
    m_process_info->name = name.c_str();
    m_process_info->processID = Cdmw::OsSupport::OS::get_processId();

}


HelloInterface3_impl::~HelloInterface3_impl()
    throw()
{
}

void HelloInterface3_impl::hello ()
     throw(CORBA::SystemException)                  
{
    m_nb_hello++;
    std::cout << m_process_info->name << " : "  << m_process_info->processID  << " : "  <<"<####### HELLO " << m_nb_hello <<"  ######>" << std::endl;
}

Cdmw::Test::process_info* HelloInterface3_impl::who_is_called()
    throw(CORBA::SystemException)
{

    std::cout<< m_process_info->name <<" is called"<<std::endl;
    Cdmw::Test::process_info_var process_info;
    Cdmw::Test::process_info info;
    info.name = m_process_info->name;
    info.processID = m_process_info->processID;
    process_info = new Cdmw::Test::process_info(info);

    return process_info._retn(); 
    
}


}; // End namespace Test

}; // End namespace Cdmw

