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


package cdmw.ccm.common;

import cdmw.ossupport.OS;
import cdmw.ossupport.Timeval;

/**
 * Implementation of the org.omg.Components.Cookie valuetype.
 * It redefines the equals() method and the hashCode() method,
 * allowing use of Cookies in a Map.
 */
public class CookieImpl extends org.omg.Components.Cookie 
    implements Comparable {

    /**
     * Default Constructor.
     * cookieValue is set to null.
     */
    public CookieImpl() {
        this.cookieValue = null;
    }

    /**
     * Constructor.
     *
     * @param cookieValue the value to be set for the new Cookie
     */
    public CookieImpl(byte[] cookieValue) {
        this.cookieValue = cookieValue;
    }
    
    /**
     * Constructor.
     *
     * @param cookieValue the value to be set for the new Cookie
     */
    public CookieImpl(String cookieValue) {
        this.cookieValue = cookieValue.getBytes();
    }
    
    /**
     * Returns the cookie value as a String object.
     * 
     * @return The cookie value.
     */
    public String getCookieString() {
        return new String(this.cookieValue);
    }

    /**
     * Compares this object to the specified object. 
     * The result is true if and only if the argument is not null 
     * and is a CookieImpl object that contains the same cookieValue 
     * as this object.
     *
     * @param the object to compare with.
     *
     * @return true if the objects are the same; false otherwise.
     */
    public boolean equals(Object obj) {

        // if not the same type, not equals
        if (!(obj instanceof CookieImpl)) {
            return false;
        }

        // get obj.cookieValue
        byte[] b = ((CookieImpl) obj).cookieValue;

        // if cookieValues have different size, not equals
        if (b.length != cookieValue.length) {
            return false;
        }

        // if a byte differs, not equals
        for (int i = 0; i < b.length; i++) {
            if (b[i] != cookieValue[i]) {
                return false;
            }
        }

        // they are equals
        return true;
    }

    /**
     * Returns a hashcode for this CookieImpl.
     *
     * @return a hash code value for this object.
     */
    public int hashCode() {
        int hash = 0;

        // for each byte of the cookieValue
        for (int i = 0;(i < cookieValue.length); i++) {

            // create a corresponding mask (a byte has 8 bits)
            // (shift is made with modulo sizeof(int)) 
            int mask = cookieValue[i] << (8 * i);

            // apply mask to hash code with xor
            // (xor reduce probability to have only 1 bits 
            // with large cookieValues)
            hash = hash ^ mask;
        }

        return hash;
    }
    
    /**
     * Compares this object with the specified object for order.
     * 
     * @param o the Object to be compared.
     * @return a negative integer, zero, or a positive integer as this object 
     * is less than, equal to, or greater than the specified object.
     * @throws ClassCastException if the specified object's type prevents it 
     * from being compared to this Object
     */
    public int compareTo(Object o) {
        CookieImpl other = (CookieImpl) o;
        return getCookieString().compareTo(other.getCookieString());
    }

    /**
     * Static method to create a Cookie.
     * 
     * @return CookieImpl
     * @throws ExceededConnectionLimit
     */
    public static org.omg.Components.Cookie createCookie()
        throws org.omg.Components.ExceededConnectionLimit {

        try {
            int counter = CCMUtils.createIdentifier();
            Timeval tv = OS.getTime();
            String ostr = new String(tv.getSeconds() + ":" + 
                tv.getMicroseconds() + ":" + counter);
            return new CookieImpl(ostr);
        } catch (Exception e) {
            throw new org.omg.Components.ExceededConnectionLimit();
        }

    }

}

