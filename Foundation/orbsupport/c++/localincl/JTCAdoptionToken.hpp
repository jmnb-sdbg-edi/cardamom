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


#ifndef INCL_ORBSUPPORT_JTC_ADOPTION_TOKEN_HPP
#define INCL_ORBSUPPORT_JTC_ADOPTION_TOKEN_HPP

#if CDMW_ORB_VDR == orbacus

#include "Foundation/orbsupport/ThreadAdoptionToken.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include <JTC/Thread.h>

namespace Cdmw
{

    namespace OrbSupport
    {
        /**
         *Purpose:
         *<p>    This class provide a ThreadAdoptionToken specific to the JTC Thread
         * library used by Orbacus/C++ ORB.
         *@see Cdmw::OrbSupport::ThreadAdoptionToken
         *Features:
         *<p> Thread safe
         *
         */

        class JTCAdoptionToken : public ThreadAdoptionToken
        {

            public:
                /**
                 * Purpose:
                 * <p> Constructor initialises internal reference count to 1.
                 */
                JTCAdoptionToken() throw(JTCSystemCallException) : m_ref_count(1)
                {}

                /**
                 *@see OrbSupport::ThreadAdoptionToken::_add_ref
                 */
                virtual void _add_ref() throw();

                /**
                 *@see OrbSupport::ThreadAdoptionToken::_remove_ref
                 */
                virtual void _remove_ref() throw();

            protected:
                virtual ~JTCAdoptionToken() throw();

                JTCAdoptionToken(const JTCAdoptionToken &) throw(JTCSystemCallException)
                        : m_ref_count(1)
                {}

                JTCAdoptionToken&
                operator=(const JTCAdoptionToken&)
                {
                    return * this;
                }

            private:
                /**
                 * A Mutex to protect from concurrent access to internal
                 * reference counter.
                 */
                Cdmw::OsSupport::Mutex m_count_mutex;

                /**
                 * Instance reference counter.
                 */
                unsigned long m_ref_count;

                /**
                 * This object registers the current calling thread, within its 
                 * constructor, and unregisters it in its destructor.
                * JTCAdoptCurrentThread constructor may throw JTCSystemCallException
                * in case of a failure of system call.
                 */
                JTCAdoptCurrentThread dummy;
        }

        ; // End class JTCAdoptionToken


    } // End namespace OrbSupport
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == orbacus

#endif // INCL_ORBSUPPORT_JTC_ADOPTION_TOKEN_HPP

