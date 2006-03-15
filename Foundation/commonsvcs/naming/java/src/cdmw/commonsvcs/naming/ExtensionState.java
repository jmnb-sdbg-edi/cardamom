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
 * This class provides an implementation of the abstract definitions
 * of the State class methods.
 *
 */
public class ExtensionState extends State {

    /**
     * Default name of the state.
     */
    private final String name = "Extension";

    /**
     * When a common character has been read and the context of the parser
     * is in this state, this method is executed.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public State other(NameParserContext npc) throws InvalidNameException {
        npc.putCharInKind();
        return NameParserFSM.EXTENSION_STATE;
    }

    /**
     * When a end-of-file character has been read and
     * the context of the parser is in this state, this method is executed.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public State eos(NameParserContext npc) throws InvalidNameException {
        npc.addLastKind();
        return NameParserFSM.EXTENSION_STATE;
    }

    /**
     * When a back-slash character has been read and
     * the context of the parser is in this state, this method is executed.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public State backSlash(NameParserContext npc) throws InvalidNameException {
        return NameParserFSM.ESC_EXTENSION_STATE;
    }

    /**
     * When a slash character has been read and
     * the context of the parser is in this state, this method is executed.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public State slash(NameParserContext npc) throws InvalidNameException {
        npc.addNameComponent();
        return NameParserFSM.BASENAME_STATE;
    }

    /**
     * When a dot character has been read and the context of the parser
     * is in this state, this method is executed.
     *
     * @param npc The context of the parser.
     * @return The new state of the context.
     * @exception InvalidNameException
     */
    public State dot(NameParserContext npc) throws InvalidNameException {
        return NameParserFSM.EXTENSION_STATE;
    }

}

