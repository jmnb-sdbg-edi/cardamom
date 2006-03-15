/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
*/
/* ===================================================================== */


#ifndef INCL_TEST_OBSERVER_IMPL_HPP 
#define INCL_TEST_OBSERVER_IMPL_HPP

#include <iostream>
#include <map>

#include <Foundation/commonsvcs/datastore/DataStoreObserver.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>



namespace Cdmw
{
    class Observer_impl : 
       public Cdmw::CommonSvcs::DataStore::DataStoreObserver<std::string, 
                                                             CdmwFT::ReplyData>
    {

    public:

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */        
        Observer_impl()
        {
        }
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        ~Observer_impl() throw() 
        {
        }
        
    
        void on_insert(_Oid_in_type oid, _Data_in_type data)
            throw ()
        {
           std::cout << "*********** Observer => on_insert called oid="
                     << oid
                     << std::endl;

        }

        void on_update(_Oid_in_type oid, _Data_in_type data)
            throw ()
        {
        }

        void on_remove(_Oid_in_type oid)
            throw ()
        {
           std::cout << "*********** Observer => on_remove called oid="
                     << oid
                     << std::endl;
        }

    private:

        // Hide copy constructor/assignment operator
        Observer_impl(const Observer_impl& rhs)
            throw();
        
        Observer_impl&
        operator=(const Observer_impl& rhs)
            throw();
        
    }; // End class Observer_impl


}; // End namespace Cdmw

#endif // INCL_TEST_OBSERVER_IMPL_HPP

