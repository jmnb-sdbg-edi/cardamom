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


package cdmw.ccm.assembly;

import com.thalesgroup.CdmwDeployment._AssemblyPackage;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.Node;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.XPathFormatException;


/**
 * Generic ProcessCollocationLoader.
 * This class determins if a processcollocation has fault tolerance
 * properties load balancing properties or no specific properties, and
 * calls the appropriate loader (FTProcessCollocationLoader, 
 * LBProcessCollocationLoader or DefaultProcessCollocationLoader).
 */
class ProcessCollocationLoader extends DefaultProcessCollocationLoader {


    /**
     * Specialized ProcessCollocationLoader for FT processes
     * (null if not compiled with FT)
     */
    private DefaultProcessCollocationLoader ftProcessCollocationLoader = null;

    /**
     * Specialized ProcessCollocationLoader for LB processes
     * (null if not compiled with LB)
     */
    private DefaultProcessCollocationLoader lbProcessCollocationLoader = null;


    /**
     * Constructor.
     */
    public ProcessCollocationLoader() {
        // try to load FTProcessCollocationLoader
        try {
            this.ftProcessCollocationLoader = (DefaultProcessCollocationLoader)
                Class.forName("cdmw.ccm.assembly.FTProcessCollocationLoader").newInstance();
        } catch (ClassNotFoundException e) {
            // FT is not available
        } catch (InstantiationException e) {
            // Shouldn't happen since FTProcessCollocationLoader is not abstract
            // and has a default constructor
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (IllegalAccessException e) {
            // Shouldn't happen since FTProcessCollocationLoader's
            // default constructor is accessible
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // try to load LBProcessCollocationLoader
        try {
            this.lbProcessCollocationLoader = (DefaultProcessCollocationLoader)
                Class.forName("cdmw.ccm.assembly.LBProcessCollocationLoader").newInstance();
        } catch (ClassNotFoundException e) {
            // FT is not available
        } catch (InstantiationException e) {
            // Shouldn't happen since FTProcessCollocationLoader is not abstract
            // and has a default constructor
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (IllegalAccessException e) {
            // Shouldn't happen since FTProcessCollocationLoader's
            // default constructor is accessible
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    }


    /**
     * Loads a processcollocation XML element, create a corresponding ComponentServer
     * and add it to its parent HostCollocation and to the assembly.
     * This operation delegates the loading to a DefaultProcessCollocationLoader,
     * to a FTProcessCollocationLoader or to a LBProcessCollocationLoader.
     *
     * @param processElt the processcollocation XML element
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     * @param parentHome the parent HostCollocation of the ComponentServer.
     *
     * @exception CreateFailure if loading fails.
     */
    void load(Element processElt,
              _AssemblyPackage assemblyPack,
              HostCollocation parentHost)
        throws CreateFailure
    {
        // if processcollocation as a faulttolerance declaration
        if (isFTProcess(processElt)) {
            if (ftProcessCollocationLoader != null) {
                // delegate loading to FTProcessCollocationLoader
                ftProcessCollocationLoader.load(processElt, assemblyPack, parentHost);
            } else {
                throw new CreateFailure(
                    "Processcollocation " + getProcesscollocationId(processElt) +
                    " is declared as FaultTolerant in .cad, but Fault Tolerance" +
                    " is not compiled with this version of Cardamom",
                    ERROR_IN_CAD.value);
            }

        // if processcollocation as a loadbalancing declaration
        } else if (isLBProcess(processElt)) {
            if (lbProcessCollocationLoader != null) {
                // delegate loading to LBProcessCollocationLoader
                lbProcessCollocationLoader.load(processElt, assemblyPack, parentHost);
            } else {
                throw new CreateFailure(
                    "Processcollocation " + getProcesscollocationId(processElt) +
                    " is declared as LoadBalanced in .cad, but Load Balancing" +
                    " is not compiled with this version of Cardamom",
                    ERROR_IN_CAD.value);
            }

        // default processcollocation loading
        } else {
            super.load(processElt, assemblyPack, parentHost);
        }
    }


    /**
     * Returns true if the processcollocation XML element
     * has fault tolerance specifications.
     *
     * @param processElt the processcollocation XML element.
     * @return true if the processcollocation is FT
     */
    private boolean isFTProcess(Element processElt) {
        try {
            NodeDescription[] ftElt = processElt.select_nodes(
                XMLHelper.toShortArray("faulttolerant"));
            return (ftElt.length != 0);
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
            return false;
        }
    }


    /**
     * Returns true if the processcollocation XML element
     * has load balancing specifications.
     *
     * @param processElt the processcollocation XML element.
     * @return true if the processcollocation is LB
     */
    private boolean isLBProcess(Element processElt) {
        try {
            NodeDescription[] lbElt = processElt.select_nodes(
                XMLHelper.toShortArray("loadbalanced"));

            return (lbElt.length != 0);
        } catch (XPathFormatException e) {
            // this should not happen
            cdmw.common.Assert.check(false);
            return false;
        }
    }


}
