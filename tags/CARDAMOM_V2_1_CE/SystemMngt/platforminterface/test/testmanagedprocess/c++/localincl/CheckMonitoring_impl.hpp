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


#ifndef INCL_TESTMANAGEDPROCESS_CHECKMONITORING_IMPL_HPP
#define INCL_TESTMANAGEDPROCESS_CHECKMONITORING_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"

#include "testmanagedprocess/TestManagedProcess.skel.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "testmanagedprocess/MyProcessBehaviour.hpp"

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

class CheckMonitoring_impl :
    public virtual POA_Cdmw::TestManagedProcess::CheckMonitoring
{

    public:

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */
        CheckMonitoring_impl()
            throw();

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */
        CheckMonitoring_impl(MyProcessBehaviour* behaviour)
            throw();

        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */
        virtual
        ~CheckMonitoring_impl()
            throw();

        virtual
        CORBA::Short
        get_counter()
            throw(CORBA::SystemException);

    protected:

    private:

        MyProcessBehaviour* m_pBehaviour;

}; // End class CheckMonitoring_impl

#endif // INCL_TESTMANAGEDPROCESS_CHECKMONITORING_IMPL_HPP

