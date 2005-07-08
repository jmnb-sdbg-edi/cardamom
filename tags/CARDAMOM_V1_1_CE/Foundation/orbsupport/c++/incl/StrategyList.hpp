/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_ORBSUPPORT_STRATEGYLIST_HPP
#define INCL_ORBSUPPORT_STRATEGYLIST_HPP

#include <cstddef>

#include "Foundation/common/Exception.hpp"

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    This class defines is used to set ORB dependent
        *        capability in an ORB independent way.
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  OrbSupport 
        *
        */

        class StrategyList
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                * 
                */
                StrategyList()
                throw();

                /**
                * Purpose:
                * <p>  Copy constructor
                * 
                *@param rhs the Strategy to be copied
                *@exception
                */
                StrategyList(const StrategyList& rhs)
                throw();


                /**
                * Purpose:
                * <p> Destructor
                */
                virtual
                ~StrategyList()
                throw();


                /**
                * Purpose:
                * <p> Assignement operator
                * 
                *@param rhs the Strategy to be copied
                *
                *@return the Strategy itself
                */
                StrategyList&
                operator=(const StrategyList& rhs)
                throw();



                /**
                *  Purpose:
                *  <p> Swap the content of this StrategyList
                *        with rhs
                *
                * @param rhs the StrategyList to be swapped
                */
                void
                swap( StrategyList& rhs )
                throw();


                /**
                * Purpose:
                * <p> Set the ORB shall use multi-threading.
                *
                */
                void
                add_OrbThreaded()
                throw( BadParameterException );


                /**
                * Purpose:
                * <p> Set the POA shall use multi-threading. The threads
                *  will be created at the initialisation time. 'nbThreads'
                *  will be created.
                *
                *  This Strategy is incompatible with:
                *   - add_PoaThreadPerRequest
                *   - add_PoaThreadPerConnection
                * 
                *  This Strategy requires:
                *   - ORB_init() call with add_OrbThreaded Strategy
                * 
                *@param nbThreads the size of the thread pool
                *
                *@exception BadParameterException if the nbThreads is eaqual to zero
                *@exception BadParameterException if the Strategy is incompatible
                *           with a previous one
                *
                */
                void
                add_PoaThreadPool( std::size_t nbThreads )
                throw( BadParameterException );



                /**
                * Purpose:
                * <p> Set the POA shall use multi-threading: one thread per request
                * 
                *  This Strategy is incompatible with:
                *   - add_PoaThreadPerConnection
                *   - add_PoaThreadPool
                * 
                *  This Strategy requires:
                *   - ORB_init() call with add_OrbThreaded Strategy
                * 
                *
                */
                void
                add_PoaThreadPerRequest()
                throw( BadParameterException );

                /**
                * Purpose:
                * <p> Set the POA shall use multi-threading: one thread per request
                * 
                *  This Strategy is incompatible with:
                *   - add_PoaThreadPerRequest
                *   - add_PoaThreadPool
                * 
                *  This Strategy requires:
                *   - ORB_init() call with add_OrbThreaded Strategy
                * 
                *
                */
                void
                add_PoaThreadPerConnection()
                throw( BadParameterException );

                /**
                * Purpose:
                * <p> Set the multicast capability
                * 
                *  REM: This Strategy is not implemented with Orbacus
                *  In this case the exception won't be thrown here
                *  (StrategyList is Orb independant & we want to avoid #if CDMW_ORB_VDR)
                *  but in ORB_init of OrbOrbacusImpl
                */
                void
                add_multicast()
                throw( BadParameterException );



                /**
                * Purpose:
                * <p> Returns if the Multi-threaded strategy has been 
                *     choosen for the ORB
                * 
                *@return true if the strategy has been set, false elsewhere
                *
                */
                bool
                is_OrbThreaded() const
                throw() ;


                /**
                * Purpose:
                * <p> Returns true if the Thread Pool strategy has been 
                *     choosen for the POA, and its value
                * 
                *@param nbThreads if the Thread Pool strategy is set, this
                *        variable contains the number of thread to be used
                *
                *@return true if the strategy has been set, false elsewhere
                *
                */
                bool
                is_PoaThreadPool( std::size_t& nbThreads ) const
                throw();



                /**
                * Purpose:
                * <p> Returns true if the ThreadPerRequest strategy has been 
                *     choosen for the POA.
                * 
                *@return true if the strategy has been set, false elsewhere
                *
                */
                bool
                is_PoaThreadPerRequest() const
                throw();


                /**
                * Purpose:
                * <p> Returns true if the ThreadPerRequest strategy has been 
                *     choosen for the POA.
                * 
                *@return true if the strategy has been set, false elsewhere
                *
                */
                bool
                is_PoaThreadPerConnection() const
                throw();


                /**
                * Purpose:
                * <p> Returns true if we choose to have multicast
                * 
                *@return true if the strategy has been set, false elsewhere
                *
                */
                bool
                is_multicast() const
                throw();






            protected:

                /**
                * Purpose:
                * <p> Define the POA must use the specified port for
                *   managing incoming CORBA request. 
                * <p><STRONG>NOTA:</STRONG> <B>This should not be used by application 
                * developpers as it will be removed in the next release.</B>
                * 
                *@param port the port to be used ( 0 < port < 65535 )
                *
                */
                void
                add_PoaPortAssigned(std::size_t poaPort)
                throw( BadParameterException ) ;

                /**
                 * Purpose:
                 * <p> Returns true if the user has ask for a specific
                 *     port for one POA
                 * <p><STRONG>NOTA:</STRONG> <B>This should not be used by application 
                 * developpers as it will be removed in the next release.</B>
                 * 
                 *@return true if the strategy has been set, false elsewhere
                 *
                 */
                bool
                is_PoaPortAssigned( std::size_t& poaPort ) const
                throw();


            private:

                /**
                * If true, the user has asked for a multi-threaded ORB.
                */
                bool m_has_orbThreaded;


                /**
                * If true, the user has asked for a Threaded POA
                */
                bool m_has_poaThreadPerRequest;


                /**
                * If true, the user has asked for a Threaded POA
                */
                bool m_has_poaThreadPerConnection;


                /**
                * If true, the user has asked for a POA with Thread Pool
                */
                bool m_has_poaThreadPool;



                /**
                * If m_nbPoaThreads is true, this variable contains the
                * thread number to be used by the ORB.
                */
                std::size_t m_nbPoaThreads;


                /**
                * If true, the user has asked for multicast
                */
                bool m_has_multicast;


                /**
                * If true, the user has asked to use a specific port 
                * for the POA
                */
                bool m_has_poaPortAssigned;


                /**
                * Value of the port to be used for the POA, if 
                * m_has_poaPortAssigned has been set.
                */
                std::size_t m_poaPort;



        }

        ; // End class StrategyList

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_STRATEGYLIST_HPP

