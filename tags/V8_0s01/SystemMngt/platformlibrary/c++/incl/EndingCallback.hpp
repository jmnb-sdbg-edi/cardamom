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


#ifndef INCL_PLATFORMMNGT_ENDINGCALLBACK_HPP
#define INCL_PLATFORMMNGT_ENDINGCALLBACK_HPP

#include "Foundation/ossupport/ProcessCallback.hpp"
#include "Foundation/ossupport/OS.hpp"

namespace Cdmw
{

namespace PlatformMngt
{

/**
* Purpose:
* <p> 
* The callback that may be attached to the ending of a process and 
* that manages reference counter.
*/

class EndingCallback : virtual public OsSupport::ProcessCallback
{

public:

    /**
    * Purpose:
    * <p> Constructor
    */
    EndingCallback();


    /**
    * Purpose:
    * <p> Destructor
    */
    virtual ~EndingCallback()
       throw();


    /**
    * Purpose:
    * <p> Executes the callback.
    */
    virtual void execute(OsSupport::OS::ProcessId processId) throw() = 0;


    /**
    * Purpose:
    * <p> Increases the callback counter.
    */
    void increaseCounter();


    /**
    * Purpose:
    * <p> Decreases the callback counter.
    *
    * @return true if deletable (counter=0 and delete flag set)
    */
    bool decreaseCounter();


    /**
    * Purpose:
    * <p> Checks if the callback must be deleted.
    */
    bool isDeletable();


    /**
    * Purpose:
    * <p> Checks if the callback can be called.
    */
    bool isCallable();


    /**
    * Purpose:
    * <p> Indicates that the callback has to be deleted when the counter is null.
    */
    void setDeleteCallback();

private:
    int m_counter;
    bool m_delete;

    /**
    * The mutex protecting concurrent access to m_counter and m_delete.
    */
    OsSupport::Mutex m_mutex;

}

; // End class EndingCallback

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_ENDINGCALLBACK_HPP


