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


#include "LifeCycle/lifecycle/ServantProviderBase.hpp"
#include "LifeCycle/lifecycle/CleanupNotifier.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        ServantProviderBase::ServantProviderBase()
        throw()
                : m_cleanup_notifier(0)
        {
        }


        ServantProviderBase::~ServantProviderBase()
        throw()
        {
            m_cleanup_notifier = 0;
        }

        bool
        ServantProviderBase::supports(const std::string &repository_id) const
        throw()
        {
            bool result = false;
            const char** ids = get_ids();
            const CORBA::ULong nb_ids = get_number_of_ids();

            for (CORBA::ULong i = 0 ; i < nb_ids ; ++i)
                if (strcmp(repository_id.c_str(), ids[i]) == 0)
                {
                    result = true;
                    break;
                }

            return result;
        }

        CdmwLifeCycle::RepositoryIdSeq*
        ServantProviderBase::get_supported_objects() const
        throw()
        {
            const char** ids = get_ids();
            const CORBA::ULong nb_ids = get_number_of_ids();

            CdmwLifeCycle::RepositoryIdSeq_var rep_ids
            = new CdmwLifeCycle::RepositoryIdSeq(nb_ids);
            rep_ids->length (nb_ids);

            for (CORBA::ULong i = 0; i < nb_ids ; ++i)
                rep_ids[i] = CORBA::string_dup(ids[i]);

            return rep_ids._retn();
        }


        // NOTA: notifier should not be destroyed!
        void ServantProviderBase::set_cleanup_notifier(CleanupNotifier * notifier)
        throw()
        {
            // assert (notifier != 0);
            m_cleanup_notifier = notifier;
        }

        //void ServantProviderBase::cleanup_object(const std::string & object_name)
        void ServantProviderBase::cleanup_object(const PortableServer::ObjectId & oid)
        throw()
        {
            if (m_cleanup_notifier)
                m_cleanup_notifier->unregister_object(oid);
        }

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

