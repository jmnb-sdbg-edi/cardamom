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


#include "faulttoleranceserver/TestHello.skel.hpp"
#include <FaultTolerance/idllib/FT.stub.hpp>


namespace Cdmw
{

namespace Test
{

class HelloInterface1_impl :
public virtual POA_Cdmw::Test::HelloInterface1,
        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        HelloInterface1_impl(CORBA::ORB_ptr orb, const std::string& name, const std::string& hostname_file_loc)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~HelloInterface1_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual void hello ()
            throw(CORBA::SystemException);
                  
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual Cdmw::Test::process_info* who_is_called()
            throw(CORBA::SystemException);


        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual int send_exception(int exception_number)
            throw(CORBA::SystemException);


        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual ::FT::FTRequestServiceContext* request_identification()
        throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual int group_version_identification()
        throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual FT::ObjectGroup* get_objectgroup(::FT::ObjectGroupId id)
        throw(CORBA::SystemException);

    private:

        // Hide copy constructor/assignment operator
        HelloInterface1_impl(const HelloInterface1_impl& rhs)
                throw();
        
        HelloInterface1_impl&
        operator=(const HelloInterface1_impl& rhs)
            throw();
    
    CORBA::ORB_var m_orb;
    unsigned long m_nb_hello;

    Cdmw::Test::process_info_var m_process_info;
    CORBA::Boolean m_flags;
    int m_counter;
    
    std::string m_client_id;
    long m_retention_id;
    TimeBase::TimeT m_expiration_time;
    
    std::string m_hostname_file_location;
    
}; // End class HelloInterface1_impl 

class HelloInterface2_impl :
public virtual POA_Cdmw::Test::HelloInterface2,
        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        HelloInterface2_impl(CORBA::ORB_ptr orb, std::string name)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~HelloInterface2_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual void hello ()
            throw(CORBA::SystemException);
                  
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual Cdmw::Test::process_info* who_is_called()
            throw(CORBA::SystemException);

    private:

        // Hide copy constructor/assignment operator
        HelloInterface2_impl(const HelloInterface2_impl& rhs)
                throw();
        
        HelloInterface2_impl&
        operator=(const HelloInterface2_impl& rhs)
            throw();
    
    unsigned long m_nb_hello;
    Cdmw::Test::process_info_var m_process_info;
    CORBA::ORB_var m_orb;
    

}; // End class HelloInterface2_impl 

class HelloInterface3_impl :
public virtual POA_Cdmw::Test::HelloInterface3,
        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        HelloInterface3_impl(CORBA::ORB_ptr orb, std::string name)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~HelloInterface3_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual void hello ()
            throw(CORBA::SystemException);
                  
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
    virtual Cdmw::Test::process_info* who_is_called()
            throw(CORBA::SystemException);

    private:

        // Hide copy constructor/assignment operator
        HelloInterface3_impl(const HelloInterface3_impl& rhs)
                throw();
        
        HelloInterface3_impl&
        operator=(const HelloInterface3_impl& rhs)
            throw();
    
    unsigned long m_nb_hello;
    Cdmw::Test::process_info_var m_process_info;
    CORBA::ORB_var m_orb;
    

}; // End class HelloInterface3_impl 


}; // End namespace Test

}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP 

