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


#ifndef INCL_ORBSUPPORT_CORBA_HPP
#define INCL_ORBSUPPORT_CORBA_HPP

#if CDMW_ORB_VDR == orbacus

#include <OB/CORBA.h>
// These are needed because of the USE of STLport
#define HAS_CORBA_EXCEPTION_OSTREAM_OPERATOR_DECLARATION 1
#define HAS_CORBA_EXCEPTION_OSTREAM_OPERATOR_DEFINITION 0


#elif CDMW_ORB_VDR == tao

#if CDMW_ORB_VER > 13
#define HAS_CORBA_EXCEPTION_OSTREAM_OPERATOR_DECLARATION 1
#define HAS_CORBA_EXCEPTION_OSTREAM_OPERATOR_DEFINITION 1
#endif

#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#include <tao/PortableServer/Servant_Base.h>

#if CDMW_ORB_VER < 14
/**
 * ValueType factories in TAO are not in the CORBA namespace but
 * are rather prefixed by CORBA_.
 * The following definitions will enable writing portable C++ code across
 * several ORBs.
 */
namespace CORBA
{
    typedef CORBA_ValueFactoryBase     ValueFactoryBase;
    typedef CORBA_ValueFactoryBase_var ValueFactoryBase_var;
    typedef CORBA_ValueFactory         ValueFactory;
};
#endif

#else

#error "Cdmw: ORB not supported"

#endif



#endif // INCL_ORBSUPPORT_CORBA_HPP


