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

#ifndef INCL_PRINTER_IMPL_HPP 
#define INCL_PRINTER_IMPL_HPP 

#include <string>


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <LifeCycle/lifecycle/LifeCycleSingleObject.hpp>

#include "PrinterModule.skel.hpp"


namespace Cdmw
{
namespace Tutorial
{


class Printer_impl :
        public virtual POA_PrinterModule::Printer,
        public Cdmw::LifeCycle::LifeCycleSingleObject,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{

    public:

 
        Printer_impl(const Cdmw::LifeCycle::ObjectDeactivator & obj_d)
            throw(CORBA::SystemException);


        virtual 
        ~Printer_impl()
                throw();
    
         
        //
        // IDL:thalesgroup.com/PrinterModule/Printer/print_message:1.0
        //
        virtual void print_message (const char* message)
            throw(CORBA::SystemException);
                  

    private:

        // Hide default constructor
        Printer_impl()
            throw(CORBA::SystemException);
            
            
        // Hide copy constructor/assignment operator
        Printer_impl(const Printer_impl& rhs)
                throw();
        
        Printer_impl&
        operator=(const Printer_impl& rhs)
                throw();


}; // End class Printer_impl 

}; // End namespace Tutorial
}; // End namespace Cdmw

#endif // INCL_PRINTER_IMPL_HPP 
