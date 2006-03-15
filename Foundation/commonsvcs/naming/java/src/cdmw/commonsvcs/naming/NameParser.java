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

import org.omg.CosNaming.NameComponent;

import cdmw.common.Assert;
import cdmw.common.AssertionFailedException;
import cdmw.orbsupport.ExceptionMinorCodes;

/**
 * This class provides two static methods to convert
 * a NameComponent array to a stringified name and conversely
 *
 * @see NameParserContext
 * @see NameParserFSM
 * @see State
 */
public class NameParser {

    /**
     * Add INS stringified-name escapes.
     * Characters '.', '/' and '\\' must be escaped with a '\\'.
     *
     * @param s initial string
     * @return the initial string with stringified-name escapes
     */
    private static String escapeField(String s) {

        if ( s == null ) return "";

        char[] string = s.toCharArray();
        String escaped = new String();
        for(int i=0; i<string.length; i++) {
            switch(string[i]) {
            case '/':
            case '.':
            case '\\':
                escaped += String.valueOf('\\');
            default:
                escaped += String.valueOf(string[i]);
            }
        }
        return escaped;

    }

    /**
     * INS compatible stringifying of a NameComponent object.
     *
     * The single '.' character is the only representation of a name
     * component with empty id and kind fields.
     * If a name component has a non-empty id field and an empty kind field,
     * the stringified representation consists only of the id field.
     * A trailing '.' character is not permitted.
     *
     * @param nc the NameComponent object to be converted into a string
     * @return INS compatible stringifying of the NameComponent object
     */
    private static String toString(NameComponent nc) {

        String s = escapeField(nc.id);
        if (!nc.kind.equals("") || nc.id.equals("")) {
            s += ".";
            s += escapeField(nc.kind);
        }
        return s;

    }



    /**
     * This operation accepts a stringified name
     *    and returns a NameComponent array.
     *
     * @param sn A stringified name, INS compatible, to be converted
     * to a NameComponent array.
     *
     * @return A NameComponent array that holds the decoded string.
     *
     * @exception InvalidNameException Indicates that the input string is
     * syntactically malformed.
     */
    public static NameComponent[] toName(String sn)
        throws InvalidNameException {

        NameParserFSM parser = new NameParserFSM(sn);
        boolean finished = false;

        while(!finished) {

            try {

                switch(parser.readChar()) {

                    case '/':
                        parser.slash();
                        break;
                    case '.':
                        parser.dot();
                        break;
                    case '\\':
                        parser.backSlash();
                        break;
                    default:
                        parser.other();
                        break;
                }

            } catch(IndexOutOfBoundsException e) {
                parser.eos();
                finished = true;
            }

        }
        return parser.getName();

    }


    /**
     * This operation accepts a NameComponent array
     *    and returns a stringified name.
     *
     * @param name A NameComponent array to stringify.
     *
     * @return A stringified name, INS compatible,
     *    of the input NameComponent array.
     *
     * @exception InvalidNameException Indicates
     *    that the name is invalid (length 0)
     */
    public static String toString(NameComponent[] n)
        throws InvalidNameException {

        if (n==null || n.length == 0) {
            throw new InvalidNameException("The empty sequence is not a legal name");
        }

        String s = new String();
        for( int i = 0; i < n.length; i++ ) {

            if(i!=0) {
                s += "/";
            }
            s += toString(n[i]);

        }
        return s;

    }

}

