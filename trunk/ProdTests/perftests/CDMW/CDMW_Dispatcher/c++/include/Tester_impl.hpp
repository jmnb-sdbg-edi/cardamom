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
 
 

#ifndef INCL_TESTER_IMPL_HPP
#define INCL_TESTER_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"

#include "Dispatcher.skel.hpp"

namespace Dispatcher {

class Tester_impl: public virtual POA_Dispatcher::Tester
{
    
    public: 

        virtual void ping()
            throw(CORBA::SystemException);
    
}; // End class Tester_impl    
    
}; // End namespace Dispatcher

#endif // INCL_TESTER_IMPL_HPP

