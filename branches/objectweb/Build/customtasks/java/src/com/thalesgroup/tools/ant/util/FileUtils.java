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


package com.thalesgroup.tools.ant.util;


import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import java.util.Iterator;
import java.util.Vector;

import org.apache.tools.ant.types.FilterSet;
import org.apache.tools.ant.types.FilterSetCollection;


/**
 * Set of helper methods.
 */
public class FileUtils {


    /**
     * Load the content of a file into a string. The content of the file may
     * be altered by the provided filtersets.
     *
     * @param f          The file to be read.
     * @param filtersets The list of filtersets.
     *
     * @return The content of the file with expanded tokens (if any).
     */
    public static String loadFile(File f, Vector filtersets) {
        if ((filtersets != null) && !filtersets.isEmpty() &&
            (filtersets.firstElement() instanceof FilterSet)) {

            FilterSetCollection executionFilters = new FilterSetCollection();

            Iterator iter = filtersets.iterator();
            while (iter.hasNext()) {
                executionFilters.addFilterSet((FilterSet) iter.next());
            }

            return loadFile(f, executionFilters);
        }

        return "";
    }


    /**
     * Load the content of a file into a string. The content of the file may
     * be altered by the provided filtersets.
     *
     * @param f                The file to be read.
     * @param executionFilters The filtersets to be applied.
     *
     * @return The content of the file with expanded tokens (if any).
     */
    public static String loadFile(File f,
                                  FilterSetCollection executionFilters) {

        String LS = System.getProperty("line.separator");

        StringBuffer text = new StringBuffer();
        FileReader reader_ = null;

        try {
            reader_ = new FileReader(f);
            BufferedReader reader = new BufferedReader(reader_);

            String newline = null;
            String line = reader.readLine();

            while (line != null) {
                if (line.length() > 0) {
                    newline = (executionFilters != null)
                        ? executionFilters.replaceTokens(line)
                        : line;

                    text.append(newline);
                }
                text.append(LS);
                
                line = reader.readLine();
            }
        } catch (IOException ioe) {
            ioe.printStackTrace();
        } finally {
            try {
                reader_.close();
            } catch (IOException ioe) {
                // Ignore.
            }
        }

        return text.toString();
    }


    /**
     * Try to detect Unix/Linux symbolic links.
     *
     * @param f The file.
     *
     * @return <code>true</code> or <code>false</code>
     */
    public static boolean isLink(File f)
        throws IOException {

        if (f.exists()) {
            String cnnpath = f.getCanonicalPath();
	        String abspath = f.getAbsolutePath();

	        return !abspath.equals(cnnpath);
        } else {
            String msg = f.getName() + " (No such file or directory)";
            throw new java.io.FileNotFoundException(msg);
        }
    }


}

