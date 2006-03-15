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


// C++ include
#include <iostream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>

#include <ftinit/FTCurrent_impl.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{
    
namespace FT
{

namespace FTInit
{

//
// IDL:thalesgroup.com/CdmwFT/Current:1.0
//
Current_impl::Current_impl(PortableInterceptor::Current_ptr   pi_current,
                           const PortableInterceptor::SlotId  ft_domain_id_slot_id,
                           const PortableInterceptor::SlotId  object_group_id_slot_id,
                           const PortableInterceptor::SlotId  object_group_ref_version_slot_id,
                           const PortableInterceptor::SlotId  client_id_slot_id,
                           const PortableInterceptor::SlotId  retention_id_slot_id,
                           const PortableInterceptor::SlotId  expiration_time_slot_id)
   : m_pi_current(PortableInterceptor::Current::_duplicate(pi_current)),
     m_ft_domain_id_slot_id(ft_domain_id_slot_id),
     m_object_group_id_slot_id(object_group_id_slot_id),
     m_object_group_ref_version_slot_id(object_group_ref_version_slot_id),
     m_client_id_slot_id(client_id_slot_id),
     m_retention_id_slot_id(retention_id_slot_id),
     m_expiration_time_slot_id(expiration_time_slot_id)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

Current_impl::~Current_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

//
// IDL:thalesgroup.com/CdmwFT/Current/get_ft_domain_id:1.0
//
::FT::FTDomainId
Current_impl::get_ft_domain_id()
    throw(CdmwFT::Current::NoContext, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        CORBA::Any_var slot_info = m_pi_current->get_slot(m_ft_domain_id_slot_id);

        CORBA::TypeCode_var tc = slot_info->type();
        if (tc->kind() == CORBA::tk_null)
        {
           // the given slot has not been set, raises NoContext
            CdmwFT::Current::NoContext ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        const char* domain;
        if (slot_info.in() >>= domain)
        {
            CDMW_LOG_FUNCTION_RETURN(domain);
            return CORBA::string_dup(domain);
        }
        else
        {
            // Unexpected value inerted in Any
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: this slot is not a ::FT::FTDomainId!" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }  
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        // This slot that has not been allocated
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: InvalidSlot exception raised!" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
    }
    catch (const CORBA::SystemException& ex )
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Current/get_object_group_id:1.0
//
::FT::ObjectGroupId
Current_impl::get_object_group_id()
    throw(CdmwFT::Current::NoContext, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        CORBA::Any_var slot_info = m_pi_current->get_slot(m_object_group_id_slot_id);

        CORBA::TypeCode_var tc = slot_info->type();
        if (tc->kind() == CORBA::tk_null)
        {
           // the given slot has not been set, raises NoContext
            CdmwFT::Current::NoContext ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        ::FT::ObjectGroupId object_group_id;

        if (slot_info.in() >>= object_group_id)
        {
            CDMW_LOG_FUNCTION_RETURN(object_group_id);
            return object_group_id;
        }
        else
        {
            // Unexpected value inerted in Any
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: this slot is not a ::FT::ObjectGroupId!" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }  
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: InvalidSlot exception raised!" );
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Current/get_object_group_ref_version:1.0
//
::FT::ObjectGroupRefVersion
Current_impl::get_object_group_ref_version()
    throw(CdmwFT::Current::NoContext, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        CORBA::Any_var slot_info 
            = m_pi_current->get_slot(m_object_group_ref_version_slot_id);

        CORBA::TypeCode_var tc = slot_info->type();
        if (tc->kind() == CORBA::tk_null)
        {
            // the given slot has not been set, raises NoContext
            CdmwFT::Current::NoContext ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        ::FT::ObjectGroupRefVersion object_group_ref_version;

        if (slot_info.in() >>= object_group_ref_version)
        {
            CDMW_LOG_FUNCTION_RETURN(object_group_ref_version);
            return object_group_ref_version;
        }
        else
        {
            // Unexpected value inerted in Any
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: this slot is not a ::FT::ObjectGroupRefVersion !" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }  
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: InvalidSlot exception raised!" );
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Current/get_client_id:1.0
//
char*
Current_impl::get_client_id()
    throw(CdmwFT::Current::NoContext, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        CORBA::Any_var slot_info = m_pi_current->get_slot(m_client_id_slot_id);

        CORBA::TypeCode_var tc = slot_info->type();
        if (tc->kind() == CORBA::tk_null)
        {
            // the given slot has not been set, raises NoContext
            CdmwFT::Current::NoContext ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        const char* client_id;

        if (slot_info.in() >>= client_id)
        {
            CDMW_LOG_FUNCTION_RETURN(client_id);
            return CORBA::string_dup(client_id);
        }
        else
        {
            // Unexpected value inerted in Any
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: this slot is not a string!" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }  
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: InvalidSlot exception raised!");
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Current/get_retention_id:1.0
//
CORBA::Long
Current_impl::get_retention_id()
    throw(CdmwFT::Current::NoContext, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        CORBA::Any_var slot_info = m_pi_current->get_slot(m_retention_id_slot_id);

        CORBA::TypeCode_var tc = slot_info->type();
        if (tc->kind() == CORBA::tk_null)
        {
            // the given slot has not been set, raises NoContext
            CdmwFT::Current::NoContext ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        CORBA::Long retention_id;

        if (slot_info.in() >>= retention_id)
        {
            CDMW_LOG_FUNCTION_RETURN(retention_id);
            return retention_id;
        }
        else
        {
            // Unexpected value inerted in Any
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: this slot is not a CORBA::Long!");
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }  
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: InvalidSlot exception raised!" );
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Current/get_expiration_time:1.0
//
TimeBase::TimeT
Current_impl::get_expiration_time()
    throw(CdmwFT::Current::NoContext, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        CORBA::Any_var slot_info = m_pi_current->get_slot(m_expiration_time_slot_id);

        CORBA::TypeCode_var tc = slot_info->type();
        if (tc->kind() == CORBA::tk_null)
        {
           // the given slot has not been set, raises NoContext
            CdmwFT::Current::NoContext ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        TimeBase::TimeT expiration_time;

        if (slot_info.in() >>= expiration_time)
        {
            CDMW_LOG_FUNCTION_RETURN(expiration_time);
            return expiration_time;
        }
        else
        {
            // Unexpected value inerted in Any
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: this slot is not a TimeBase::TimeT!" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }  
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: InvalidSlot exception raised!" );
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

} // End of namespace FTInit

} // End of namespace FT

} // End of namespace Cdmw

