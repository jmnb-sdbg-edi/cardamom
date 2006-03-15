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


#ifndef _CDMW_FT_MANAGER_INIT_UTILS_HPP_
#define _CDMW_FT_MANAGER_INIT_UTILS_HPP_


#include "ftmanager/FaultDetector_impl.hpp"
#include "ftmanager/FaultNotifier_impl.hpp"
#include "ftmanager/FTManager_impl.hpp"
#include "ftmanager/MutualMonitoringObserver_impl.hpp"
#include "ftmanager/TypeRepository_impl.hpp"
#include <iostream>

#include "FaultTolerance/ftreplicationmanager/InitUtils.hpp"
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTTypeRepository.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
#include "FaultTolerance/ftmonitoring/InitUtils.hpp"
#include "ftmanager/ConfigurationMngr.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/String.hpp"


DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::FT::Properties);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::ObjectGroupData);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::LocationInfo);

namespace Cdmw {
namespace FT {
namespace FTManager {
        

    /**
     * Purpose:
     * <p> This class provides helper functions to be used to instanciate
     * a ftmanager
     *

     */
    class InitUtils
    {
    public:

        /**
         * Purpose:
         * <p> pre_init
         * 
         *@exception CORBA::SystemException
         *@return the port number
         */        
        static 
        std::string
        Pre_init(int& argc, char** argv)
            throw(ConfigurationException, CORBA::SystemException);
        

        /** Purpose:
         * <p> post_init 
         * 
         *@exception CORBA::SystemException
         */    
        static
        void
        Post_init (int& argc, char** argv, CORBA::ORB_ptr orb)
            throw(ConfigurationException, CORBA::SystemException);


        /** Purpose:
         * <p> cleanup
         * 
         *@exception CORBA::SystemException
         */    
        static
        void
        Cleanup ()
            throw(CORBA::SystemException);
        
    private:
        
        static FTManagerDataStore * M_ftManagerDataStore ;
        static IOGRInfoDataStore * M_iogrInfoDataStore ;
        static ConsumerIdDataStore * M_consumerIdDataStore ;
        static SequencePushConsumerDataStore * M_sequencePushConsumerDataStore ;
        static StructuredPushConsumerDataStore * M_structuredPushConsumerDataStore ;
        static HostDataStore * M_hostDataStore ;
        static MonitoringIdDataStore * M_monitoringIdDataStore ;
        static HostStatusDataStore * M_hostStatusDataStore ;
        static LocInfoDataStore * M_locInfoDataStore ;
        static OGIDDataStore * M_ogidDataStore ;
        static OGDDataStore * M_ogdDataStore ;
        static PropertiesDataStore * M_propertiesDataStore ;
        static Cdmw::FT::DataStoreContext * M_context ;

        static std::string M_xml_conf;
        static std::string M_xml_group_conf;
        static bool M_validate;
        

    }; // End class InitUtils
    
}; // namespace FTManager
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_MANAGER_INIT_UTILS_HPP_

