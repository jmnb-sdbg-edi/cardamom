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

#include <fstream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include <testftinit/TestHello_impl.hpp>
#include <idllib/CdmwFTActivation.skel.hpp>

#include <idllib/CdmwFTCurrent.stub.hpp>



namespace Cdmw
{
namespace Test
{


    const CORBA::Long HelloInterface_impl::MaxCycles(1000);


HelloInterface_impl::HelloInterface_impl(CORBA::ORB_ptr orb, 
                                         std::string location,
                                         ::CdmwFT::Location::ActivationHandler_ptr activation_handler )
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_nb_hello(0),
      m_location(location),
      m_activation_handler(::CdmwFT::Location::ActivationHandler::_duplicate(activation_handler)),
      m_counter(0),
      m_cycle(0), 
      m_results(new ::CdmwFT::Test::ResultsSeq(HelloInterface_impl::MaxCycles))
{ 
}


HelloInterface_impl::~HelloInterface_impl()
    throw()
{
}

void 
HelloInterface_impl::hello ()
     throw(CORBA::SystemException)                  
{
    // export the location to a file
    std::ofstream os("hello.stt");
    os << m_location;
    os.close();

    m_nb_hello++;
    std::cout << m_location << " <####### HELLO " << m_nb_hello <<"  ######>" << std::endl;

    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        ::FT::FTDomainId ft_domain_id = ft_current->get_ft_domain_id();
        std::cout << "    ft_domain_id = " << ft_domain_id << std::endl;

        ::FT::ObjectGroupId group_id = ft_current->get_object_group_id();
        std::cout << "    group_id = " << group_id << std::endl;

        ::FT::ObjectGroupRefVersion group_ref_version 
            = ft_current->get_object_group_ref_version();
        std::cout << "    group_ref_version = " << group_ref_version << std::endl;

        std::string client_id = ft_current->get_client_id();
        std::cout << "    client_id = " << client_id << std::endl;

        CORBA::Long retention_id = ft_current->get_retention_id();
        std::cout << "    retention_id = " << retention_id << std::endl;

        TimeBase::TimeT expiration_time = ft_current->get_expiration_time();
        std::cout << "    expiration_time = " << expiration_time << std::endl;
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}

void 
HelloInterface_impl::init(::FT::ObjectGroup_ptr group_ref)
     throw(CORBA::SystemException)                  
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTActivationManager");
        CdmwFT::Location::ActivationManager_var  activation_manager 
            = CdmwFT::Location::ActivationManager::_narrow(obj.in());
        
        activation_manager->register_activation_handler(group_ref, 
                                                        m_activation_handler.in()); 
    }
    catch (const FT::ObjectGroupNotFound&)
    {
        std::cerr << "ERROR: Impossible to register FTActivationHandler! Object Group not found!"
            << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CdmwFT::Location::HandlerAlreadyExist&)
    {
        std::cerr << "ERROR: Impossible to register FTActivationHandler! Handler already exists!"
            << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "ERROR: Impossible to register FTActivationManager! Name is invalid!"
            << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        
        std::cerr << "ERROR: Impossible to register FTActivationHandler!"
            << std::endl;
        std::cerr << e << std::endl;
        throw;
    }
}

::FT::FTDomainId
HelloInterface_impl::get_ft_domain_id()
    throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException)
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        ::FT::FTDomainId ft_domain_id = ft_current->get_ft_domain_id();
        std::cout << "    ft_domain_id = " << ft_domain_id << std::endl;

        return ft_domain_id;
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        std::cerr << "NoContext exception raised!" << std::endl;
        throw CdmwFT::Test::HelloInterface::NoContext();
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}

::FT::ObjectGroupId
HelloInterface_impl::get_object_group_id()
    throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException)
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        ::FT::ObjectGroupId group_id = ft_current->get_object_group_id();
        std::cout << "    group_id = " << group_id << std::endl;

        return group_id;
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        std::cerr << "NoContext exception raised!" << std::endl;
        throw CdmwFT::Test::HelloInterface::NoContext();
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}

::FT::ObjectGroupRefVersion
HelloInterface_impl::get_object_group_ref_version()
    throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException)
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        ::FT::ObjectGroupRefVersion group_ref_version 
            = ft_current->get_object_group_ref_version();
        std::cout << "    group_ref_version = " << group_ref_version << std::endl;

        return group_ref_version;
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        std::cerr << "NoContext exception raised!" << std::endl;
        throw CdmwFT::Test::HelloInterface::NoContext();
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}

char*
HelloInterface_impl::get_client_id()
    throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException)
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        CORBA::String_var client_id = ft_current->get_client_id();
        std::cout << "    client_id = " << client_id.in() << std::endl;

        return client_id._retn();
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        std::cerr << "NoContext exception raised!" << std::endl;
        throw CdmwFT::Test::HelloInterface::NoContext();
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}

CORBA::Long
HelloInterface_impl::get_retention_id()
    throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException)
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        CORBA::Long retention_id = ft_current->get_retention_id();
        std::cout << "    retention_id = " << retention_id << std::endl;

        return retention_id;
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        std::cerr << "NoContext exception raised!" << std::endl;
        throw CdmwFT::Test::HelloInterface::NoContext();
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}

TimeBase::TimeT
HelloInterface_impl::get_expiration_time()
    throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException)
{
    try
    {
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");

        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        TimeBase::TimeT expiration_time = ft_current->get_expiration_time();
        std::cout << "    expiration_time = " << expiration_time << std::endl;

        return expiration_time;
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        std::cerr << "NoContext exception raised!" << std::endl;
        throw CdmwFT::Test::HelloInterface::NoContext();
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
}
        

int HelloInterface_impl::send_exception(int exception_number, int how_many)
    throw(CORBA::SystemException)
{

    CORBA::ULong minor_code = 0;
    m_counter++;
    if ((how_many != m_counter) || (how_many == 0))
    {
        switch (exception_number) 
        {
            case 1:
                {
                    std::cout<<"throw CORBA::COMM_FAILURE("<<minor_code<<", CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_NO);
                }
            case 2:
                {
                    std::cout<<"throw CORBA::COMM_FAILURE("<<minor_code<<", CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_MAYBE);
                }
            case 3:
                {
                    std::cout<<"throw CORBA::TRANSIENT("<<minor_code<<", CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_NO);
                }
            case 4:
                {
                    std::cout<<"throw CORBA::TRANSIENT("<<minor_code<<", CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_MAYBE);
                }
            case 5:
                {
                    std::cout<<"throw CORBA::NO_RESPONSE("<<minor_code<<", CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_NO);
                }
            case 6:
                {
                    std::cout<<"throw CORBA::NO_RESPONSE("<<minor_code<<", CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_MAYBE);
                }
            case 7:
                {
                    std::cout<<"throw CORBA::OBJ_ADAPTER("<<minor_code<<", CORBA::COMPLETED_NO)"<<std::endl;
                    throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_NO);
                }
            case 8:
                {
                    std::cout<<"throw CORBA::OBJ_ADAPTER("<<minor_code<<", CORBA::COMPLETED_MAYBE)"<<std::endl;
                    throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_MAYBE);
                }
        }
    }
    int tmp = m_counter;
    m_counter = 0;
    return tmp;
}

void HelloInterface_impl::send_TRANSIENT_exception (::CORBA::Long cycle)
    throw(CORBA::SystemException)
{
    throw CORBA::TRANSIENT(0, CORBA::COMPLETED_MAYBE);
}
      
::CdmwFT::Test::ResultsSeq * HelloInterface_impl::get_results ()
    throw(CORBA::SystemException)
{
    ::CdmwFT::Test::ResultsSeq_var res = new ::CdmwFT::Test::ResultsSeq(MaxCycles);
    res->length(m_results->length());
    for (CORBA::ULong i = 0L; i <m_results->length();++i)
        res[i]=m_results[i];
    return res._retn();
}


}; // End namespace Test

}; // End namespace Cdmw

