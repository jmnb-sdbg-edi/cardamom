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


import com.thalesgroup.tools.ant.util.StringUtils;

import java.io.File;

import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;


/**
 * Update the list of dependencies.
 */
public class UpdateDependencies extends Task {


    // The list of prerequisites to be built.
    private Vector m_prereqs = new Vector();

    // The list of prerequisites that have already been built.
    private Vector m_builtPrereqs = new Vector();

    // The property to be created.
    private String m_property = null;


    /**
     * Specify the list of prerequisites to be built.
     *
     * @param prereqs The list of prerequisites to be built.
     */
    public void setPrereqs(String prereqs) {
        m_prereqs =
            org.apache.tools.ant.util.StringUtils.split(
                StringUtils.normalizeSpaces(prereqs), ' ');
    }


    /**
     * Get the list of prerequisites to be built.
     *
     * @return The list of prerequisites to be built.
     */
    public Collection getPrereqs() {
        return m_prereqs;
    }


    /**
     * Specify the list of prerequisites that have already been built.
     *
     * @param builtPrereqs The list of prerequisites that have already been
     *                     built.
     */
    public void setBuiltPrereqs(String builtPrereqs) {
        m_builtPrereqs =
            org.apache.tools.ant.util.StringUtils.split(
                StringUtils.normalizeSpaces(builtPrereqs), ' ');
    }


    /**
     * Get the list of prerequisites that have already been built.
     *
     * @return The list of prerequisites that have already been built.
     */
    public Collection getBuiltPrereqs() {
        return m_builtPrereqs;
    }


    /**
     * Specify the property to be created.
     *
     * @param property The property to be created.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Get the property to be created.
     *
     * @return The property to be created.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Constructor.
     */
    public UpdateDependencies() {
        super();
    }

    /**
     * Runs the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        m_prereqs.removeAll(m_builtPrereqs);

        StringBuffer sb = new StringBuffer();

        if (!m_prereqs.isEmpty()) {
            LinkedHashSet set = new LinkedHashSet();

            Iterator iter = m_prereqs.iterator();
            while (iter.hasNext()) {
                File f = new File((String) iter.next());
                sb.append(f.getParentFile().getAbsolutePath());
                sb.append(" ");
            }
        }

        getProject().setNewProperty(m_property, sb.toString().trim());
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_prereqs == null) || m_prereqs.isEmpty()) {
            throw new BuildException("attribute prereqs must not be null.");
        }

        if ((m_builtPrereqs == null) || m_builtPrereqs.isEmpty()) {
            throw new BuildException(
                "attribute builtPrereqs must not be null.");
        }

        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException("attribute property must not be null.");
        }
    }


}


