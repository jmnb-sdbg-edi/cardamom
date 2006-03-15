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

#ifndef INCL_FT_TESTDATASTORE_HPP
#define INCL_FT_TESTDATASTORE_HPP

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "datastoretest/DatastoreDefinition.hpp"


namespace Cdmw 
{
namespace FT
{

	class TestDataStore : public TestUtils::Testable
{

    public:

       /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestDataStore(DataPart1StorageHome* dataPart1StorageHome,
			             DataPart2StorageHome* dataPart2StorageHome,
			             short scenarioIndex,
                         bool realOverlap)
                throw();

        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestDataStore(const TestDataStore& rhs)
                throw();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~TestDataStore()
                throw();


        /**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */ 
        TestDataStore&
        operator=(const TestDataStore& rhs)
                throw();

        /**
        *[Attribute description]
        */ 


    protected:

        virtual void do_tests();

    private:
         struct CheckResult
         {
                 // Which interval has the oldest timestamp (begin from 1)
                 unsigned short firstInterval;
                 // Indice, in the firstInterval, of last timestamp before the first
                 // of the other interval. If=-1 there was not overlap of timestamps
                 int overlapIndice;
                 // Indice, in the firstInterval, of an operation that has not finished
                 // while a second operation has began (more real parallelism)
                 // (this indice is >= overlapIndice)
                 int realOverlapIndice1;
                 // Indice, in the second interval, of the "second operation" (see above)
                 int realOverlapIndice2;
         };

         CheckResult checkTimestamps(int inter1[], int inter2[]);

		 DataPart1StorageHome* m_dataPart1StorageHome;
  	     DataPart2StorageHome* m_dataPart2StorageHome;

		 short m_scenarioIndex;
         bool  m_realOverlap;

         static bool                       M_predWaitOk;
         static Cdmw::OsSupport::Mutex     M_synchroMutex;
         static Cdmw::OsSupport::Condition M_synchroCondition;
         static bool                       M_pred12WaitOk;
         static Cdmw::OsSupport::Mutex     M_synchro12Mutex;
         static Cdmw::OsSupport::Condition M_synchro12Condition;
         static bool                       M_pred21WaitOk;
         static Cdmw::OsSupport::Mutex     M_synchro21Mutex;
         static Cdmw::OsSupport::Condition M_synchro21Condition;

         static bool M_continueLoop;

         static const int MAPSIZE = 300*SIZESCALE;
         static Cdmw::OsSupport::OS::Timeval M_beforeOp[MAPSIZE];
         static Cdmw::OsSupport::OS::Timeval M_afterOp[MAPSIZE];

}; // End class TestDataStore


}; // End namespace FT

}; // End namespace Cdmw 


#endif  // INCL_FT_TESTDATASTORE_HPP
