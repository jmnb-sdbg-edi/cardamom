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

#include "lifecycleclient/TestData.hpp"

namespace
{
    const char* ENTITY_FACTORY_NAME             = "Entity Resource.factory";
    const char* UNMANAGED_ENTITY_FACTORY_NAME   = "Unmanaged Entity Resource.factory";
    const char* SESSION_FACTORY_NAME            = "Session Resource.factory";
    const char* SERVICE_FACTORY_NAME            = "Service Resource.factory";

    const char* RESOURCES_DOMAIN_NAME           = "System.resources";
    const char* FACTORIES_DOMAIN_NAME           = "Resource.Factories";

    const char* ENTITY_DEVICE_KEY               = "USB device.dev";
    const char* ENTITY_DEVICE_LABEL             = "K1 203";
    const char* ENTITY_APPLICATION_I_KEY        = "XLOGO.appl";
    const char* ENTITY_APPLICATION_II_KEY       = "XLOGO\\.2.appl";

    const char* UNMANAGED_ENTITY_DEVICE_KEY         = "unmanaged - USB device.dev";
    const char* UNMANAGED_ENTITY_DEVICE_LABEL       = "unmanaged - K1 203";
    const char* UNMANAGED_ENTITY_APPLICATION_I_KEY  = "unmanaged - XLOGO.appl";
    const char* UNMANAGED_ENTITY_APPLICATION_II_KEY = "unmanaged - XLOGO\\.2.appl";

    const char* SESSION_DEVICE_LABEL             = "K1 203";

    const char* SERVICE_DEVICE_LABEL             = "K1 203";

#ifdef _WIN32
    const char* PROGRAM_EXECUTABLE_1 = "notepad.exe";
    const char* PROGRAM_EXECUTABLE_2 = "welcome.exe";
#else
    const char* PROGRAM_EXECUTABLE_1 = "xlogo";
    const char* PROGRAM_EXECUTABLE_2 = "xclock";
#endif

}; // End Anonymous namespace

namespace Cdmw 
{
namespace LifeCycle
{

const char* get_ENTITY_FACTORY_NAME() throw()
{
    return ENTITY_FACTORY_NAME;
}

const char* get_UNMANAGED_ENTITY_FACTORY_NAME() throw()
{
    return UNMANAGED_ENTITY_FACTORY_NAME;
}

const char* get_SESSION_FACTORY_NAME() throw()
{
    return SESSION_FACTORY_NAME;
}
const char* get_SERVICE_FACTORY_NAME() throw()
{
    return SERVICE_FACTORY_NAME;
}
const char* get_RESOURCES_DOMAIN_NAME() throw()
{
    return RESOURCES_DOMAIN_NAME;
}
const char* get_FACTORIES_DOMAIN_NAME() throw()
{
    return FACTORIES_DOMAIN_NAME;
}

const char* get_ENTITY_DEVICE_KEY() throw()
{
    return ENTITY_DEVICE_KEY;
}
const char* get_ENTITY_DEVICE_LABEL() throw()
{
    return ENTITY_DEVICE_LABEL;
}
const char* get_ENTITY_APPLICATION_I_KEY() throw()
{
    return ENTITY_APPLICATION_I_KEY;
}
const char* get_ENTITY_APPLICATION_II_KEY() throw()
{
    return ENTITY_APPLICATION_II_KEY;
}

const char* get_UNMANAGED_ENTITY_DEVICE_KEY() throw()
{
    return UNMANAGED_ENTITY_DEVICE_KEY;
}
const char* get_UNMANAGED_ENTITY_DEVICE_LABEL() throw()
{
    return UNMANAGED_ENTITY_DEVICE_LABEL;
}
const char* get_UNMANAGED_ENTITY_APPLICATION_I_KEY() throw()
{
    return UNMANAGED_ENTITY_APPLICATION_I_KEY;
}
const char* get_UNMANAGED_ENTITY_APPLICATION_II_KEY() throw()
{
    return UNMANAGED_ENTITY_APPLICATION_II_KEY;
}

const char* get_SESSION_DEVICE_LABEL() throw()
{
    return SESSION_DEVICE_LABEL;
}

const char* get_SERVICE_DEVICE_LABEL() throw()
{
    return SERVICE_DEVICE_LABEL;
}

const char* get_PROGRAM_EXECUTABLE_1() throw()
{
    return PROGRAM_EXECUTABLE_1;
}
const char* get_PROGRAM_EXECUTABLE_2() throw()
{
    return PROGRAM_EXECUTABLE_2;
}

}; // End namespace LifeCycle

}; // End namespace Cdmw 

