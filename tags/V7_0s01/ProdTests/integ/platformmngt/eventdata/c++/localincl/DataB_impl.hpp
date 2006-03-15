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

#ifndef __DATATYPE_B_HPP__
#define __DATATYPE_B_HPP__

#include "eventdata/Data.skel.hpp"
#include "eventdata/DataA_impl.hpp"

class DataBFactory : public virtual DataB_init
{
private:

    /**
     * Purpose:
     * <p> Inherited method used for unmarshalling factory object.
     * 
     *@return The unmarshalled factory.
     */ 
    virtual CORBA::ValueBase* create_for_unmarshal();

public:

    /**
     * Purpose:
     * <p> Creates a DataB.
     * 
     *@param message The message of the event.
     *@param number an associated number.
     *
     *@return the newly created DataB object.
     */ 
    virtual DataB* create(
        const char* message,
        CORBA::ULong number );
};

class DataB_impl : public virtual OBV_DataB,
                   public virtual DataA_impl
{
protected:

    friend class DataBFactory;

    DataB_impl();
    DataB_impl( const char* message, CORBA::ULong theNumber );

public:

    /**
     * Inherited from CORBA::ValueBase
     */
    virtual CORBA::ValueBase* _copy_value();
};

#endif //__DATATYPE_B_HPP__

