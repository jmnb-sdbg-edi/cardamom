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


package cdmw.ccm.deployment;


/**
 * This class represents a codebase reference in a package descriptor.
 *
 */
public class Codebase extends Link {

    /**
     * The filename of the codebase
     */
    protected String filename;


    /**
     * Constructor.
     *
     * @param filename the filename of the codebase. If this parameter is null,
     * the filename is retrieved from sourceLink.
     * @param sourceLink the link where to obtain the file if not installed.
     */
    public Codebase(String filename, String sourceLink)
        throws cdmw.common.BadParameterException
    {
        super(sourceLink);

        if ((filename == null) || (filename.equals(""))) {
            this.filename = this.sourceLink.getFile();
        } else {
            this.filename = filename;
        }

        this.actualLocation = null;
    }

    /**
     * Returns the filename of the Codebase.
     *
     * @return the filename of the codebase
     */
    public String getFilename() {
        return filename;
    }

    /**
     * Delete the installed file. For Codebase, this method does nothing and
     * always returns true.
     * (a Codebase could be used by several components or assemblies)
     *
     * @return always true.
     */
    public boolean delete() {
        // Do nothing. A codebase is never removed.
        // (it could be used by another package)
        return true;
    }

    /**
     * Constructs a string representation of this Codebase.
     * The string is a representation of the equivalent 
     * <codebase> XML element.
     *
     * @return a string representation of this object.
     */
    public String toString() {
        return "<codebase name=\"" + filename +
               "\" href=\"" + sourceLink.toString() + "\">";
    }

}







