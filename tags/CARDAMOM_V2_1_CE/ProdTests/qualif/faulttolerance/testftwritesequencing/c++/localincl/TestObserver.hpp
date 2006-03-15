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


#ifndef INCL_TEST_OBSERVER_IMPL_HPP 
#define INCL_TEST_OBSERVER_IMPL_HPP

#include <Foundation/commonsvcs/datastore/DataStoreObserver.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "testftwritesequencing/TestSequenceRecorder.skel.hpp"
#include <iostream>


namespace Cdmw
{
    class Observer_impl : public Cdmw::CommonSvcs::DataStore::DataStoreObserver<int, std::string>
    {

    public:
//         typedef Cdmw::FT::DataStoreObserver<Oid, Data> DataStoreObserver;
//         typedef typename DataStoreObserver::_Oid_in_type _Oid_in_type;
//         typedef typename DataStoreObserver::_Data_in_type _Data_in_type;

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */        
        Observer_impl(CORBA::ORB_ptr orb, const char* host)
            : m_host(host)
        {
            m_orb = CORBA::ORB::_duplicate(orb);
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
            init();
            recorder->insert(m_host.c_str(), oid, data.c_str());
        }

        void on_update(_Oid_in_type oid, _Data_in_type data)
            throw ()
        {
            init();
            recorder->update(m_host.c_str(), oid, data.c_str());
        }

        void on_remove(_Oid_in_type oid)
            throw ()
        {
            init();
            recorder->remove(m_host.c_str(), oid);
        }

    private:

        // Hide copy constructor/assignment operator
        Observer_impl(const Observer_impl& rhs)
            throw();
        
        Observer_impl&
        operator=(const Observer_impl& rhs)
            throw();
        
        void init()
        {
            static bool initialised = false;

            if (!initialised)
            {
                initialised = true;
                CdmwNamingAndRepository::Repository_var repository
                    = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

                Cdmw::CommonSvcs::Naming::NamingInterface ni =
                    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2/dom3");
                CORBA::Object_var obj;

                try
                {
                    obj = ni.resolve ("P12Recorder");
                    recorder = Test::SequenceRecorder::_narrow(obj.in());
                }
                catch(const CosNaming::NamingContext::NotFound& e)
                {
                    std::cout<< e << std::endl;
                }
                catch(const CosNaming::NamingContext::CannotProceed& e)
                {
                    std::cout<< e << std::endl;
                }
                catch(const CosNaming::NamingContext::InvalidName& e)
                {
                    std::cout<< e << std::endl;
                }
                catch (const CORBA::SystemException& e)
                {
                    std::cout<< e << std::endl;
                }
            }

        }

        /**
         * The ORB.
         */
        CORBA::ORB_var m_orb;
        std::string m_host;
        Test::SequenceRecorder_var recorder;

    }; // End class Observer_impl


}; // End namespace Cdmw

#endif // INCL_TEST_OBSERVER_IMPL_HPP

