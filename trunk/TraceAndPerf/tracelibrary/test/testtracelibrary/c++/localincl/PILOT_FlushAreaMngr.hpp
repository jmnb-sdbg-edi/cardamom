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


#ifndef INCL_TRACE_PILOT_FLUSHAREAMNGR_HPP 
#define INCL_TRACE_PILOT_FLUSHAREAMNGR_HPP 


#include "Foundation/testutils/Testable.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class PILOT_FlushAreaMngr : public TestUtils::Testable
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        PILOT_FlushAreaMngr();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~PILOT_FlushAreaMngr()
                throw();



        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual 
	    void
        do_tests()
                throw();


    protected:


    private:

        //
	    // Copy ctor
	    //
        PILOT_FlushAreaMngr(const PILOT_FlushAreaMngr& rhs)
                throw();


        //
	    // Assignement operator
	    //
        PILOT_FlushAreaMngr&
        operator=(const PILOT_FlushAreaMngr& rhs)
                throw();



        

}; // End class PILOT_FlushAreaMngr 

}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_TRACE_PILOT_FLUSHAREAMNGR_HPP

