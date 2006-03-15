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


import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;


/**
 * Display the free memory and call the garbage collector.
 */
public class CallGC extends Task {


    // Flag.
    private boolean m_verbose = false;


    /**
     * Set verbose.
     *
     * @param verbose If <code>true</code> then display memory information.
     */
    public void setVerbose(boolean verbose) {
        m_verbose = verbose;
    }


    /**
     * Get the verbose value.
     *
     * @return <code>true</code> or <code>false</code>
     */
    public boolean getVerbose() {
        return m_verbose;
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        Runtime r = Runtime.getRuntime();

        long freeMem = r.freeMemory();
        
        if (m_verbose) {
            log("free memory before : " + freeMem, Project.MSG_WARN);
        }

        // Call the garbage collector.
        r.gc();

        if (m_verbose) {
            freeMem = r.freeMemory(); 
            log("free memory after  : " + freeMem, Project.MSG_WARN); 
        }
    }

    
}
