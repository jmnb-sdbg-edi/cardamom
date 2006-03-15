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

#ifndef _CDMW_FT_FACTORYINFOS_HPP_
#define _CDMW_FT_FACTORYINFOS_HPP_

// Standard Files 
#include <string>
//#include <Foundation/orbsupport/CORBA.hpp>
#include "FaultTolerance/idllib/FT.stub.hpp"

namespace Cdmw {
namespace FT {


    class FactoryInfos
    {
    public:
        
        /**
         * Constructor
         */       

        FactoryInfos() throw();
        
        /**
         * Destructor
         */
        ~FactoryInfos() throw();

        /**
         * add the FT location
         *
         * @param hostname        Host name of the location     
         * @param applicationName Application name of the location
         * @param processName     Process name of the location
         */
        void
        add_ftLocation(const std::string hostName,
		       const std::string applicationName,
		       const std::string processName);

        /**
         * get the current sequence of FactoryInfo
         *
         * @return the FactoryInfo sequence
         */
        ::FT::FactoryInfos
	get_factoryInfos()
        {
	  return m_factoryInfos;
	}

    private:

        /**
         * the sequence of FactoryInfo containing each location for each object
         */
        ::FT::FactoryInfos m_factoryInfos;
    };    
    

 
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_FACTORYINFOS_HPP_

