/* =========================================================================== *
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
 * =========================================================================== */

#include <string>


#include "ColorPrinter_impl.hpp"

namespace Cdmw
{
namespace Tutorial
{


ColorPrinter_impl::ColorPrinter_impl(const Cdmw::LifeCycle::ObjectDeactivator & obj_d)
    throw(CORBA::SystemException)
    : Cdmw::LifeCycle::LifeCycleSingleObject(obj_d)
{ 
    std::cout << "<# ColorPrinter servant constructed #>" << std::endl;
    
    m_mode = PrinterModule::ColorPrinter::BlackAndWhite;
}


ColorPrinter_impl::~ColorPrinter_impl()
    throw()
{
    std::cout << "<# ColorPrinter servant destroyed #>" << std::endl;
}


//
// IDL:thalesgroup.com/PrinterModule/ColorPrinter/print_message:1.0
//
void ColorPrinter_impl::print_message (const char* message)
     throw(CORBA::SystemException)                  
{
    if (m_mode == PrinterModule::ColorPrinter::BlackAndWhite)
    {
        std::cout << "---> print in black and white" << std::endl;
    }
    else
    {
        std::cout << "---> print in color" << std::endl;
    }
        
    std::cout << message << std::endl;
}


//
// IDL:thalesgroup.com/PrinterModule/ColorPrinter/set_color:1.0
//    
void ColorPrinter_impl::set_color (PrinterModule::ColorPrinter::ColorMode mode)
     throw(CORBA::SystemException)
{
    m_mode = mode;
}


//
// set to print in color
//    
void ColorPrinter_impl::set_color()
{
    m_mode = PrinterModule::ColorPrinter::FullColor;
}

                
//
// set to print in black and white
//    
void ColorPrinter_impl::set_blackAndWhite()
{
    m_mode = PrinterModule::ColorPrinter::BlackAndWhite;
}
 
                  

}; // End namespace Tutorial
}; // End namespace Cdmw
