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
 * This is the class used to parse the string given
 * in the NameParserContext object. Parsing is made by a set of
 * transitions between four states corresponding to four classes:<UL>
 * <LI> BasenameState,</LI>
 * <LI> EscBasenameState,</LI>
 * <LI> ExtensionState,</LI>
 * <LI> and EscExtensionState.</LI></UL>
 *
 * @see State
 * @see BasenameState
 * @see EscBasenameState
 * @see ExtensionState
 * @see EscExtensionState
 */
public class NameParserFSM {

    /**
     * Basename state instance.
     */
    public final static BasenameState BASENAME_STATE
        = new BasenameState();

    /**
     * EscBasenameState state instance.
     */
    public final static EscBasenameState ESC_BASENAME_STATE
        = new EscBasenameState();

    /**
     * ExtensionState state instance.
     */
    public final static ExtensionState EXTENSION_STATE
        = new ExtensionState();

    /**
     * EscExtensionState state instance.
     */
    public final static EscExtensionState ESC_EXTENSION_STATE
        = new EscExtensionState();

    /**
     * Current state of the parser, initialized with a basename state.
     */
    private State state = new BasenameState();

    /**
     * Context of the parser.
     */
    private NameParserContext context;

    /**
     * Constructor
     *
     * @param s String to parse, needed to create the context.
     */
    public NameParserFSM(String s) {
        this.context = new NameParserContext(s);
    }

    /**
     * Read the next character of the string.
     *
     * @return The next character.
     */
    public char readChar() {
        return context.readChar();
    }

    /**
     * This operation returns a NameComponent array that corresponds to
     * the stringified string passed to the constructor
     *
     * @return A NameComponent array that holds the decoded string.
     */
    public org.omg.CosNaming.NameComponent[] getName() {
        return context.getName();
    }

    /**
     * Change the state of the context in case
     * of a common character transition.
     *
     * @exception InvalidNameException
     */
    public void other() throws InvalidNameException {
        setState(state.other(context));
    }

    /**
     * Change the state of the context in case
     * of a end-of-file character transition.
     *
     * @exception InvalidNameException
     */
    public void eos() throws InvalidNameException {
        setState(state.eos(context));
    }

    /**
     * Change the state of the context in case
     * of a back-slash character transition.
     *
     * @exception InvalidNameException
     */
    public void backSlash() throws InvalidNameException {
        setState(state.backSlash(context));
    }

    /**
     * Change the state of the context in case
     * of a slash character transition.
     *
     * @exception InvalidNameException
     */
    public void slash() throws InvalidNameException {
        setState(state.slash(context));
    }

    /**
     * Change the state of the context in case
     * of a dot character transition.
     *
     * @exception InvalidNameException
     */
    public void dot() throws InvalidNameException {
        setState(state.dot(context));
    }

    /**
     * Get the current state.
     *
     * @return The current state.
     */
    public State getState() {
        return this.state;
    }

    /**
     * Set the current state.
     *
     * @param s The new state to be set.
     */
    public void setState(State s) {
        this.state = s;
    }

}

