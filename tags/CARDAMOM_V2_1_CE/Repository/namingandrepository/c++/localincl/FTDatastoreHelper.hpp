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


#ifndef INCL_NAMINGANDREPOSITORY_FTDATASTORE_HELPER_HPP
#define INCL_NAMINGANDREPOSITORY_FTDATASTORE_HELPER_HPP

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <iostream>
#include <functional>

#include <Foundation/common/Assert.hpp> 
#include <Foundation/commonsvcs/datastore/DataStoreInterface.hpp>
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>

#include "namingandrepository/Exceptions.hpp"
#include "namingandrepository/StateTransferConfig.hpp"
#include "namingandrepository/GlobalDataStores.hpp"
#include "namingandrepository/CdmwNamingAndRepositoryDatastoreEntities.skel.hpp"

namespace Cdmw
{
namespace NamingAndRepository
{
    typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface
    < std::string,
     CdmwNamingAndRepository::StorageData >
    FTDataStore;
    
    typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory
    < std::string, 
      CdmwNamingAndRepository::StorageData > 
    DefaultDataStoreFactory;

    class FTDataStoreHelper
    {
    public:
	static FTDataStore* getStorage(Cdmw::CommonSvcs::DataStore::DsIdentifier dsId);
    };


// FIXME - rename to compoundStorageKey()
inline std::string
createStorageKey (const std::string & prefix, const std::string & id) 
    throw (Cdmw::OutOfResourcesException)
{
    try {
	return prefix + "::" + id;
    } 
    catch (const std::bad_alloc &) {
	CDMW_THROW(Cdmw::OutOfResourcesException);
    } 
}
  
    // FIXME - inefficient - copies all elements
typedef std::pair<std::string,  
		  CdmwNamingAndRepository::StorageData_var> StorageKeyDataPair;

typedef std::list<StorageKeyDataPair> StorageKeyDataPairList;

    class GatherSiblings :   // FIXME - put into anonymous namespace
    public std::unary_function<std::pair< 
                                   const std::string ,  
                                   CdmwNamingAndRepository::StorageData_var> , 
                               void>
{
public:
    
    GatherSiblings (const std::string &contextPrefix, 
		    StorageKeyDataPairList &siblings)
	: m_prefix(contextPrefix), m_siblings(siblings)
    { 
    } 
    
    void operator ()(const std::pair<
		             const std::string, 
		             CdmwNamingAndRepository::StorageData_var > & it)
    {
	const std::string  prefix = it.first.substr(0, m_prefix.length());
	// OPTME - do not create substr for prefix-comparison
	if (m_prefix == prefix) {
	    const StorageKeyDataPair sibling(it);
	    m_siblings.push_back(sibling); 
	}
    }

    const std::string      &m_prefix; 
    StorageKeyDataPairList &m_siblings;
};


// FIXME remove this operator, or make consistent to IDL 
inline ostream&
operator << (ostream& os, 
	     const CdmwNamingAndRepository::StorageData &data) 
{
    switch (data._d()) {
    case CdmwNamingAndRepository::NAR_DOMAIN:
	{ 
	    return os 
		<< "domain(" 
		<< data.domain_info().the_name.in()		
		<< ")";
	}
    case CdmwNamingAndRepository::NAR_REGISTRATION:
	{
	    return os 
		<< "registration(" 
		<< data.registration_info().the_name.in()
		<< ","
		<< "expiration time/TODO)";
	    break;
	}
    case CdmwNamingAndRepository::NAR_FACTORY_REGISTRATION:
	{
	    std::string regid = data.factory_registration_info().the_registration_id.in();
	    return os 
		<< "factory_registration(" 
		<< data.factory_registration_info().the_name.in()
		<< ",regid=" << regid 
		<< ")";
	    break;
	}
    case CdmwNamingAndRepository::NAR_OBJECT_REGISTRATION:
	{
	    std::string regid = data.object_registration_info().the_registration_id.in();
	    return os 
		<< "object_registration(" 
		<< data.object_registration_info().the_name.in()
		<< ",regid=" << regid 

		<< ")";
	    break;
	}
    case CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION:
	{
	    std::string regid = data.domain_registration_info().the_registration_id.in();
	    return os 
		<< "domain_registration(" 
		<< data.domain_registration_info().the_name.in()
		<< ", user_supplied="
		<< data.domain_registration_info().is_user_supplied
		<< ", domain="
		<< data.domain_registration_info().the_domain.in()
		<< ", factory_finder="
		<< data.domain_registration_info().the_factory_finder.in()
		<< ",regid=" << regid 
		<< ")";
	    break;
	}
    case CdmwNamingAndRepository::NAR_CONTEXT:
	{
	    return os 
		<< "context_local(" 
		<< data.context_binding_info().the_name.in()
		<< ")";
	    break;
	}
    case CdmwNamingAndRepository::NAR_OBJECT_BINDING:
	{
	    return os 
		<< "object(" 
		<< data.object_binding_info().the_name.in()
		<< ","
		<< data.object_binding_info().the_object.in()
		<< ")";
	    break;
	}
    case CdmwNamingAndRepository::NAR_CONTEXT_BINDING:
	{
	    return os 
		<< "context_local(" 
		<< data.context_binding_info().the_name.in()
		<< ","
		<< data.context_binding_info().the_object.in()
		<< ")";
	    break;
	}
    default:
	CDMW_NEVER_HERE();
    }
}


class PrintForeach :  
    public std::unary_function<std::pair< 
                                   const std::string ,  
                                   CdmwNamingAndRepository::StorageData_var> , 
                               void>
{
public:
    PrintForeach(ostream& os)
	: m_os(os)
    {
    }

    void operator ()(const std::pair<
		     const std::string, 
		     CdmwNamingAndRepository::StorageData_var > & it) const 
    {	
	std::ostringstream temp;
	temp << "Datastore[\"" << it.first << "\"]=" << it.second.in()
	     << std::endl;
	m_os << temp.str();
    }

    ostream& m_os;
};

inline ostream&
operator << (ostream& os, 
	     FTDataStore *storage)
{
    os << "--------- Datastore begin ---------" << std::endl;
    storage->for_each (PrintForeach(os));
    os << "--------- Datastore end ---------" << std::endl;

    return os;
} 


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

