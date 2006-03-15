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


#ifndef INCL_PULLMONITORABLE_IMPL_HPP
#define INCL_PULLMONITORABLE_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/idllib/FTPullMonitorable.skel.hpp"

#include <string>

/**
*Purpose: PullMonitorable implementation
*<p> 
*
*
*/
class PullMonitorable_impl : virtual public POA_FT::PullMonitorable,
                             virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        */ 
        PullMonitorable_impl(std::string& pullName, unsigned int sleepTime, bool request, bool except)
                throw();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~PullMonitorable_impl()
                throw();



        /**
		* Purpose:
		* <p> to call to check if object is still living
		* 
		*@exception CORBA::SystemException
		*
		*/ 
        virtual CORBA::Boolean is_alive ()
                throw(CORBA::SystemException);


        /**
		* Purpose:
		* <p> to set sleep time
		*
		*/ 
        void set_sleepTime (unsigned int sleepTime);


        /**
		* Purpose:
		* <p> to set request value
		*
		*/ 
        void set_request (bool resquest);

        
    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        PullMonitorable_impl (const PullMonitorable_impl& rhs)
                throw();




        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        PullMonitorable_impl&
        operator=(const PullMonitorable_impl& rhs)
                throw();
     
    

        /**
        * Name of the pullMonitorable
        */  
        std::string m_pullName; 


    
        /**
        * time to sleep in msec
        */  
        unsigned int m_sleepTime; 
        

        /**
        * PullMonitorable request
        */  
        bool m_request; 


        /**
        * PullMonitorable exception to raise
        */  
        bool m_except; 


        /**
        * PullMonitorable counter
        */  
        int m_counter;

}; // End class PullMonitorable_impl



#endif // INCL_PULLMONITORABLE_IMPL_HPP

