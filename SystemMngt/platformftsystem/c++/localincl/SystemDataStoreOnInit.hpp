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


#ifndef INCL_PLATFORMMNGT_SYSTEM_DATASTORE_ONINIT_HPP
#define INCL_PLATFORMMNGT_SYSTEM_DATASTORE_ONINIT_HPP

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include "FaultTolerance/ftstatemanager/DataStoreInitialiser.hpp"

#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"
#include "platformftaccessorlibrary/SystemAccessor_impl.hpp"



namespace Cdmw
{
namespace PlatformMngt
{  
    
    
typedef Cdmw::FT::DataStore<std::string,
                            CdmwPlatformMngt::SystemRecord> SystemFTDataStore;
                                  

/**
 *Purpose:
 *<p>
 */
class SystemDataStoreOnInit : virtual public Cdmw::FT::DataStoreInitialiser
                                             <std::string,
                                              CdmwPlatformMngt::SystemRecord>

{

public:
    
    typedef SystemFTDataStore::DataInsertIterator DataInsertIterator;

    virtual void on_init(DataInsertIterator it, bool cold_restart = true) 
            throw (Cdmw::InitErrorException);

    /**
     *Purpose:
     *<p> Constructor.
     *     
     */

    SystemDataStoreOnInit (SystemAccessor_impl* p_system_accessor)    
        throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~SystemDataStoreOnInit() throw ();



protected:

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SystemDataStoreOnInit (SystemDataStoreOnInit& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SystemDataStoreOnInit& operator = (const SystemDataStoreOnInit& rhs);
    
    /**
     * The system object.
     */
    SystemAccessor_impl* m_system_accessor;
                          

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SYSTEM_DATASTORE_ONINIT_HPP

