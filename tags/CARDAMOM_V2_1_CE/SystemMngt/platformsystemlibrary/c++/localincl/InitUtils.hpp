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


#ifndef INCL_CDMW_PLATFORMMNGT_INIT_UTILS_HPP 
#define INCL_CDMW_PLATFORMMNGT_INIT_UTILS_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Exception.hpp"

#include "platformsystemlibrary/CdmwInitConfiguration.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

/**
 *Purpose: Static class to get initialisation data
 *<p>
 */
class InitUtils
{
   public:

    /**
     * Purpose:
     * <p> Returns the reference of the RootPOA pseudo object.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *
     *@return A reference to the RootPOA pseudo object
     *
     *@exception CORBA::SystemException Any CORBA system exception
     */ 
    static 
    PortableServer::POA_ptr
    Get_root_POA(CORBA::ORB_ptr orb)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);

    /**
     * Purpose:
     * <p> Returns the reference of the CDMW upper most POA, parent of all
     * CDMW-specific POAs.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *
     *@return A reference to the CDMW RootPOA pseudo object
     *
     *@exception CORBA::SystemException Any CORBA system exception
     */ 
    static 
    PortableServer::POA_ptr
    Create_cdmw_root_POA(CORBA::ORB_ptr orb)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);

      
    /**
     * Purpose:
     * <p> Retrieves the CDMW Process XML initialisation file name
     * from Program command line arguments.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *@param argc The number of arguments passed to the program
     *@param argv The list of parameters received of the command line	
     *
     *@return The name of current CDMW Process XML initialisation file name
     *
     *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes 
     * details tha reason: <B>BAD_PARAMProcessXMLFileNameMissing</B> when the CDMW
     * Process XML initialisation file name is missing from the command line arguments
     *@exception CORBA::NO_MEMORY OsSupport library could not allocate memory
     *@exception CORBA::INTERNAL  Internal Error within OsSupport library
     */ 
    static
    std::string 
    Get_xml_initialisation_file(int & argc, char** argv)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>);




    /**
     * Purpose:
     * <p> Retrieves the CDMW Process XML initialisation file validation option.
     * It instructs the XML parser whether to validate the XML document or not.
     * This will return <B>true</B> XML document willis validated whenever 
     * VALIDATE_XML_OPTION appears in the argument list.
     *
     *@param argc The number of arguments passed to the program
     *@param argv The list of parameters received of the command line	
     *
     *@return <B>true</B> if the XML document has to be validated by the XML 
     * Parser,
     * <B>false</B> otherwise.
     *
     *@exception CORBA::NO_MEMORY OsSupport library could not allocate memory
     *@exception CORBA::INTERNAL  Internal Error within OsSupport library
     */ 
    static
    bool
    Get_xml_validation_option(int & argc, char** argv)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>);


    static void
    Get_parameters(CdmwInitConfiguration& xml_data,
                   std::string&  multicast_port,
                   std::string&  multicast_domain,
                   std::string&  multicast_IP_address,
                   int&          max_transaction_in_progress,
                   int&          max_transaction_done,
                   Cdmw::OsSupport::OS::Timeval& cohort_timeout,
                   Cdmw::OsSupport::OS::Timeval& coordinator_timeout,
                   Cdmw::OsSupport::OS::Timeval& freeze_timeout,
                   long&         number_of_scheduler_thread,
                   long&         chunk_size,
                   long&         freeze_size)
          throw (BadParameterException, XMLErrorException, OutOfMemoryException);

    static void
    Get_datastore_parameters(CdmwInitConfiguration& xml_data,
                             const char*   datastore_name,
                             int           datastoreInstance,
                             CORBA::ULong& datastore_id)
        throw (BadParameterException, XMLErrorException, OutOfMemoryException);

private:
    // hide constructor and destructor
    InitUtils();
    ~InitUtils();

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_CDMW_PLATFORMMNGT_INIT_UTILS_HPP


   
