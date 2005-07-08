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


#ifndef INCL_TEST_SEQUENCE_RECORDER_IMPL_HPP 
#define INCL_TEST_SEQUENCE_RECORDER_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/StorageHome.hpp>
#include <FaultTolerance/ftstatemanager/StorageObject.hpp>

#include "testftwritesequencing/TestSequenceRecorder.skel.hpp"

#include <string>
#include <list>

namespace Cdmw
{
    typedef std::list<std::string> StringList;

    class SequenceRecorder_impl :public virtual POA_Test::SequenceRecorder,
                                 virtual public PortableServer::RefCountServantBase
    {

    public:
        
       SequenceRecorder_impl (CORBA::ORB_ptr          orb,
                              PortableServer::POA_ptr poa,
                              const char*             hostname)
           throw(CORBA::SystemException);
        
        virtual 
        ~SequenceRecorder_impl()
            throw();
        
        virtual void
        insert(const char * host, CORBA::Long oid, const char * data)
            throw(CORBA::SystemException);
        
        virtual void
        update(const char * host, CORBA::Long oid, const char * data)
            throw(CORBA::SystemException);
        
        virtual void
        remove(const char * host, CORBA::Long oid)
            throw(CORBA::SystemException);

        virtual CORBA::Boolean
        validate(const char * primary)
            throw(CORBA::SystemException);

    private:

        // Hide copy constructor/assignment operator
        SequenceRecorder_impl(const SequenceRecorder_impl& rhs)
            throw();
        
        SequenceRecorder_impl&
        operator=(const SequenceRecorder_impl& rhs)
            throw();
        
        CORBA::ORB_var m_orb;
        PortableServer::POA_var m_poa;

        std::string m_host;
        StringList m_insert_hosts;
        StringList m_update_hosts;
        StringList m_remove_hosts;

    }; // End class SequenceRecorder_impl 

}; // End namespace Cdmw

#endif // INCL_TEST_SEQUENCE_RECORDER_IMPL_HPP
