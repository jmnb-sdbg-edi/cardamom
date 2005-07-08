/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#ifndef CARDAMOM_CLOCK_CLOCK_CATALOG_IMPL_HPP
#define CARDAMOM_CLOCK_CLOCK_CATALOG_IMPL_HPP

/**
 * @file
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

// -- Clock Service Includes --
#include <Time/clockservice/CosClockService.skel.hpp>

// -- STD C++ Includes --
#include <Time/clockservice/config.hpp>
#include <map>
#include <string>

namespace Cdmw { namespace clock {
    
/**
 * @brief   This class implements the basic clock Catalog functionalities
 * described in the EVoT specification
 *
 * This class implements the basic clock Catalog functionalities
 * described in the EVoT specification.  Allows register, delete
 * and find voice in/from the clock catalog.
 *
 * @author Lello Mele <lellomele@libero.it>
 */
class CLOCK_API ClockCatalog_impl :
    public virtual POA_CosClockService::ClockCatalog
{
public:
    // Constructor
    ClockCatalog_impl (void);
    
    // Destructor
    virtual ~ClockCatalog_impl (void);
    
public:
    /**
     * Return a Clock from it's name.
     *
     * @params with_name The string name of the clock voice.
     * @return ClockEntry stucture with clock voice details.
     */
    virtual ::CosClockService::ClockCatalog::ClockEntry*
    get_entry(const char * with_name )
        throw (CORBA::SystemException,
               ::CosClockService::ClockCatalog::UnknownEntry);
        
    /**
     * Return all Clock entries from the Clock Catalog.
     */
    virtual ::CosClockService::ClockCatalog::ClockEntries*
    available_entries()
        throw ( CORBA::SystemException );

    /**
     * Register a Clock in the Catalog.
     *
     * @param entry ClockEntry structure with clock voice details.
     */
    virtual void _cxx_register(const ::CosClockService::ClockCatalog::ClockEntry& entry)
        throw (CORBA::SystemException);
        
    /**
     * Delete a Clock voice from the Catalog.
     *
     * @param with_name The string name of the clock voice.
     */
    virtual void delete_entry(const char* with_name)
        throw (CORBA::SystemException, ::CosClockService::ClockCatalog::UnknownEntry);
        
private:
    // Some useful type definition.
    typedef std::map<std::string, CosClockService::Clock_var> ClockMapT;
    typedef ClockMapT::const_iterator ClockMapT_CI;
    typedef ClockMapT::iterator ClockMapT_I;
    // Internal Clock Catalog implementation
    ClockMapT catalog_;
};

}/* Cdmw */ 
} /* clock */

#endif /* CARDAMOM_CLOCK_CLOCK_CATALOG_IMPL_HPP  */
