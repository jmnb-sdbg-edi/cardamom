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

import com.thalesgroup.CdmwDeployment.RequirementDescription;


public class Requirement {

    private static final String VERSION_DELIMITER = ",";

    private String name;
    private byte[] version;


    public Requirement(String name, String version)
        throws cdmw.common.BadParameterException
    {
        if (name == null) {
            throw new NullPointerException("name is null");
        }
        this.name = name;

        if ((version == null) || version.equals("")) {
            this.version = null;

        } else {
            this.version = new byte[4];
            java.util.StringTokenizer st =
                new java.util.StringTokenizer(version, VERSION_DELIMITER);

            if (st.countTokens() != 4) {
                throw new cdmw.common.BadParameterException(
                    "version " + version + " is not made of 4 numbers");
            }

            for (int i = 0; i < this.version.length; i++) {
                try {
                    Byte b = new Byte(st.nextToken());
                    this.version[i] = b.byteValue();
                } catch (NumberFormatException e) {
                    throw new cdmw.common.BadParameterException(
                    "version '" + version + "' is not made of 4 numbers");
                }
            }
        }
    }

    public Requirement(RequirementDescription req)
        throws cdmw.common.BadParameterException
    {
        this(req.name, req.version);
    }


    public String getName() {
        return name;
    }

    public String getVersion() {
        if (version == null) {
            return "";
        }

        return "" + version[0] +
               VERSION_DELIMITER + version[1] +
               VERSION_DELIMITER + version[2] +
               VERSION_DELIMITER + version[3];
    }

    public String toString() {
        return getName() + " " + getVersion();
    }


    public int hashCode() {
        return name.hashCode();
    }

    public boolean equals(Object obj) {
        if (!(obj instanceof Requirement)) {
            return false;
        }

        Requirement req = (Requirement) obj;

        if (!name.equals(req.name)) {
            return false;
        }

        if ((version == null) || (req.version == null)) {
            return version == req.version;
        }

        return version[0] == req.version[0] &&
               version[1] == req.version[1] &&
               version[2] == req.version[2] &&
               version[3] == req.version[3];
    }

    // if no version specified for one, the one with version is more recent
    public boolean moreRecentThan(Requirement req)
        throws cdmw.common.BadParameterException
    {
        if (!name.equals(req.name)) {
            throw new cdmw.common.BadParameterException(
                "Cannot compare versions of " + name +
                " and " + req.name);
        }

        if ((version == null) || (req.version == null)) {
            // this is more recent if version is not null
            return version != null;
        }

        if (version[0] != req.version[0]) {
            return version[0] > req.version[0];
        } else if (version[1] != req.version[1]) {
            return version[1] > req.version[1];
        } else if (version[2] != req.version[2]) {
            return version[2] > req.version[2];
        } else if (version[3] != req.version[3]) {
            return version[3] > req.version[3];
        } else {
            // requirements are the same
            return false;
        }
    }

    public RequirementDescription getDescription() {
        RequirementDescription req = new RequirementDescription();
        req.name = name;
        req.version = getVersion();
        return req;
    }

}







