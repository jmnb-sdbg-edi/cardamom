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


package cdmw.ccm.container;

import cdmw.common.BadParameterException;

import cdmw.lifecycle.FactoryAllocatorBase;
import cdmw.lifecycle.ObjectDeactivator;

/**
 * Base interface for all Component homes allocators.
 *
 */
public interface HomeAllocatorBase extends FactoryAllocatorBase {

    /**
     * This operation is called by the container during homes installation
     * to allocate a servant for the home.
     *
     * @param homeExecutor Home executor acquired from the loaded library.
     * @param deactivator Object Deactivator for use
     * for home object deactivation.
     * @return The servant allocated
     * @exception BadParameterException Wrong parameter value
     */
    public org.omg.PortableServer.Servant allocate(
        org.omg.Components.HomeExecutorBase homeExecutor,
        ObjectDeactivator deactivator)
        throws BadParameterException;
    
    /**
     * Returns the delegate of the servant in case of a Tie implementation.
     *
     * @param servant Servant that represents the Tie implementation
     * @return The delegate.
     * @exception BadParameterException Wrong parameter value
     */
    public org.omg.PortableServer.Servant getDelegate(
        org.omg.PortableServer.Servant servant) throws BadParameterException;

}

