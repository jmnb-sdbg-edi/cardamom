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


package cdmw.namingandrepository;

import org.omg.CosNaming.NameComponent;

/**
 * This is the context class of the corba name parser.
 * This class knows about the input stream,
 * How to read and write chars, and which critical chars to
 * look for in the input stream.
 *
 */
public class NameParserContext {

    /**
     * last character read
     */
    private char c;
    /**
     * char array representation of the string to be parsed
     */
    private char[] inputData;
    /**
     * index of the current character to be read
     */
    private int inputDataIndex = 0;
    /**
     * current id value
     */
    private String id = new String();
    /**
     * current kind value
     */
    private String kind = new String();
    /**
     * Vector that contains all the NameComponent objects already parsed
     */
    private java.util.Vector name = new java.util.Vector();

    /**
     * Constructor
     *
     * @param s String to be parsed
     */
    public NameParserContext(String s) {

        this.inputData = s.toCharArray();

    }

    /**
     * Constructs an InvalidNameException with an explicit message
     *    in case of a transition error
     *
     * @param t Description of the transition
     * @param s Description of the state of the parser
     *
     * @exception InvalidNameException always thrown by the method
     */
    public void FSMError(String t, String s)
        throws InvalidNameException {

        String message = "Transition error: ";
        message += t + " in state " + s;
        throw new InvalidNameException(message);
        // TODO: use named constants

    }


    /**
     * Read the next character of the string to be parsed.
     *
     * @return The next character of the string.
     * @exception IndexOutOfBoundsException when there is no more character
     * to be read
     */
    public char readChar() throws IndexOutOfBoundsException {
        c = inputData[inputDataIndex];
        inputDataIndex++;
        return c;
    }

    /**
     * Return the NameComponent array corresponding to the characters
     * of the string already parsed
     *
     * @return the NameComponent array result of the parsing
     */
    public NameComponent[] getName() {

        NameComponent[] n = new NameComponent[name.size()];
        int i = 0;
        java.util.Enumeration e = name.elements();
        for ( ; e.hasMoreElements() ;) {
            n[i] = (NameComponent) e.nextElement();
            i++;
        }
         return n;

    }

    /**
     * Put the current character in the id string
     */
    public void putCharInId()  {
        id += String.valueOf(c);
    }

    /**
     * Put the current character in the kind string
     */
    public void putCharInKind() {
        kind += String.valueOf(c);
    }

    //
    // Actions
    //

    /**
     * Add the current id and kind values in the name Vector
     * as a NameComponent object
     */
    public void addNameComponent() {
        appendNameComponent(id,kind);
    }

    /**
     * Add the current id value (with an empty kind) in the name Vector
     * as a NameComponent object
     */
    public void addLastId() {
        appendNameComponent(id,"");
    }

    /**
     * Add the current id and kind values in the name Vector
     * as a NameComponent object
     * A trailing '.' character is not permitted.
     *
     * @exception InvalidNameException when the kind value is empty and
     * the id value not
     */
    public void addLastKind() throws InvalidNameException {

        // If a name component has a non-empty id field and an empty kind field,
        // the stringified representation consists only of the id field.
        // A trailing '.' character is not permitted.
        if ( !id.equals("") && kind.equals("") ) {
            throw new InvalidNameException("Trailing '.' not permitted!");
        }
        appendNameComponent(id,kind);

    }

    /**
     * Checks if the id value is empty
     *
     * @exception InvalidNameException when the id value is empty
     */
    public void checkIdNotEmpty() throws InvalidNameException {
        // The single '.' character is the only representation of a name
        // component with empty id and kind fields.
        if (id.equals("")) {
            throw new InvalidNameException("Invalid empty name component :"
                + " '.' may be missing");
        }

    }

    /**
     * Throws an InvalidNameException in case of
     * an unexpected end of string
     *
     * @exception InvalidNameException exception always thrown by the method
     */
    public void unexpectedEos() throws InvalidNameException {
        throw new InvalidNameException("Unexpected end of string!");
    }

    /**
     * Add the id and kind values passed as arguments in the name Vector
     * Reset the current id and kind values
     *
     * @param String idValue the id value to be added
     * @param String kindValue the kind value to be added
     */
    private void appendNameComponent(String idValue,String kindValue) {

        name.add( new NameComponent(idValue,kindValue) );
        this.id = "";
        this.kind = "";

    }

}

