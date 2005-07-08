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


import java.io.File;

import java.util.Collection;
import java.util.Vector;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

import org.apache.tools.ant.Project;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.types.FileSet;


/**
 * Set of helper methods.
 */
public class FindUtils {


    /**
     * Find files recursively from a start directory but returns only the
     * first match.
     *
     * @param project   The ANT project.
     * @param pattern   The file pattern.
     * @param dir       The start directory.
     * @param recursive If <code>true</code> then search recursively from
     *                  from the start directory.
     */
    public static File findFile(Project project,
                                String pattern,
                                File dir,
                                boolean recursive) {

        Vector v = new Vector(findFiles(project, pattern, dir, recursive));
        File f = null;

        try {
            f = (File) v.firstElement();
        } catch (NoSuchElementException nsee) {
            // Ignore.
        }

        return f;
    }


    /**
     * Find files recursively from a start directory and return all the
     * matches in a Collection.
     *
     * @param project   The ANT project.
     * @param pattern   The file pattern.
     * @param dir       The start directory.
     * @param recursive If <code>true</code> then search recursively from
     *                  from the start directory.
     */
    public static Collection findFiles(Project project,
                                       String pattern,
                                       File dir,
                                       boolean recursive) {

        Vector v = new Vector();

        if (dir.exists() && dir.isDirectory()) {
            FileSet fileSet = new FileSet();
            fileSet.setProject(project);
            fileSet.setDir(dir);
            /*
            fileSet.setIncludes(
                (recursive ? "**" + File.separator : "") + pattern);
            */
            StringTokenizer st = new StringTokenizer(pattern, " ");
            StringBuffer sb = new StringBuffer();
            while (st.hasMoreTokens()) {
                sb.append((recursive ? "**" + File.separator : "")
                            + st.nextToken()).append(" ");
            }
            fileSet.setIncludes(sb.toString().trim());

            DirectoryScanner ds = fileSet.getDirectoryScanner(project);
            String[] files = ds.getIncludedFiles();

            for (int i = 0; i < files.length; i++) {
                File f = new File(dir, files[i]);
                v.add(f);
            }
        }

        return v;
    }


}

