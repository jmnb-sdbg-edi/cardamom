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


#ifndef INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP 
#define INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP 


#include <testftinit/TestHello.skel.hpp>

#include <idllib/CdmwFTActivation.stub.hpp>



namespace Cdmw
{

namespace Test
{

class HelloInterface_impl :
public virtual POA_CdmwFT::Test::HelloInterface,
        virtual public PortableServer::RefCountServantBase
{

    public:

    static const CORBA::Long MaxCycles;

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        HelloInterface_impl(CORBA::ORB_ptr orb, 
                            std::string location,
                            CdmwFT::Location::ActivationHandler_ptr activation_handler)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~HelloInterface_impl()
                throw();
    
        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/hello:1.0
        //
        virtual void hello ()
            throw(CORBA::SystemException);
                  
        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/init:1.0
        //
        virtual void init (::FT::ObjectGroup_ptr group_ref)
            throw(CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/get_ft_domain_id:1.0
        //
        virtual ::FT::FTDomainId get_ft_domain_id()
            throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/get_object_group_id:1.0
        //
        virtual ::FT::ObjectGroupId get_object_group_id()
            throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException);
    
        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/get_object_group_ref_version:1.0
        //
        virtual ::FT::ObjectGroupRefVersion get_object_group_ref_version()
            throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException);
    
        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/get_client_id:1.0
        //
        virtual char* get_client_id()
            throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException);
    
        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/get_retention_id:1.0
        //
        virtual CORBA::Long get_retention_id()
            throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException);
    
        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/get_expiration_time:1.0
        //
        virtual TimeBase::TimeT get_expiration_time()
            throw(CdmwFT::Test::HelloInterface::NoContext, CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwFT/Test/Hello/set_exception:1.0
        //    
        virtual int send_exception(int exception_number, int how_many)
            throw(CORBA::SystemException);


    virtual void HelloInterface_impl::send_TRANSIENT_exception (::CORBA::Long cycle)
        throw(CORBA::SystemException);
      
    virtual ::CdmwFT::Test::ResultsSeq * HelloInterface_impl::get_results ()
        throw(CORBA::SystemException);

    
    private:

        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
                throw();
        
        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
                throw();

        CORBA::ORB_var m_orb;

        unsigned long m_nb_hello;

        std::string m_location;

        CdmwFT::Location::ActivationHandler_var m_activation_handler;

        int m_counter;

        CORBA::Long m_cycle;

    ::CdmwFT::Test::ResultsSeq_var m_results;
        
    

}; // End class HelloInterface_impl 

}; // End namespace Test

}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP 

