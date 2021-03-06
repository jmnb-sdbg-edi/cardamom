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
 
#include "platformftsystem/SystemObsvDataStoreObserver.hpp"
#include "platformlibrary/Configuration.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// SystemObsvDataStoreObserver class.
// ----------------------------------------------------------------------

SystemObsvDataStoreObserver::SystemObsvDataStoreObserver (System_impl* p_system)
  throw()
    : m_system (p_system)
{     
}


SystemObsvDataStoreObserver::~SystemObsvDataStoreObserver()
  throw()
{
}


void SystemObsvDataStoreObserver::on_insert(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
           std::cout << "<<SystemObsvDataStoreObserver::on_insert (" << oid <<")>>" << std::endl;
           
           if (!strcmp (data.host_name, ""))
           {
               m_system->register_observer (oid.c_str(),
                                            data.observer);
           }
           else
           {
               m_system->register_proxy_observer (oid.c_str(),
                                                  data.host_name,
                                                  data.observer);
           }
        }
        catch (...)
        {
            // nothing to do
        }    
    }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore

}

void SystemObsvDataStoreObserver::on_update(_Oid_in_type oid, _Data_in_type data)
  throw ()
{

}

void SystemObsvDataStoreObserver::on_remove(_Oid_in_type oid) 
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        m_system->unregister_observer (oid.c_str());
    }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore

}

     

} // End namespace PlatformMngt
} // End namespace Cdmw

