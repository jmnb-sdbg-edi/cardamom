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

#ifndef INCL_LIFECYCLE_IN_PROGRESS_TESTDATA_HPP
#define INCL_LIFECYCLE_IN_PROGRESS_TESTDATA_HPP


namespace Cdmw 
{
namespace LifeCycle
{


extern const char* get_ENTITY_FACTORY_NAME() throw();
extern const char* get_UNMANAGED_ENTITY_FACTORY_NAME() throw();
extern const char* get_SESSION_FACTORY_NAME() throw();
extern const char* get_SERVICE_FACTORY_NAME() throw();

extern const char* get_RESOURCES_DOMAIN_NAME() throw();
extern const char* get_FACTORIES_DOMAIN_NAME() throw();

extern const char* get_ENTITY_DEVICE_KEY() throw();
extern const char* get_ENTITY_DEVICE_LABEL() throw();
extern const char* get_ENTITY_APPLICATION_I_KEY() throw();
extern const char* get_ENTITY_APPLICATION_II_KEY() throw();

extern const char* get_UNMANAGED_ENTITY_DEVICE_KEY() throw();
extern const char* get_UNMANAGED_ENTITY_DEVICE_LABEL() throw();
extern const char* get_UNMANAGED_ENTITY_APPLICATION_I_KEY() throw();
extern const char* get_UNMANAGED_ENTITY_APPLICATION_II_KEY() throw();

extern const char* get_SESSION_DEVICE_LABEL() throw();

extern const char* get_SERVICE_DEVICE_LABEL() throw();

extern const char* get_PROGRAM_EXECUTABLE_1() throw();
extern const char* get_PROGRAM_EXECUTABLE_2() throw();


}; // End namespace LifeCycle

}; // End namespace Cdmw 

#endif // INCL_LIFECYCLE_IN_PROGRESS_TESTDATA_HPP

