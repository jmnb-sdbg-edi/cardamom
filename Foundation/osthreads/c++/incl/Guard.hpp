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

#ifndef _CDMW_FOUNDATION_OSTHREADS_GUARD_H_
#define _CDMW_FOUNDATION_OSTHREADS_GUARD_H_

#include <Foundation/common/Void.hpp>
#include "Foundation/common/Exception.hpp"
            
namespace Cdmw {
    namespace OsSupport {
        template <class LockType>
        class Guard;

        template <>
        class Guard< ::Cdmw::Common::Void >; 
    }
}

/**
 * This class provides a way of implementing the strategized guarded
 * lock idiom.
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 */
template <class LockType>
class Cdmw::OsSupport::Guard {
public:
    /**
     * Creates a <code>Guard</code> object and automatically acquires
     * the lock.
     *
     * @param lock the lock.
     */
    explicit Guard(LockType &lock) 
        throw(AlreadyDoneException,
              InternalErrorException)
        : m_lock(lock)
        {
            m_lock.lock();
        }

    /**
     * Destroys a <code>Guard</code> object and automatically releases
     * the lock.
     *
     */

    ~Guard() throw() {
        try {
            m_lock.unlock();
        }
        catch (...) { }
    }

private:
    Guard(const Guard&);
    Guard& operator=(const Guard&);
    
private:
  LockType& m_lock;
};


template <>
class Cdmw::OsSupport::Guard<Cdmw::Common::Void> {
public:
    explicit Guard(::Cdmw::Common::Void&)
        throw(AlreadyDoneException,
              InternalErrorException);
    
    ~Guard() throw();

private:
    Guard(const Guard&);
    Guard& operator=(const Guard&);
};

#endif /* _CDMW_FOUNDATION_OSTHREADS_GUARD_H_ */
