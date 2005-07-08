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

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.XPathFormatException;

import cdmw.xmlutils.XMLHelper;


import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;



public class CcdReader extends XmlReader {

    private static final String CCD_DOCTYPE = "corbacomponent";
    
    private static final String XPATH_HOME_REPID = 
        "/corbacomponent/homerepid/@repid";
    private static final String XPATH_COMPONENT_REPID = 
        "/corbacomponent/componentrepid/@repid";
    private static final String XPATH_COMPONENT_KIND = 
        "/corbacomponent/componentkind/*";
    private static final String XPATH_SERVANT_LIFETIME = 
        "/corbacomponent/componentkind/*/servant/@lifetime";
    private static final String XPATH_THREADING_POLICY =
        "/corbacomponent/threading/@policy";
    private static final String XPATH_HOMEFEATURES =
        "/corbacomponent/homefeatures";
    private static final String XPATH_HOME_SERVANT_CLASSNAME =
        "extension[@origin=\"Cdmw\"][@class=\"HOME_SERVANT_CLASSNAME\"]/text()";
    private static final String XPATH_CONFIGURATION_COMPLETE =
        "/corbacomponent/configurationcomplete/@set";
    
    public CcdReader(XMLParser parser, String csdFilename) 
        throws ParsingException
    {
        super(parser, csdFilename);
        
        if (! XMLHelper.toString(docDescr.doctype()).equals(CCD_DOCTYPE) ) {
            throw new ParsingException(
                csdFilename + " is not a .ccd file.");
        }
    }



    public String getHomeRepositoryId() {
        String homeRepid = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_HOME_REPID);
           
            // attribute must have been found (cad validated by dtd)
            AttrDescription repidAttr = (AttrDescription) nodes[0];
            homeRepid = XMLHelper.toString(repidAttr.value());
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return homeRepid;
    }
    

    public String getComponentRepositoryId() {
        String compRepid = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_COMPONENT_REPID);
           
            // attribute must have been found (cad validated by dtd)
            AttrDescription repidAttr = (AttrDescription) nodes[0];
            compRepid = XMLHelper.toString(repidAttr.value());
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return compRepid;
    }
    

    public ComponentKindValue getComponentKind() {
        ComponentKindValue compKind = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_COMPONENT_KIND);
            
            // element must have been found (cad validated by dtd)
            ElementDescription eltDesc = (ElementDescription) nodes[0];
            String kind = XMLHelper.toString(eltDesc.name());
            
            if (kind.equals("service")) {
                compKind = ComponentKindValue.SERVICE;
            } else if (kind.equals("session")) {
                compKind = ComponentKindValue.SESSION;
            } else if (kind.equals("process")) {
                compKind = ComponentKindValue.PROCESS;
            } else if (kind.equals("entity")) {
                compKind = ComponentKindValue.ENTITY;
            } else if (kind.equals("unclassified")) {
                compKind = ComponentKindValue.UNCLASSIFIED;
            } else {
                // shouldn't happen (cad validated by dtd)
                cdmw.common.Assert.check(false);
            } 
        
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return compKind;
    }
    
    
    // return null is not applicable (service or unclassified component)
    public ServantLifetimeValue getServantLifetime() {
        ServantLifetimeValue servLifetime = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_SERVANT_LIFETIME);
            
            if (nodes.length != 0) {
                AttrDescription lifetimeAttr = (AttrDescription) nodes[0];
                String lifetime = XMLHelper.toString(lifetimeAttr.value());
            
                if (lifetime.equals("component")) {
                    servLifetime = ServantLifetimeValue.COMPONENT_LIFETIME;
                } else if (lifetime.equals("method")) {
                    servLifetime = ServantLifetimeValue.METHOD_LIFETIME;
                } else if (lifetime.equals("transaction")) {
                    servLifetime = ServantLifetimeValue.TRANSACTION_LIFETIME;
                } else if (lifetime.equals("container")) {
                    servLifetime = ServantLifetimeValue.CONTAINER_LIFETIME;
                } else {
                    // shouldn't happen (cad validated by dtd)
                    cdmw.common.Assert.check(false);
                }
            }
                
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return servLifetime;
    }
    
    public ThreadingPolicyValue getThreadingPolicy() {
        ThreadingPolicyValue threadPolicy = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_THREADING_POLICY);
        
            // attribute must have been found (cad validated by dtd)
            AttrDescription threadAttr = (AttrDescription) nodes[0];
            String threading = XMLHelper.toString(threadAttr.value());

            if (threading.equals("serialize")) {
                threadPolicy = ThreadingPolicyValue.SERIALIZE;
            } else if (threading.equals("multithread")) {
                threadPolicy = ThreadingPolicyValue.MULTITHREAD;
            } else {
                // shouldn't happen (cad validated by dtd)
                cdmw.common.Assert.check(false);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return threadPolicy;
    }
    
    // return null if HomeServantClassname not found
    public String getHomeServantClassname(String homeRepid) {
        String classname = null;
        try {
            // get HOME_SERVANT_CLASSNAME <extension> from <homeheatures>
            // with appropriate repid
            NodeDescription[] nodes = selectNodes(
                XPATH_HOMEFEATURES + "[@repid=\"" + homeRepid + "\"]/"+
                XPATH_HOME_SERVANT_CLASSNAME);
        
            if (nodes.length != 0) {
                TextDescription txtDescr = (TextDescription) nodes[0];
                classname = XMLHelper.toString(txtDescr.data());
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return classname;
    }
    
    
    public boolean getConfigurationcomplete() {
        String confComplete = null;
        try {
            // get set attribute of <configurationcomplete>
            NodeDescription[] nodes = selectNodes(
                XPATH_CONFIGURATION_COMPLETE);
                
            // should have found 1 attribute (configurationcomplete is required
            // and .ccd has been validated by dtd)
            AttrDescription confDescr = (AttrDescription) nodes[0];
            confComplete = XMLHelper.toString(confDescr.value());
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        if (confComplete.equals("true")) {
            return true;
        } else if (confComplete.equals("false")) {
            return false;
        } else {
            // shouldn't happen, only "true" or "false" are
            // accepted by corbacomponent.dtd
            cdmw.common.Assert.check(false);
            return false;     // for compiler
        }
    }
    
    
}

