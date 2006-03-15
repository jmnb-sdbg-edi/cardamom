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


#ifndef INCL_PLATFORMMNGT_ENTITY_IMPL_HPP
#define INCL_PLATFORMMNGT_ENTITY_IMPL_HPP


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformelementlibrary/ManagedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/CommandedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.skel.hpp"



namespace Cdmw
{
namespace PlatformMngt
{


/**
 *Purpose:
 *<p> Provides the Entity.
 */
class Entity_impl : virtual public POA_CdmwPlatformMngt::Entity,
                    public DeactivableServant_impl,
                    public ManagedElement_impl,
                    public CommandContainer_impl
                    
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     * 
     *@param poa                the poa in charge of servants.
     *@param entity_def         the Entity definition.
     *@param entity_path        the path of the Entity.
     *@param element_repository the element repository     
     *
     *@exception BadParameterException
     *@exception OutOfMemoryException
     *
     */
     Entity_impl (PortableServer::POA_ptr poa,
                  const CdmwPlatformMngt::EntityDef& entity_def,
                  const char* entity_path,
                  CdmwPlatformMngt::RWElementRepository_ptr element_repository)      
          throw (BadParameterException,
                 OutOfMemoryException,CosPropertyService::MultipleExceptions);
                    

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~Entity_impl();

    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute(const char * attr_name, CORBA::Any_out ret_attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute(const char * attr_name, const CORBA::Any & attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

                     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status:1.0
     * operation
     */
    char* get_status(CORBA::String_out entity_info)
             throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status:1.0
     * operation
     */
    void set_status(const char* entity_status, const char* entity_info)
             throw (CdmwPlatformMngt::EntityStatusNotAllowed, 
                    CORBA::SystemException);
        

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status_values:1.0
     * operation
     */
    CdmwPlatformMngt::EntityStatusValues* get_status_values()
             throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_snapshot:1.0
     * operation
     */
     CdmwPlatformMngt::EntitySnapshot* get_snapshot()
             throw (CORBA::SystemException);

private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    Entity_impl();

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    Entity_impl (Entity_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    Entity_impl& operator= (const Entity_impl& rhs);


    /**
     *Purpose:
     *<p> Create the entity record in data store
     *
     *@param entityDef the data for entity to start.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    void create_record_in_datastore(
                  const CdmwPlatformMngt::EntityDef& entityDef)
            throw (OutOfMemoryException);
            
    /**
     *Purpose:
     *<p> Remove the entity record in data store
     *
     */
    void remove_record_in_datastore();
    
    /**
     *Purpose:
     *<p> Get the entity record from data store
     *
     */
    CdmwPlatformMngt::EntityRecord* get_record_from_datastore();
    
    /**
     *Purpose:
     *<p> Update the entity record in data store
     *
     */
    void update_record_in_datastore(const CdmwPlatformMngt::EntityRecord& entity_record);
    
    /**
     * The mutex used for changing the host record.
     */
    OsSupport::Mutex m_mutex_entity_record;
    
    /**
     * The entity datastore.
     */
    EntityDataStore* m_entity_ds;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_ENTITY_IMPL_HPP

