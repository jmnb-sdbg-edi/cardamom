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


package cdmw.ossupport;

import java.io.File;
import java.util.Vector;

/**
 * This class provides a set of static methods to help in files manipulating.
 *
 */
public class FileUtilities {

    /**
     * Search a file given its name in a specified directory.
     * Wildcard characters are not processed.
     *
     * @param filename Name of the file to search
     * @param dir Directory to search in
     * @return The files found in a Vector object
     */
    public static Vector search(String filename, File in) {

        Vector result = new Vector();
        if (!in.isDirectory()) return result;

        File file = null;
        Vector foundFiles = null;
        File[] files = in.listFiles();
        for (int i=0; i<files.length; i++) {
            file = files[i];
            
            if (file.isDirectory()) {
                foundFiles = search(filename, file);
                for (int j=0; j<foundFiles.size(); j++) {
                    result.add((File) foundFiles.get(j));
                }
            } else {
                if (file.getName().equals(filename)) {
                    result.add(file);
                }
            }
            
        }
        return result;
    
    }

}

