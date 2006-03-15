/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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
 
#ifndef CARDAMOM_CLOCK_ATOMIC_HPP
#define CARDAMOM_CLOCK_ATOMIC_HPP

#include <bits/atomicity.h>

#if defined(__GLIBCXX__) // g++ 3.4+

using __gnu_cxx::__atomic_add;
using __gnu_cxx::__exchange_and_add;

#endif

class AtomicCount
{
public:

    explicit AtomicCount(long v) : value_(v) {}

    long operator++(int)
    {
        return __exchange_and_add(&value_, 1);
    }

    long operator--(int)
    {
        return __exchange_and_add(&value_, -1);
    }

    operator long() const
    {
        return __exchange_and_add(&value_, 0);
    }

private:

    AtomicCount(AtomicCount const &);
    AtomicCount& operator=(AtomicCount const &);

protected:

    mutable _Atomic_word value_;
};

#endif // #ifndef CARDAMOM_CLOCK_ATOMIC_HPP

