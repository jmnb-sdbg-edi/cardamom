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


package com.thalesgroup.tools.ant.taskdefs;


import java.io.File;
import java.io.IOException;

import java.util.Iterator;
import java.util.StringTokenizer;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.Path;
import org.apache.tools.ant.util.StringUtils;


/**
 * Transform the output of the CPP preprocessing into a new ANT Path reference.
 */
public class DependencyToPath extends Task {


    // The output of the preprocessing.
    private String m_dependencies = null;

    // The Path id to create.
    private String m_pathId = null;


    /**
     * Specify the output of the preprocessing.
     *
     * @param dependencies The output of the preprocessing.
     */
    public void setDependencies(String dependencies) {
        m_dependencies = dependencies;
    }


    /**
     * Get the output of the preprocessing.
     *
     * @return The output of the preprocessing.
     */
    public String getDependencies() {
        return m_dependencies;
    }


    /**
     * Specify the Path id to be created.
     *
     * @param pathId The Path id to be created.
     */
    public void setPathId(String pathId) {
        m_pathId = pathId;
    }


    /**
     * Get the Path Id.
     *
     * @return The Path Id.
     */
    public String getPathId() {
        return m_pathId;
    }


    /**
     * Default constructor.
     */
    public DependencyToPath() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        Vector deps = new Vector();

        // Parse the output of the preprocessing.
        int pos = m_dependencies.indexOf(':');

        StringTokenizer st =
            new StringTokenizer(m_dependencies.substring(pos + 1), "\\");
        while (st.hasMoreTokens()) {
            String token = st.nextToken().trim();

            if (!token.equals("")) {
                Vector v = StringUtils.split(token, ' ');
                deps.addAll(v);
            }
        }

        // Create the new Path.
        Path path = new Path(getProject());
        Iterator iter = deps.iterator();
        while (iter.hasNext()) {
            try {
                // Dereference symlinks.
                File f = new File((String) iter.next());
                path.setLocation(new File(f.getCanonicalPath()));
            } catch (IOException ioe) {
                throw new BuildException(ioe);
            }
        }

        getProject().addReference(m_pathId, path);
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_dependencies == null) || m_dependencies.equals("")) {
            throw new BuildException("attribute dependencies must not be null.");
        }

        if ((m_pathId == null) || m_pathId.equals("")) {
            throw new BuildException("attribute pathId must not be null.");
        }
    }


}

