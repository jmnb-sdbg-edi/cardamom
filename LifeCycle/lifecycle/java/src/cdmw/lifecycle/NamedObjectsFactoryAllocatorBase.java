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


package cdmw.lifecycle;

import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import com.thalesgroup.CdmwNamingAndRepository.NameDomain;

/**
 * This interface provides basic methods for all CDMW
 * Named Objects Factories.
 *
 */
public interface NamedObjectsFactoryAllocatorBase
    extends FactoryAllocatorBase {

    /**
     * Allocates a named objects factory for use by the LifeCycle framework.
     * This is ideal place to retreive the factory state
     * from persistence support.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param parent Parent POA for the factory
     * @param nameDomain Object reference of the name domain where the
     * factory will register objects it creates
     * @param nameDomainName The fully qualified name of the NameDomain
     * @param factoryName Fully qualified name of the CDMW factory
     * @param threadingPolicy Threading issues
     * @param args Program arguments
     *
     * @return The allocated CDMW factory
     *
     * @exception BadParameterException Wrong parameter value, such as
     * threading ... etc.
     * @exception InternalErrorException Internal Error
     */
    public abstract org.omg.PortableServer.Servant allocate(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        NameDomain nameDomain,
        String nameDomainName,
        String factoryName,
        POAThreadingPolicy threadingPolicy,
        String[] args)
        throws BadParameterException,
               InternalErrorException;

}

