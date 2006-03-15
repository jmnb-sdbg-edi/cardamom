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


package com.thalesgroup.tools.ant.taskdefs;


import java.io.File;

import java.util.StringTokenizer;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.util.FileUtils;
import org.apache.tools.ant.types.FileSet;


/**
 * Build an ANT FileSet from a list of file names.
 */
public class ListToFileSet extends Task {


    // The FileSet id to be created.
    private String m_filesetId = null;

    // The list of file names.
    private String m_list = null;

    // The base directory of the FileSet.
    private File m_dir = null;

    private static FileUtils fileUtils = FileUtils.newFileUtils();


    /**
     * Specify the FileSet id.
     *
     * @param filesetId The FileSet id.
     */
    public void setFilesetId(String filesetId) {
        m_filesetId = filesetId;
    }


    /**
     * Get the FileSet id.
     *
     * @return The FileSet id.
     */
    public String getFilesetId() {
        return m_filesetId;
    }


    /**
     * Specify the list of file names.
     *
     * @param list The list of file names.
     */
    public void setList(String list) {
        m_list = list;
    }


    /**
     * Get the list of file names.
     *
     * @return The list of file names.
     */
    public String getList() {
        return m_list;
    }


    /**
     * Specify the base directory for the FileSet.
     *
     * @param dir The base directory for the FileSet.
     */
    public void setDir(File dir) {
        m_dir = dir;
    }


    /**
     * Get the base directory of the FileSet.
     *
     * @return The base directory of the FileSet.
     */
    public File getDir() {
        return m_dir;
    }


    /**
     * Default constructor.
     */
    public ListToFileSet() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        FileSet fileSet = new FileSet();
        fileSet.setProject(getProject());
        fileSet.setDir(m_dir);

        String dirNormal =
            fileUtils.normalize(m_dir.getAbsolutePath()).getAbsolutePath()
            + File.separator;
        
        boolean atLeastOne = false;

        StringTokenizer st = new StringTokenizer(m_list);
        while (st.hasMoreTokens()) {
            String relativeName = st.nextToken();

            File sourceFile = new File(dirNormal + relativeName);

            // Discard file that does not exist.
            if (!sourceFile.exists())
                continue;

            fileSet.createInclude().setName(relativeName);
            atLeastOne = true;
        }

        if (!atLeastOne) {
            // Need to make an empty fileset.
            fileSet.createInclude().setName(
                "a:b:c:d//This is &&& not a file !!!");
        }

        getProject().addReference(m_filesetId, fileSet);
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_filesetId == null) || m_filesetId.equals("")) {
            throw new BuildException("attribute filsetId must not be null.");
        }

        if ((m_list == null) || m_list.equals("")) {
            throw new BuildException("attribute list must not be null.");
        }

        if (m_dir == null) {
            throw new BuildException("attribute dir must not be null.");
        }

        if (!m_dir.exists()) {
            throw new BuildException("dir does not exist.");
        }

        if (!m_dir.isDirectory()) {
            throw new BuildException("dir is not a directory.");
        }
    }


}

