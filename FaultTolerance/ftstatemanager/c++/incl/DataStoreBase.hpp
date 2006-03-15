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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_DATASTOREBASE_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_DATASTOREBASE_HPP

#include <string>
#include <set>
#include <map>
#include <vector>

#include "Foundation/commonsvcs/datastore/DataStoreRegistry.hpp"
#include <Foundation/osthreads/ReaderWriterLock.hpp>
#include <FaultTolerance/idllib/CdmwFTCommon.stub.hpp>
#include "FaultTolerance/idllib/CdmwFTTPCCohort.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTTPCCoordinator.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTStateTransfer.stub.hpp"

#include "DataStoreTimeoutHandler.hpp"

namespace Cdmw {
namespace FT {

typedef Cdmw::CommonSvcs::DataStore::AlreadyExistException AlreadyExistException;
typedef Cdmw::CommonSvcs::DataStore::NotFoundException NotFoundException;
typedef Cdmw::CommonSvcs::DataStore::DsIdentifier DsIdentifier;
typedef Cdmw::CommonSvcs::DataStore::DataStoreRegistry::DataStoreDictionary DataStoreDictionnary;

class DataStoreContext;
class DataStoreRecordIteratorBase;

/**
* Base class of a DataStore
* This class also offers some static operations to manage instanciated DataStores.
*
*
*Features:
*<p> Thread safe. Exception safe.
*/
class DataStoreBase: public Cdmw::CommonSvcs::DataStore::DataStoreBase
{
    typedef std::map<DsIdentifier, DataStoreBase*> DataStoreMap;

public:
    
    typedef std::multimap<std::string, DsIdentifier> DsidMap;
    typedef std::vector<DataStoreTimeoutHandler*> TimeoutHandlerList;

    virtual DataStoreContext& get_context() = 0;

    /**
     * Initialized the datastore in cold/hot restart.
     */
    virtual void init(bool cold_restart = true) throw (Cdmw::InitErrorException) = 0;

   /**
    * Return the DataStore with the specified identifier.
    *
    *@return the requested DataStore
    */
    static DataStoreBase* Get_dataStore(const DsIdentifier& dsid);

    /**
     * Registers an handler to be notified whenever there is a timeout expiration (freeze
     * timeout).
     *
     * @param timeout_handler a const pointer to a DataStoreTimeoutHandler object
     * @return true if the Data Store timeout handler is registered, false if failed to 
     * register it
     * (it has already been registered, or exceeded maximum allowed number of handlers ...)
     */
    static bool Register_Timeout_Handler(DataStoreTimeoutHandler* timeout_handler) 
        throw();

    /**
     * Un-registers an already registered timeout handler. The handler will no longer be 
     * notified by datastores.
     *
     * @param timeout_handler a const pointer to a DataStoreTimeoutHandler object
     * @return true if the Data Store tiemout handler is unregistered, false if the 
     * timeout handler was not previously registered
     */
    static bool Unregister_Timeout_Handler(DataStoreTimeoutHandler* timeout_handler)
        throw();

    /**
     * Invokes any registered timeout handlers.
     */
    static void Timeout(const std::string& location);

    /**
    * The data store becomes the one of the master
    */
     virtual void activate() = 0;
    
    /**
    * The data store becomes the one of the slave
    */
    virtual void deactivate() = 0;
    
    /**
    * process failure of a location
    *
    *@param location is the failed location
    */
    virtual void on_failure(const std::string& location) = 0;

    /**
     * Returns an iterator that provides state information necessary to copy
     * a DataStore.
     *
     * @param from_location is the name of the location asking the records.
     * @param coordinator the transaction cordinator.
     * @param cohort the cohort.
     *
     * @exception InvalidDataStoreState is raised if the data store is not the 
     * coordinator
     */
    virtual DataStoreRecordIteratorBase* get_record_iterator
    (const char * from_location)
        throw (CORBA::SystemException,
               CdmwFT::StateTransfer::InvalidDataStoreState) = 0;

    /**
     * Load DataStore with state from another DataStore.
     *
     * @param recs sequence of state changes (inserts, updates and removes).
     */
    virtual void load_state
    (const CdmwFT::Transaction::StateChangeRecords& recs) = 0;

    /**
     * Validate a set of changes.
     *
     * @param recs sequence of state changes (inserts, updates and removes).
     */
    virtual bool validate
    (const CdmwFT::Transaction::StateChangeRecords& recs) = 0;

    /**
     * Compare the oids for a set of changes.
     *
     * @result true if any oids match, false otherwise
     */
    virtual bool oids_match
    (const CdmwFT::Transaction::StateChangeRecords& recs1,
     const CdmwFT::Transaction::StateChangeRecords& recs2) = 0;

    /**
     * Requests the DataStore is frozen with respect to insert, update
     * and remove. Multiple locations may requests freezes concurrently.
     * When frozen insert, update and remove operations will block.
     *
     * @param location the location of the iterator requesting the freeze.
     */
    virtual void freeze(const std::string& location) = 0;

    /**
     * Indicates a iterator is finished reading from the DataStore and
     * the freeze can be released. Unfreezing will actually occur when
     * the last iterator requesting a freeze is released,
     *
     * @param location the location of the iterator requesting the unfreeze.
     */
    virtual void unfreeze(const std::string& location) = 0;

    /**
    * call activate of all DataStore instances of the current location
    */
    static void Activate_All();
    
    /**
    * call deactivate of all DataStore instances of the current location
    */
    static void Deactivate_All();
    
    /**
    * call on_failure of all DataStore instances of the current location
    */
    static void On_Failure(const std::string& location) throw (NotFoundException);

    /**
    * get the FT location of this process
    */
    static const char* const Get_Location_Name()
    {
      return M_location_name.c_str();
    }
        
    /**
     * set the FT location of this process
     */
    static void Set_Location_Name(const std::string& location)
    {
        M_location_name = location;
    }
    
protected:

    /**
    * Purpose:
    * <p> Constructor
    *
    *@param string dsid Data Store Identifier
    */
    DataStoreBase(DsIdentifier dsid)
        : Cdmw::CommonSvcs::DataStore::DataStoreBase(dsid)
    {
    }

    /**
    * Purpose:
    * <p> Destructor.
    */
    virtual ~DataStoreBase()
    {
    }

    static DataStoreMap M_ft_dsMap;

private:

    /**
    * Purpose:
    * <p> Default constructor.
    */
    DataStoreBase();

    /**
    * Assignement operator is not allowed.
    */
    DataStoreBase& operator=(const DataStoreBase& rhs);

private:
    static std::string M_location_name;
    
    static DsidMap M_dsidMap;
    
    static TimeoutHandlerList M_timeoutHandlerList;

  }; // End class DataStoreBase

}  // End namespace FT
}  // End namespace Cdmw

#endif

