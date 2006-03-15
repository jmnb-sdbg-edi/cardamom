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


package com.thalesgroup.tools.ant.util;


import java.util.Collection;
import java.util.StringTokenizer;
import java.util.TreeSet;


/**
 * Set of helper methods.
 */
public class StringUtils {


    /**
     * Normalize the spaces in the specified string.
     *
     * @param str The string to be normalized.
     */
    public static String normalizeSpaces(String str) {
        StringBuffer newStr = new StringBuffer();

        StringTokenizer st = new StringTokenizer(str.trim());
        while (st.hasMoreTokens()) {
            newStr.append(st.nextToken());
            newStr.append(" ");
        }

        return newStr.toString().trim();
    }


    /**
     * Map a range like "1;3;5-12;4" into a list of identifiers.
     *
     * @param range The range.
     * @param ids   The list of identifiers to use for the mapping.
     *
     * @return The sorted list of identifiers that corresponds to the range.
     */
    public static Collection mapRangeIntoNames(String range,
                                               Object[] ids) {

        TreeSet list = new TreeSet();

        // Split into tokens. An input can be "1; 3;5-12 ;4".
        // Some token may be a single number, some may be a range of numbers.
        StringTokenizer st = new StringTokenizer(range, ";");
        while (st.hasMoreTokens()) {
            String token = st.nextToken().trim();

            if (token.indexOf("-") > -1) {
                // Range of numbers, extract the min and the max values.
                String[] bounds = token.split("-");

                int min = new Integer(bounds[0]).intValue() - 1;
                int max = new Integer(bounds[1]).intValue() - 1;

                for (int i = min; i <= max; i++) {
                    String id = (String) ids[i];
                    if (!list.contains(id)) {
                        list.add(id);
                    }
                }
            } else {
                // Single number.
                int index = new Integer(token).intValue() - 1;

                String id = (String) ids[index];
                if (!list.contains(id)) {
                    list.add(id);
                }
            }
        }

        return list;
    }


}

