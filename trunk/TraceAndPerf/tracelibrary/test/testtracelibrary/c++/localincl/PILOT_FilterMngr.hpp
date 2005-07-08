/* =========================================================================== *
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
 * =========================================================================== */


#ifndef INCL_TRACE_PILOT_FILTERMNGR_HPP 
#define INCL_TRACE_PILOT_FILTERMNGR_HPP 

#include "Foundation/testutils/Testable.hpp"
#include "TraceAndPerf/tracelibrary/FilterMngr.hpp"


namespace Cdmw
{
namespace Trace
{

class PILOT_FilterMngr : public TestUtils::Testable
{

    public:
        
        
        PILOT_FilterMngr()
                throw();


 
        virtual 
        ~PILOT_FilterMngr()
                throw();


        

        void
        do_tests()
            throw();


    protected:


    private:


        PILOT_FilterMngr(const PILOT_FilterMngr& rhs)
                throw();


        PILOT_FilterMngr&
        operator=(const PILOT_FilterMngr& rhs)
                throw();


        bool check_filter_seq (FilterMngr& filterMngr, 
                               CdmwTrace::TraceFilterSeq* ref_sequence);
        

}; // End class PILOT_FilterMngr 

}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_TRACE_PILOT_FILTERMNGR_HPP

