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


#ifndef INCL_TESTSEQUENCER_HPP
#define INCL_TESTSEQUENCER_HPP

#include "Foundation/testutils/TestManager.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"
#include "testplatformlibrary/SynchronisableTaskChecker.hpp"
#include <string>


class TestSequencer : public CppUnit::TestFixture
{

public:

    TestSequencer();

    virtual ~TestSequencer();

protected:

	bool checkTaskSuccessors(
        const Cdmw::PlatformMngt::TaskIdList& resultList,
        const std::string& taskId,
        const Cdmw::PlatformMngt::TaskIdList& successors);

	bool checkDependencyGraph(
        const Cdmw::PlatformMngt::TaskIdList& resultList,
        const Cdmw::PlatformMngt::SynchronisableTaskDefMap& taskDefMap);

    void checkExecution(const SynchronisableTaskChecker& taskChecker,
        const Cdmw::PlatformMngt::SynchronisableTaskDefMap& taskDefMap);

	virtual void do_tests();

CPPUNIT_TEST_SUITE(TestSequencer);
CPPUNIT_TEST( do_tests );
CPPUNIT_TEST_SUITE_END();
};

#endif // INCL_TESTSEQUENCER_HPP

