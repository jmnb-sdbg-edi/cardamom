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


package cdmw.validation;

import org.omg.CORBA.ORB;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;

import cdmw.lifecycle.POAThreadingPolicy;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

public class ProcessAgentFactoryImplAllocator
    extends cdmw.lifecycle.AnonymousObjectsFactoryAllocator
{

    public ProcessAgentFactoryImplAllocator() {
        super("cdmw.validation.ProcessAgentFactoryImpl");
    }
    
    public static final ProcessAgentFactoryImplAllocator theAllocator =
        new ProcessAgentFactoryImplAllocator();

        
    // redefinition of allocate operation
    public Servant allocate(ORB orb,
                            POA parent,
                            String factoryName,
                            POAThreadingPolicy threadingPolicy,
                            String[] args)
        throws BadParameterException,
               InternalErrorException
    {
        return new ProcessAgentFactoryImpl(orb, parent, factoryName, threadingPolicy);
    }    
        
        
}


