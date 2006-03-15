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

#ifndef INCL_PLATFORMMNGT_MY_PROCESS_CONTROL_HPP
#define INCL_PLATFORMMNGT_MY_PROCESS_CONTROL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
// #include "testcdmwprocess/ClientThread.hpp"
#include "ConfAndPlug/cdmwinit/DefaultProcessControl.hpp"

#include <iostream>

namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class MyProcessControl : public Cdmw::CdmwInit::DefaultProcessControl
{
public:

    MyProcessControl(CORBA::ORB_ptr      orb,
		             std::ostream &      os,
                     unsigned int sleepTime = 0, 
                     bool request = true, 
                     bool except = false);
    
    ~MyProcessControl() throw();
    
    /**
     * Purpose:
     * <p>    This operation informs the pull-based Fault Detector whether the object is able to
     *    accept requests and produce replies.
     *    This operation may be used to perform some user-defined sanity checks ...
     *
     * @return true if the object is alive and ready to take further requests, and false otherwise.
     *
     */
    virtual bool is_alive() throw(CORBA::SystemException);


private:
    CORBA::ORB_var                           m_orb;
    std::ostream &                           m_os;
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

};  // End class MyProcessControl

}; // End namespace PlatformMngt
}; // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_MY_PROCESS_CONTROL_HPP

