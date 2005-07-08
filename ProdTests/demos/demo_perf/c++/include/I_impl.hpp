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


#ifndef INCL_I_IMPL_HPP
#define INCL_I_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "iostream"
#include "Benchmark.skel.hpp"


namespace DemoPerf {

class I_impl: public virtual POA_Benchmark::I
{
    
    public:
    
        //
        // IDL:thalestgroup.com/Benchmark/I/foo:1.0
        //
        virtual void foo() 
            throw(CORBA::SystemException)
        {
            std::cout << "   I says FOO !!" << std::endl;
        };
   
    
}; // End class I_impl    

}; // End namespace DemoPerf


#endif  // INCL_I_IMPL_HPP

