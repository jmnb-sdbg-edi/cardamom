/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <CCMContainer/ccmcomponentserver/StandardHomeRegistration.hpp>
#include <Foundation/common/Assert.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"

namespace Cdmw
{
namespace CCM
{
namespace ComponentServer
{
    
StandardHomeRegistration::StandardHomeRegistration()
    throw (CORBA::SystemException)
{
    // Check that RepositoryInterface has been initialised
    CdmwNamingAndRepository::Repository_var rep =
        NamingAndRepository::RepositoryInterface::get_repository();
}

    
StandardHomeRegistration::~StandardHomeRegistration()
    throw ()
{
}


bool
StandardHomeRegistration::bind(Components::CCMHome_ptr home_ref,
                              const char* home_name)
{
    bool result = false;
    using namespace Cdmw::NamingAndRepository;  
    CdmwNamingAndRepository::NameDomain_var dom = CdmwNamingAndRepository::NameDomain::_nil();
    CdmwLifeCycle::FactoryBase_var factory = CdmwLifeCycle::FactoryBase::_nil();
    std::string basename;
    try {
        basename = NamingInterface::basename(home_name);
        std::string dirname = NamingInterface::dirname(home_name);

        // Get the NameDomain
        dom = RepositoryInterface::get_domain(dirname);
        factory = CdmwLifeCycle::FactoryBase::_narrow(home_ref);
        
        if (!CORBA::is_nil(factory.in())) {
            dom->register_new_factory(basename.c_str(),factory.in());
            result = true;
        }
    } catch (const InvalidNameException & ) {
        // ignore...
    } catch (const CdmwNamingAndRepository::NoNameDomain &) {
        // ignore...
    } catch (const CdmwNamingAndRepository::InvalidName & ) {
        // ignore...
    } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists & ) {
        try {
            // Re-register it again (force registration!).
            dom->release_name(basename.c_str());
            dom->register_new_factory(basename.c_str(),factory.in());
            result = true;
        } catch (...) {
            // ignore...
        }
    }
    return result;
}

bool
StandardHomeRegistration::unbind(const char* home_name)
{
    bool result = false;
    using namespace Cdmw::NamingAndRepository;    
    try {
        std::string basename = NamingInterface::basename(home_name);
        std::string dirname = NamingInterface::dirname(home_name);

        // Get the NameDomain
        CdmwNamingAndRepository::NameDomain_var dom = RepositoryInterface::get_domain(dirname);
        dom->release_name(basename.c_str());
        result = true;
    } catch (const InvalidNameException & ) {
        // ignore...
    } catch (const CdmwNamingAndRepository::NoNameDomain &) {
        // ignore...
    } catch (const CdmwNamingAndRepository::InvalidName & ) {
        // ignore...
    } 

    return result;
}

}; // End namespace ComponentServer
}; // End namespace CCM
}; // End namespace Cdmw

