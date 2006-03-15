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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_COORDINATOR_BASE_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_COORDINATOR_BASE_HPP

#include <FaultTolerance/ftstatemanager/Transaction_Result.hpp>
#include <FaultTolerance/ftstatemanager/ValidationFailureException.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCohort.stub.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>

#include <ace/Future.h>


/**
*Purpose:
* TransactionEngineCoordinator template.
*<p>
*
*Features:
*<p> Thread safe. Exception safe.
*/
namespace Cdmw {
namespace FT {

class TransactionEngineCoordinatorBase
{

public:   
    /**
     * Purpose:
     * <p> Ask the cohorts to apply a change
     *
     *@param oid is the Object Id key
     */
    virtual 
    bool request(const CdmwFT::Transaction::StateChanges& change)
        throw (InternalErrorException, ValidationFailureException) = 0;


    /**
     * Purpose:
     * <p> delegate send_yes
     *
     *@param sender
     *@param id
     */
    virtual 
    void send_yes_i(const char * sender,
                    const CdmwFT::Transaction::TransactionIdentification & id)
        throw (InternalErrorException) = 0;
    
    /**
     * Purpose:
     * <p> delegate send_no
     *
     *@param sender
     *@param id
     */
    virtual 
    void send_no_i(const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw (InternalErrorException) = 0;

    /**
     * Purpose:
     * <p> delegate send_end
     *
     *@param sender
     *@param id
     */
    virtual 
    void send_end_i(const char * sender,
                    const CdmwFT::Transaction::TransactionIdentification & id) 
        throw (InternalErrorException) = 0;

    /**
     * Purpose:
     * <p> delegate set_transaction_status
     *
     *@param sender
     *@param id
     */
    virtual 
    void send_set_transaction_status_i(const char * sender,
                                       const CdmwFT::Transaction::TransactionIdentification & id,
                                       CdmwFT::Transaction::TransactionStatus status)
        throw (InternalErrorException) = 0;

    //    CdmwFT::Transaction::TransactionStatus status;

    
}; // End class TransactionEngineCoordinator



}  // End namespace FT
}  // End namespace Cdmw

#endif

