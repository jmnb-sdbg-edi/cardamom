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


#ifndef INCL_VALUE_IMPL_HPP
#define INCL_VALUE_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include "Benchmark.stub.hpp"


namespace DemoPerf {
    
class Value_impl: public virtual OBV_Benchmark::Value {

    public:
    
        virtual CORBA::ValueBase* _copy_value();
    
};
    
    
class ValueFactory: virtual public CORBA::ValueFactoryBase {

    virtual CORBA::ValueBase* create_for_unmarshal();
    
};    



}; // End of namespace DemoPerf

#endif // INCL_VALUE_IMPL_HPP

