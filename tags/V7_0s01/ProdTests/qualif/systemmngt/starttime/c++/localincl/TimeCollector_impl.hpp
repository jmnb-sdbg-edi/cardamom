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


#ifndef INCL_CDMW_QUALIF_TIME_COLLECTOR_HPP 
#define INCL_CDMW_QUALIF_TIME_COLLECTOR_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "starttime/TimeCollector.skel.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>


namespace Cdmw {
    

    
    
    class TimeCollector_impl : public virtual POA_Cdmw::Test::collector,
                                                    public virtual PortableServer::RefCountServantBase
{    

public:
    
    TimeCollector_impl(CORBA::ORB_ptr orb, int nb_processes);
    ~TimeCollector_impl();
    
    
    virtual void set_time_creation (::CORBA::Long second,
                                    ::CORBA::Long micro)
        throw (CORBA::SystemException);
    
    virtual CORBA::Boolean all_process_are_started ()
        throw (CORBA::SystemException);
    
    virtual ::Cdmw::Test::time get_max_time ()
        throw(CORBA::SystemException);
    
    
protected:
    CORBA::ORB_var m_orb;
    int m_nb_processes;
    Cdmw::OsSupport::OS::Timeval m_max_time;    
    int m_current_nb_processes;
    
};
    
};


#endif //INCL_CDMW_QUALIF_TIME_COLLECTOR_HPP 
