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


package cdmw.commonsvcs.naming;

/**
 * This class provides abstract definitions of methods to be implemented
 * to associate to every transition of a state in another one
 * a corresponding action.
 *
 */
public abstract class State {

    /**
     * Method to be implemented when a common character has been read.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public abstract State other(NameParserContext npc)
        throws InvalidNameException;

    /**
     * Method to be implemented when a end-of-file character has been read.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public abstract State eos(NameParserContext npc)
        throws InvalidNameException;

    /**
     * Method to be implemented when a back slash character has been read.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public abstract State backSlash(NameParserContext npc)
        throws InvalidNameException;

    /**
     * Method to be implemented when a slash character has been read.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public abstract State slash(NameParserContext npc)
        throws InvalidNameException;

    /**
     * Method to be implemented when a dot character has been read.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public abstract State dot(NameParserContext npc)
        throws InvalidNameException;

}

