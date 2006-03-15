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


#include "Foundation/orbsupport/StrategyList.hpp"

#include <algorithm>

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Implementation notes:  none
        *<p>
        *Portability issues: none 
        *<p>
        */


        StrategyList::StrategyList()
        throw()
                : m_has_orbThreaded(false),

                m_has_poaThreadPerRequest(false),
                m_has_poaThreadPerConnection(false),
                m_has_poaThreadPool(false),
                m_nbPoaThreads(0),
                m_has_multicast(false),
                m_has_poaPortAssigned(false),
                m_poaPort(0)
        {
            // Nothing to do
        }


        StrategyList::StrategyList( const StrategyList& rhs )
        throw()
                :
                // Orb thread pool
                m_has_orbThreaded( rhs.m_has_orbThreaded ),

                //  Poa threaded
                m_has_poaThreadPerRequest( rhs.m_has_poaThreadPerRequest ),
                m_has_poaThreadPerConnection( rhs.m_has_poaThreadPerConnection ),

                //  Poa thread pool
                m_has_poaThreadPool( rhs.m_has_poaThreadPool ),
                m_nbPoaThreads( rhs.m_nbPoaThreads ),

                // Multicast capability
                m_has_multicast( rhs.m_has_multicast ),

                // Poa port
                m_has_poaPortAssigned( rhs.m_has_poaPortAssigned ),
                m_poaPort( rhs.m_poaPort )

        {

            // Nothing to do

        }


        //
        // Assignement operator
        //
        StrategyList&
        StrategyList::operator=( const StrategyList& rhs )
        throw()
        {

            StrategyList temp( rhs );
            swap ( temp );

            return *this;
        }



        //
        // Dtor
        //
        StrategyList::~StrategyList()
        throw()
        {
            // Nothing to do
        }



        //
        // Swap both objects
        //
        void
        StrategyList::swap( StrategyList& rhs )
        throw()
        {
            // Orb threaded
            std::swap( m_has_orbThreaded, rhs.m_has_orbThreaded );

            // Poa threaded
            std::swap( m_has_poaThreadPerRequest, rhs.m_has_poaThreadPerRequest );
            std::swap( m_has_poaThreadPerConnection, rhs.m_has_poaThreadPerConnection );

            // Poa thread pool
            std::swap( m_has_poaThreadPool, rhs.m_has_poaThreadPool );
            std::swap( m_nbPoaThreads, rhs.m_nbPoaThreads );

            // Multicast capability
            std::swap( m_has_multicast, rhs.m_has_multicast );

            // Poa port
            std::swap( m_has_poaPortAssigned, rhs.m_has_poaPortAssigned );
            std::swap( m_poaPort, rhs.m_poaPort );

        }



        //
        // Defines the threaded startegy for the ORB
        //
        void
        StrategyList::add_OrbThreaded()
        throw( BadParameterException )
        {
            m_has_orbThreaded = true;
        }


        //
        // Defines the Thread Pool strategy for the POA
        //
        void
        StrategyList::add_PoaThreadPool( std::size_t nbThreads)
        throw( BadParameterException )
        {

            // Check incompatible Strategy
            // ---------------------------------------------------------------------

            if ( m_has_poaThreadPerRequest )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreaded already set");
            }

            if ( m_has_poaThreadPerConnection )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreaded already set");
            }

            // Check already set Strategy
            // ---------------------------------------------------------------------
            if ( m_has_poaThreadPool )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreadPool already set");

            }


            // Check Parameters
            // ---------------------------------------------------------------------
            if ( nbThreads > 0 )
            {
                m_has_poaThreadPool = true;
                m_nbPoaThreads = nbThreads;

            }
            else
            {

                CDMW_THROW2( BadParameterException, "nbThreads", "minimum value is 1");

            }
        }



        //
        // Defines the threaded strategy for the POA
        //
        void
        StrategyList::add_PoaThreadPerRequest()
        throw( BadParameterException )
        {

            // Check incompatible Strategy
            // ---------------------------------------------------------------------

            if ( m_has_poaThreadPool )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreadPool already set");

            }

            if ( m_has_poaThreadPerConnection )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreaded already set");
            }


            // Check already set Strategy
            // ---------------------------------------------------------------------
            if ( m_has_poaThreadPerRequest )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreaded already set");
            }

            // Check Parameters
            // ---------------------------------------------------------------------
            m_has_poaThreadPerRequest = true;

        }


        //
        // Defines the threaded strategy for the POA
        //
        void
        StrategyList::add_PoaThreadPerConnection()
        throw( BadParameterException )
        {

            // Check incompatible Strategy
            // ---------------------------------------------------------------------

            if ( m_has_poaThreadPool )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreadPool already set");

            }

            if ( m_has_poaThreadPerRequest )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreaded already set");
            }


            // Check already set Strategy
            // ---------------------------------------------------------------------
            if ( m_has_poaThreadPerConnection )
            {

                CDMW_THROW2( BadParameterException, "", "PoaThreaded already set");
            }

            // Check Parameters
            // ---------------------------------------------------------------------
            m_has_poaThreadPerConnection = true;

        }



        //
        // Defines that the user want multicast
        //
        void
        StrategyList::add_multicast()
        throw( BadParameterException )
        {
          m_has_multicast = true;
        }

        //
        // Defines the port to be used by a POA
        //
        void
        StrategyList::add_PoaPortAssigned( std::size_t poaPort )
        throw( BadParameterException )
        {

            // Check incompatible Strategy
            // ---------------------------------------------------------------------
            // None


            // Check already set Strategy
            // ---------------------------------------------------------------------

            if ( m_has_poaPortAssigned )
            {

                CDMW_THROW2( BadParameterException, "", "PoaPortAssigned already set");
            }

            // Check Parameters
            // ---------------------------------------------------------------------
            if ( ! (poaPort > 0 && poaPort < 65535) )
            {

                CDMW_THROW2( BadParameterException, "", "The Port is not in the allowed range");

            }

            m_has_poaPortAssigned = true;

            m_poaPort = poaPort;

        }


        //
        // Returns the threaded strategy for the ORB
        //
        bool
        StrategyList::is_OrbThreaded() const
        throw()
        {
            return m_has_orbThreaded;

        }


        //
        // Returns the ThreadPool strategy for the POA
        //
        bool
        StrategyList::is_PoaThreadPool( std::size_t& nbThreads) const
        throw()
        {
            if ( m_has_poaThreadPool )
            {
                nbThreads = m_nbPoaThreads;
            }

            return m_has_poaThreadPool;

        }


        //
        // Returns the ThreadPool strategy for the POA
        //
        bool
        StrategyList::is_PoaThreadPerRequest() const
        throw()
        {

            return m_has_poaThreadPerRequest;

        }


        //
        // Returns the ThreadPool strategy for the POA
        //
        bool
        StrategyList::is_PoaThreadPerConnection() const
        throw()
        {

            return m_has_poaThreadPerConnection;

        }

        //
        // Returns if user want multicast
        //
        bool
        StrategyList::is_multicast() const
        throw()
        {

            return m_has_multicast;

        }


        //
        // Returns the port to be used by the POA
        //
        bool
        StrategyList::is_PoaPortAssigned( std::size_t& poaPort ) const
        throw()
        {

            if ( m_has_poaPortAssigned )
            {
                poaPort = m_poaPort;
            }

            return m_has_poaPortAssigned;

        }

    } // End namespace OrbSupport
} // End namespace Cdmw

