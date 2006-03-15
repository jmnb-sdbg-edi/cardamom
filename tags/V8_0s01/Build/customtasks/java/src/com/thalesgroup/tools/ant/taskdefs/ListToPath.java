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
import org.apache.tools.ant.types.Path;


/**
 * Build an ANT Path from a list of file names.
 */
public class ListToPath extends Task {


    // The Path id to be created.
    private String m_pathId = null;

    // The list of file names.
    private String m_list = null;


    /**
     * Specify the Path id.
     *
     * @param pathId The Path id.
     */
    public void setPathId(String pathId) {
        m_pathId = pathId;
    }


    /**
     * Get the Path id.
     *
     * @return The Path id.
     */
    public String getPathId() {
        return m_pathId;
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
     * Default constructor.
     */
    public ListToPath() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        Path path = new Path(getProject());
        StringTokenizer st = new StringTokenizer(m_list);
        while (st.hasMoreTokens()) {
            path.setLocation(new File(st.nextToken()));
        }

        getProject().addReference(m_pathId, path);
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_pathId == null) || m_pathId.equals("")) {
            throw new BuildException("attribute pathId must not be null.");
        }

        if ((m_list == null) || m_list.equals("")) {
            throw new BuildException("attribute list must not be null.");
        }
    }


}

