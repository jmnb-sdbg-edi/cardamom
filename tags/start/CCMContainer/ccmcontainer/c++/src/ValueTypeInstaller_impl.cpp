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


#include "ccmcontainer/ValueTypeInstaller_impl.hpp"
#include "ccmcontainer/ValueTypeRegistration_impl.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"

#include "ccmcontainer/MacroDefs.hpp"

namespace Cdmw
{

namespace CCM
{
namespace Container
{


ValueTypeInstaller_impl::ValueTypeInstaller_impl(CORBA::ORB_ptr orb)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
{
    DEBUG_ECHO << "ValueTypeInstaller_impl::ValueTypeInstaller_impl()" << std::endl;
}
    
        
ValueTypeInstaller_impl::~ValueTypeInstaller_impl()
    throw ()
{
    DEBUG_ECHO << "ValueTypeInstaller_impl::~ValueTypeInstaller_impl()" << std::endl;
}
        
CdmwCcmContainer::ValueTypeRegistration_ptr
ValueTypeInstaller_impl::install_valuetype_factory(const char* repid,
                                                   const char* location,
                                                   const char* entrypoint)
    throw (CORBA::SystemException)
{
    
    CDMW_MUTEX_GUARD(m_mutex);

    DEBUG_ECHO << "ValueTypeInstaller_impl::install_valuetype_factory("
               << "\n\t repid = " << repid << ","
               << "\n\t location = " << location << ","
               << "\n\t entrypoint = " << entrypoint << ")" << std::endl;
        
    RegistrationInfo info(0,0,false);
    
    CdmwCcmContainer::ValueTypeRegistration_var reg;

    RegistrationMap::iterator i = m_registrations.find(std::string(repid));
    if (i != m_registrations.end()) {
        // Already registered through the installer.
        // Return existing registration object.
        info = (*i).second;
        reg = CdmwCcmContainer::ValueTypeRegistration::_duplicate(info.m_registration);

        DEBUG_ECHO << "==> [" << repid << "] already registered. Returning existing handle."
                   << std::endl;
    } else {
        // Register the valuetype factory within the ORB.
    #if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
        CORBA_ValueFactoryBase_var old_valuetype_fac =
    #else
        CORBA::ValueFactoryBase_var old_valuetype_fac =
    #endif
            m_orb->lookup_value_factory(repid);
        if (old_valuetype_fac.in() == 0) {

            DEBUG_ECHO << "==> Loading valuetype factory for [" << repid << "] ..."
                       << std::endl;
        
            // Was not already registered outside of the installer
            // Leave previous valuetype factory and create a registration object with no
            // unregistration if registrerd outside of the installer.
            const char* shlib_pathname = location;
            DEBUG_ECHO << "OS::dlopen(" << shlib_pathname << ")..." << std::endl;
            if (strlen(shlib_pathname) == 0)
                shlib_pathname = NULL; // Access global symbol object

            using namespace Cdmw::OsSupport;
            using namespace Components::Deployment;
            
        OS::SharedObjectHandle *shlib = OS::dlopen(shlib_pathname, RTLD_NOW | RTLD_GLOBAL);
            if (shlib->is_null()) {
                DEBUG_ECHO << "Cannot open '" << shlib_pathname << "'\nReason:"
                           << OS::dlerror() << std::endl;
                throw InstallationFailure(CdmwDeployment::CANNOT_OPEN_SHARED_LIB);
            }
            DEBUG_ECHO << "shlib = " << shlib << std::endl;
            
            CORBA::ValueFactoryBase* (*value_fn_ptr) ();
            
            DEBUG_ECHO << "OS::dlsym(" << shlib << "," << entrypoint << ")..." << std::endl;
            value_fn_ptr = (CORBA::ValueFactoryBase* (*) ()) OS::dlsym(shlib,entrypoint);
            
            if (value_fn_ptr == NULL) {
                DEBUG_ECHO << "Entry point '" << entrypoint << "' not found.\nReason:"
                           << OS::dlerror() << std::endl;
                throw ImplEntryPointNotFound();
            }
            DEBUG_ECHO << "value_fn_ptr = " << value_fn_ptr << std::endl;
            
            // Get Valuetype Factory
        #if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
            CORBA_ValueFactoryBase_var valuetype_fac = (*value_fn_ptr)();
        #else
            CORBA::ValueFactoryBase_var valuetype_fac = (*value_fn_ptr)();
        #endif

            if (valuetype_fac.in() == 0)
                throw InstallationFailure(CdmwDeployment::UNSUPPORTED_COMPONENT_TYPE);
 
        #if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
            CORBA_ValueFactoryBase_var tmp_fac =
        #else
            CORBA::ValueFactoryBase_var tmp_fac =
        #endif           
                m_orb->register_value_factory(repid, valuetype_fac.in());
            info.m_shlib = shlib;
            info.m_release = true;
        } else {
            DEBUG_ECHO << "==> [" << repid << "] already registered outside of the installer."
                       << std::endl;
        }
        info.m_registration = new ValueTypeRegistration_impl(this,repid);
        reg = info.m_registration;
        
        m_registrations.insert(RegistrationMap::value_type(std::string(repid),info));
    }
    return reg._retn();
}


bool
ValueTypeInstaller_impl::unregister_valuetype_factory(const std::string & repid)
{
    CDMW_MUTEX_GUARD(m_mutex);

    bool found = false;
    RegistrationMap::iterator i = m_registrations.find(repid);
    if (i != m_registrations.end()) {
        RegistrationInfo info = (*i).second;
        if (info.m_release) {
            DEBUG_ECHO << "==> Unregistering valuetype factory for [" << repid << "] ..."
                       << std::endl;
            m_orb->unregister_value_factory(repid.c_str());
            Cdmw::OsSupport::OS::dlclose(info.m_shlib);
        }
        m_registrations.erase(i);
        found = true;
    }
    return found;
}


}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

