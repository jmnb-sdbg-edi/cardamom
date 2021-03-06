/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#include "testlbgroupmanager/TestHello_impl.hpp"

#include <string>
#include <iostream>
using namespace std;

/**
 * @file
 * @brief Hello Interface implementation.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */
namespace Cdmw
{

/**
 * @brief Constructor.
 *
 */
    HelloInterface1_impl::HelloInterface1_impl()
    throw(CORBA::SystemException)
    { 
    }

    
/**
 * @brief Destructor.
 *
 */
    
    
    HelloInterface1_impl::~HelloInterface1_impl()
        throw()
    {
    }
    
/**
 * @brief IDL HelloInterface1 definition.
 *
 */
    void HelloInterface1_impl::hello ()
        throw(CORBA::SystemException)                  
    {
        std::cout << "<####### HELLO 1 ######>" << std::endl;
    }
    
    
/**
 * @brief Costructor.
 *
 */
    
    HelloInterface2_impl::HelloInterface2_impl()
        throw(CORBA::SystemException)
    { 
    }
    
/**
 * @brief Destructor.
 *
 */
    
    HelloInterface2_impl::~HelloInterface2_impl()
        throw()
    {
    }
    
/**
 * @brief IDL HelloInterface2 definition.
 *
 */
    void HelloInterface2_impl::hello ()
        throw(CORBA::SystemException)                  
    {
        std::cout << "<####### HELLO 2 ######>" << std::endl;
    }
    
    
    
}; // End namespace Cdmw

