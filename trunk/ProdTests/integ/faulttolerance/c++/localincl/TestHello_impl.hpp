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

#ifndef INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP
#define INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP


#include "faulttolerance/TestHello.skel.hpp"
#include <FaultTolerance/idllib/FT.stub.hpp>



namespace Cdmw
{

namespace Test
{

class HelloInterface_impl : public virtual POA_Cdmw::Test::HelloInterface,
                            public virtual PortableServer::RefCountServantBase
{

public:

    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     *
     *@param
     *@exception
     */
    HelloInterface_impl(
        CORBA::ORB_ptr orb,
        std::string name )
            throw( CORBA::SystemException );

    /**
     * Purpose:
     * <p> [ Destructor description if necessary]
     *
     */
    virtual ~HelloInterface_impl()
            throw();

    /**
     * Purpose:
     * <p>
     *
     *@param
     *@return
     *@exception
     */
    virtual void hello()
            throw( CORBA::SystemException );

    /**
     * Purpose:
     * <p>
     *
     *@param
     *@return
     *@exception
     */
    virtual Cdmw::Test::process_info* who_is_called()
            throw( CORBA::SystemException );

    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    virtual int send_exception(
        int exception_number )
            throw( CORBA::SystemException );


    void set_nb_hello(unsigned long nb_hello);

private:

    // Hide copy constructor/assignment operator
    HelloInterface_impl(
        const HelloInterface_impl& rhs )
            throw();

    HelloInterface_impl& operator =(
        const HelloInterface_impl& rhs )
            throw();

    unsigned long m_nb_hello;
    Cdmw::Test::process_info_var m_process_info;
    CORBA::ORB_var m_orb;
    CORBA::Boolean m_flags;
    int m_counter;
    
    std::string m_client_id;
    long m_retention_id;
    TimeBase::TimeT m_expiration_time;
}; // End class HelloInterface_impl 


}; // End namespace Test

}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP 

