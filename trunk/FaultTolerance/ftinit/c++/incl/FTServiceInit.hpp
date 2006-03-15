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


#ifndef INCL_CDMW_FT_FTINIT_HPP 
#define INCL_CDMW_FT_FTINIT_HPP 

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    
/**
* Namespace for CDMW Fault Tolerance types and data.
*/

namespace FT
{

//namespace RequestLogging
//{
    class ReplyRecording;
//}

/**
*Purpose:
*<p>    This class provides an init of the Fault Tolerance Framework
* capability.
*
*Features:
*<p> Thread safe
*
*/
class FTServiceInit 
{
public:

    
    /**
     * Purpose:
     * <p> Initialises ORB FT mechanism and creates a group repository and server
     * request interceptor able to manage FT failover mechanism.
     * 
     *@param argc The number of parameter received of the command line
     *@param argv The list of parameter received of the command line
     *@param client_only Set to true to limit initialisation to client side init. 
     * No group repository neither interceptor is created in that case.
     *
     *@return none
     *
     *@exception CORBA::INTERNAL  The CDMW <B>INTERNAL</B> minor code indicates an
     * internal error occured within Cdmw FT library.
     *@exception CORBA::SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */ 
    static 
    void
    Init (int& argc, char** argv, bool client_only=false)
        throw(CORBA::SystemException);


    static void Set_reply_recorder(Cdmw::FT::ReplyRecording* recorder);

protected:    

private:

    FTServiceInit() {}
    ~FTServiceInit() {}

    static Cdmw::FT::ReplyRecording* M_recorder;

}; // End class FTServiceInit 

} // End namespace FT
} // End namespace Cdmw



#endif // INCL_CDMW_FT_FTINIT_HPP

