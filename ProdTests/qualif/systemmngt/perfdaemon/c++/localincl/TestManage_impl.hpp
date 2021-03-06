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

#ifndef INCL_CDMW_TEST_MANAGE_IMPL_HPP 
#define INCL_CDMW_TEST_MANAGE_IMPL_HPP 


#include "perfdaemon/TestManage.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <map>
namespace Cdmw
{

namespace Test
{

    class observer_impl :
        public virtual POA_Cdmw::Test::observer,
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
        observer_impl(CORBA::ORB_ptr orb)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~observer_impl()
            throw();
    
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void  kill_daemon ()
            throw(CORBA::SystemException);
                  
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void  kill_daemon_time ( CORBA::Long second, CORBA::Long micro)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void notify( CdmwPlatformMngt::Event* an_event )
                throw( CORBA::SystemException );

        void incrementation();
        
        bool host_is_available();

        CdmwPlatformMngtBase::TimeStamp get_local_time_before()
        {
            return m_localtimebefore[m_inc];
        }

        CdmwPlatformMngtBase::TimeStamp get_local_time_after()
        {
            return m_localtimeafter[m_inc];
        }
        
    private:

        // Hide copy constructor/assignment operator
        observer_impl(const observer_impl& rhs)
            throw();
        
        observer_impl&
        operator=(const observer_impl& rhs)
            throw();
    
        CORBA::ORB_var m_orb;

        int m_inc;
        bool m_host_is_available;
        
        typedef std::map<int, CdmwPlatformMngtBase::TimeStamp> timemap;
        timemap m_localtimebefore;
        timemap m_localtimeafter;

    
    }; // End class observer_impl 
    
    class killer_impl :
        public virtual POA_Cdmw::Test::killer,
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
        killer_impl(CORBA::ORB_ptr orb)
            throw(CORBA::SystemException);
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~killer_impl()
            throw();
        
        virtual void init(observer_ptr obs)
            throw(CORBA::SystemException);
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void start_daemon()
            throw(CORBA::SystemException);
                  
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void kill_daemon()
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void auto_kill()
            throw(CORBA::SystemException);
                  

    private:

        // Hide copy constructor/assignment operator
        killer_impl(const killer_impl& rhs)
            throw();
        
        killer_impl&
        operator=(const killer_impl& rhs)
            throw();

        CORBA::ORB_var m_orb;

        int m_pid_daemon;

        observer_var m_observer;
        
    

    }; // End class killer_impl 




}; // End namespace Test

}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_MANAGE_IMPL_HPP 

