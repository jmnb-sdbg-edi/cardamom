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

#ifndef PERFORMANCE_MANAGER_HPP
#define PERFORMANCE_MANAGER_HPP

/**
 * @brief Performance Manager class.
 * 
 * This class occurs to insulate performance test requirements in usual tests.
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 */

#include <cppunit/extensions/HelperMacros.h>

class PerformanceException 
:   public CppUnit::Exception
{
public:
    PerformanceException(const CppUnit::Message &message=CppUnit::Message(), 
        const CppUnit::SourceLine &sourceLine=CppUnit::SourceLine())
    {
    }

};

class PerformanceManager
{
    bool    is_active_;
public:
        
    PerformanceManager();
    ~PerformanceManager();
    static  PerformanceManager* get_instance();
    void    activate();
    void    de_activate();
    void    check_CDMW_ASSERTion(bool,
                            const CppUnit::SourceLine& sl);
    bool    is_active() const;
};


#define PERFORMANCE_ASSERT(aValue)\
PerformanceManager::get_instance()->check_CDMW_ASSERTion((bool)(aValue),\
CppUnit::SourceLine (__FILE__,__LINE__));

#endif // PERFORMANCE_MANAGER_HPP
