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


#ifndef INCL_PLATFORMMNGT_HOST_PROXY_IMPL_HPP
#define INCL_PLATFORMMNGT_HOST_PROXY_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHostProxy.skel.hpp"
#include "platformsystem/DaemonMonitor.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#endif

namespace Cdmw
{
namespace PlatformMngt
{

class HostProxy_impl;
class System_impl;

/**
 * The callback for the responding host.
 */
class HostRespondingCallback : public virtual Callback
{
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  hostProxy  the host proxy associated to callback.
     */
    HostRespondingCallback (HostProxy_impl* hostProxy);
        

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~HostRespondingCallback ();
        
    /**
     *Purpose:
     *<p> Callback execution. notifies the host is responding
     */
    void execute() throw();
 
    
private:

    /**
     * The host proxy.
     */
    HostProxy_impl* m_hostProxy;

};



/**
 * The callback for the not responding host.
 */
class HostNotRespondingCallback : public virtual Callback
{
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  hostProxy  the host proxy associated to callback.
     */
    HostNotRespondingCallback (HostProxy_impl* hostProxy);
        
   
    /**
     *Purpose:
     *<p> Destructor.
     */
    ~HostNotRespondingCallback ();
    
    
    /**
     *Purpose:
     *<p> Callback execution. notifies the host is not responding
     */
    void execute() throw();
 
    
private:

    /**
     * The host proxy.
     */
    HostProxy_impl* m_hostProxy;

};



/**
 *Purpose:
 *<p> Provides the host monitoring.
 */
class HostProxy_impl : virtual public DeactivableServant_impl,
                       virtual public POA_CdmwPlatformMngt::HostProxy,
                       virtual public PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     * 
     *@param poa       the poa in charge of servants.
     *@param hostDef   the host definition.
     *@param system    the system responsible for the host.     
     *
     *@exception BadParameterException
     *@exception OutOfMemoryException
     *
     */
#ifdef CDMW_USE_FAULTTOLERANCE
    HostProxy_impl (PortableServer::POA_ptr poa,
                    const CdmwPlatformMngt::HostDef& hostDef,
                    System_impl* system,
                    CdmwFT::FaultManagement::FaultDetector_ptr faultDetector)
#else
        HostProxy_impl (PortableServer::POA_ptr poa,
                        const CdmwPlatformMngt::HostDef& hostDef,
                        System_impl* system)      
#endif
          throw (BadParameterException,
                 OutOfMemoryException);
                    



    /**
     *Purpose:
     *<p> Destructor.
     */
    ~HostProxy_impl();
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostProxy/name:1.0
     * operation
     */
    CdmwPlatformMngt::HostName name()
         throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostProxy/status:1.0
     * operation
     */
    CdmwPlatformMngt::HostStatus status()
             throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostProxy/platform_application_exe:1.0
     * operation
     */
    char* platform_application_exe()
             throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostProxy/application_agent_exe:1.0
     * operation
     */
    char* application_agent_exe()
             throw (CORBA::SystemException);
        
    /**
     *Purpose:
     *<p> change host status
     *
     *@param  status  the new status of host.
     */
    void setStatus(CdmwPlatformMngt::HostStatus status);
    
    
    /**
     *Purpose:
     *<p> start the pull monitoring
     *
     */
    void start();
    
    
    /**
     *Purpose:
     *<p> stop the pull monitoring
     *
     */
    void stop();
 
 
 
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_number_of_properties:1.0
     * operation
     */
    CORBA::ULong get_number_of_properties()
         throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_all_property_names:1.0
     * operation
     */
    void get_all_property_names (CORBA::ULong how_many,       
                                 CosPropertyService::PropertyNames_out property_names,
                                 CosPropertyService::PropertyNamesIterator_out rest)
         throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_property_value:1.0
     * operation
     */
    CORBA::Any* get_property_value (const char* property_name)
            throw (CosPropertyService::PropertyNotFound,
                   CosPropertyService::InvalidPropertyName,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_properties:1.0
     * operation
     */
    CORBA::Boolean get_properties (
                          const CosPropertyService::PropertyNames& property_names,
                          CosPropertyService::Properties_out nproperties)
         throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_all_properties:1.0
     * operation
     */
    void get_all_properties (CORBA::ULong how_many,      
                             CosPropertyService::Properties_out nproperties,
                             CosPropertyService::PropertiesIterator_out rest)
         throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWPropertyContainer/set_property:1.0
     * operation
     */
    void set_property (const char* property_name,      
                       const CORBA::Any& property_value)
         throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWPropertyContainer/remove_property:1.0
     * operation
     */
    void remove_property (const char* property_name)
            throw (CosPropertyService::PropertyNotFound,
                   CORBA::SystemException);


private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    HostProxy_impl();

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    HostProxy_impl (HostProxy_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    HostProxy_impl& operator= (const HostProxy_impl& rhs);


    
    /**
     *Purpose:
     *<p> return the host name
     *
     */
    const char* get_hostName();

    
    /**
     * The system responsible for the entity.
     */
    System_impl* m_system;
    
    /**
     * The Daemon Pull Monitor.
     */
    DaemonMonitor* m_monitor;

    /**
     * The Host name
     */
    CdmwPlatformMngt::HostName_var m_hostName;
    
    
    /**
     * The application process exe
     */
    CORBA::String_var m_platform_application_exe;

    /**
     * The full path to the application agent process executable of this host.
     */
    CORBA::String_var m_application_agent_exe;

    
    /**
     * The host status.
     */
    CdmwPlatformMngt::HostStatus m_status;

#ifdef CDMW_USE_FAULTTOLERANCE
    CdmwFT::FaultManagement::FaultDetector_var m_faultDetector;

    CdmwFT::FaultManagement::MonitoringId m_monitoringID;    
#endif
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_HOST_PROXY_IMPL_HPP

