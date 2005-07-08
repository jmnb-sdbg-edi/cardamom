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

public class CCMUtils {

    public final static int MAX_COUNTER = Integer.MAX_VALUE;

    private static int counter = 0;

    public synchronized static int createIdentifier() {
        if (counter == MAX_COUNTER) {
            counter = 0;
        } else {
            counter++;
        }
        return counter;
    }

    public static String createUniqueIdentifier(
        String prefix, int fillWidth) {
        int counter = createIdentifier();
        String formattedCounter = String.valueOf(counter);
        int lg = formattedCounter.length();
        if (lg > fillWidth) {
            formattedCounter = formattedCounter.substring(lg - fillWidth, lg);
        } else {
            int diff = fillWidth - lg;
            StringBuffer countBuffer = new StringBuffer();
            while (diff-- > 0) countBuffer.append('0');
            countBuffer.append(formattedCounter);
            formattedCounter = countBuffer.toString();
        }
        return prefix + "_" + formattedCounter;
    }

}

