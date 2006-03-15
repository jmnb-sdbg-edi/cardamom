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


#ifndef __CDMW_FT_CDMW_TYPE_REPOSITORY_IMPL_HPP__
#define __CDMW_FT_CDMW_TYPE_REPOSITORY_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "FaultTolerance/idllib/CdmwFTTypeRepository.skel.hpp"
#include <list>
#include <map>


namespace Cdmw {
namespace FT {


typedef std::list< std::string > InterfacesList;
typedef std::map< std::string, InterfacesList > SupportedInterfacesMap;
    
    
class TypeRepository_impl : virtual public CdmwFT::TypeRepository, 
                            virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:

    TypeRepository_impl(SupportedInterfacesMap supportedInterfaces);
    
    
    CORBA::Boolean has_base_interface(const char* type_id, const char* base_type_id)
        throw(::CdmwFT::TypeRepository::TypeNotFound);
    
    
private:

    /**
     * supported interfaces map
     */
    SupportedInterfacesMap m_supportedInterfacesMap;

};
    
}; // namespace FT
}; // namespace Cdmw

#endif // __CDMW_FT_CDMW_TYPE_REPOSITORY_IMPL_HPP__
