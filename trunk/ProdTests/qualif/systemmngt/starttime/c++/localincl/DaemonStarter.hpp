/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#ifndef INCL_CDMW_QUALIF_DAEMON_STARTER_HPP 
#define INCL_CDMW_QUALIF_DAEMON_STARTER_HPP


#include "starttime/ProcessStarter.hpp"

#include <Foundation/orbsupport/CORBA.hpp>



namespace Cdmw {
    

class DaemonStarter : public ProcessStarter
{
    public:
        
        DaemonStarter(const std::string& args,
                      CORBA::ORB_ptr orb);
    
        bool check_if_started() 
            throw (ProcessError);
        
        
    private:
        CORBA::ORB_var m_orb;

        static const std::string DAEMON_EXE;
        static const std::string DAEMON_CORBALOC;
    
        
};

};

#endif
