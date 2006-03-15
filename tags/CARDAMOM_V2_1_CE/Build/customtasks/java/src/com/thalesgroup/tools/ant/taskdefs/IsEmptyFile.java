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

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.ProjectComponent;
import org.apache.tools.ant.taskdefs.condition.Condition;


/**
 * Condition that checks if the specified file is empty.
 */
public class IsEmptyFile
    extends ProjectComponent
    implements Condition {


    private File m_file = null;

    /**
     * @param file the file to check.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * @return <code>true</code> if the file is empty.
     *
     * @throws BuildException if the file is not set.
     */
    public boolean eval()
        throws BuildException {

        // Sanity check.
        if (m_file == null) {
            throw new BuildException("attribute file set to null");
        }

        // Perform check.
        return (m_file.length() == 0);
    }


}
