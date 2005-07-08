/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_PILOT_PLATFORMDAEMON_HPP 
#define INCL_PLATFORMMNGT_PILOT_PLATFORMDAEMON_HPP 

#include "Foundation/testutils/Testable.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose:
*<p>   
*
*Features:
*<p> Thread safe
*
*
*/
class PILOT_PlatformDaemon : public TestUtils::Testable
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        PILOT_PlatformDaemon()
                throw();



        /**
        * Purpose:
        * <p> Destructor 
        * 
        */ 
        virtual 
        ~PILOT_PlatformDaemon()
                throw();


        
        /**
        * Purpose:
        * <p> Call from the testable object 
        * 
        */ 
        virtual 
        void
        do_tests()
                throw();




    protected:


    private:

        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        PILOT_PlatformDaemon&
        operator=(const PILOT_PlatformDaemon& rhs)
                throw();
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        PILOT_PlatformDaemon (const PILOT_PlatformDaemon& rhs)
                throw();
        

};

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_PILOT_PLATFORMDAEMON_HPP

