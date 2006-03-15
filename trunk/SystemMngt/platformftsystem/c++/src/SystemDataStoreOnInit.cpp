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
 
#include "platformftsystem/SystemDataStoreOnInit.hpp"
#include "platformlibrary/Configuration.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// SystemDataStoreOnInit class.
// ----------------------------------------------------------------------

SystemDataStoreOnInit::SystemDataStoreOnInit (SystemAccessor_impl* p_system_accessor)
  throw()
    : m_system_accessor (p_system_accessor)
{      
}


SystemDataStoreOnInit::~SystemDataStoreOnInit()
  throw()
{
}

void SystemDataStoreOnInit::on_init(DataInsertIterator it, bool cold_restart) 
  throw (Cdmw::InitErrorException)
{
    std::cout << "<<SystemDataStoreOnInit::on_init()>>" << std::endl;
    
    // set first primary
    std::cout << "<<SystemDataStoreOnInit::on_init !!set process as first primary !!>>" << std::endl;
    Configuration::Set_first_primary();
    
    // connect the ft converter to the Fault Notifier of the FT Manager
    m_system_accessor->connect_ft_converter();
    
    std::cout << "<<FTConverter>> is connected to the FaultNotifier" << std::endl;
}

 

} // End namespace PlatformMngt
} // End namespace Cdmw

