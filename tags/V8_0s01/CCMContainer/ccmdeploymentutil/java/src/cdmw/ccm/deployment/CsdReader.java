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


package cdmw.ccm.deployment;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;
import cdmw.xmlutils.XMLHelper;

import org.omg.Components.CreateFailure;

import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;


public class CsdReader extends XmlReader {

    private static final String CSD_DOCTYPE = "softpkg";
    
    private static final String XPATH_NAME_ATTR        = "/softpkg/@name";
    private static final String XPATH_VERSION_ATTR     = "/softpkg/@version";
    private static final String XPATH_IMPLEMENTATION   = "/softpkg/implementation";
    private static final String XPATH_PROCESSOR_NAME   = "processor/@name";
    private static final String XPATH_OS               = "os";
    private static final String XPATH_LANGUAGE_NAME    = "programminglanguage/@name";
    private static final String XPATH_LANGUAGE_VERSION = "programminglanguage/@version";
    private static final String XPATH_COMPILER_NAME    = "compiler/@name";
    private static final String XPATH_COMPILER_VERSION = "compiler/@version";
    private static final String XPATH_RUNTIME_NAME     = "runtime/@name";
    private static final String XPATH_RUNTIME_VERSION  = "runtime/@version";
    private static final String XPATH_CODE_TYPE        = "code/@type";
    

    
    public CsdReader(XMLParser parser, String csdFilename) 
        throws ParsingException
    {
        super(parser, csdFilename);
        
        if (! XMLHelper.toString(docDescr.doctype()).equals(CSD_DOCTYPE) ) {
            throw new ParsingException(
                csdFilename + " is not a .csd file.");
        }
    }


    public String getName() {
        String name = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_NAME_ATTR);
           
            AttrDescription nameAttr = (AttrDescription) nodes[0];
            name = XMLHelper.toString(nameAttr.value());
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return name;
    }
    
    
    public String getVersion() {
        String version = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_VERSION_ATTR);
            if (nodes.length == 0) {
                // no version specified (version is implied)
                version = "";
            } else {
                AttrDescription verAttr = (AttrDescription) nodes[0];
                version = XMLHelper.toString(verAttr.value());
            }
                    
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return version;
    }
    
 
    // return null if no processor specified
    public String getProcessorName(String implUUID) {
        String processorName = null;
        try {
            NodeDescription[] nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_PROCESSOR_NAME);
            if (nodes.length != 0) {
                AttrDescription procAttr = (AttrDescription) nodes[0];
                processorName = XMLHelper.toString(procAttr.value());
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return processorName;
    }
    

    public RequirementList getOsList(String implUUID)  
        throws CreateFailure
    {
        RequirementList list = new RequirementList();
        try {
            NodeDescription[] nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_OS);
            
            for (int i = 0; i < nodes.length; i++) {
                ElementDescription eltDescr = (ElementDescription) nodes[i];
                Element elt = ElementHelper.narrow(eltDescr.reference());
                
                // get name of OS
                String name = null;
                try {
                    AttrDescription nameDescr = elt.get_attribute(
                        XMLHelper.toShortArray("name"));
                    name = XMLHelper.toString(nameDescr.value());
                
                } catch (DOMException e) {
                    // shouldn't happen, name is required for <os> (validated by dtd)
                    cdmw.common.Assert.check(false);
                }
                
                // get version of OS
                String version = null;
                try {
                    AttrDescription verDescr = elt.get_attribute(
                        XMLHelper.toShortArray("version"));
                    version = XMLHelper.toString(verDescr.value());
                
                } catch (DOMException e) {
                    // no version specified
                    version = "";
                }
                
                // add os requirement to list
                try {
                    Requirement req = new Requirement(name, version);
                    list.add(req);
                
                } catch (cdmw.common.BadParameterException e) {
                    // error in requirement
                    throw new CreateFailure(
                        "Error while reading <os name=\"" + name +
                        "\" version=\"" + version +  "\"> : " + 
                        e.getMessage(),
                        ERROR_IN_CAD.value);
                }
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        return list;
    }
    
    
    public Requirement getProgrammingLanguage(String implUUID)  
        throws CreateFailure
    {
        Requirement lang = null;
        try {
            // get name of programminglanguage
            NodeDescription[] nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_LANGUAGE_NAME);
            if (nodes.length == 0) {
                // no programminglanguage specified
                return null;
            }
            AttrDescription nameAttr = (AttrDescription) nodes[0];
            String name = XMLHelper.toString(nameAttr.value());

            // get version of programminglanguage
            nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_LANGUAGE_VERSION);
            String version = "";
            if (nodes.length != 0) {
                AttrDescription verAttr = (AttrDescription) nodes[0];
                version = XMLHelper.toString(verAttr.value());
            }

            // create Requirement
            try {
                lang = new Requirement(name, version);
            } catch (cdmw.common.BadParameterException e) {
                // error in requirement
                throw new CreateFailure(
                    "Error while reading <programinglanguage name=\"" + 
                    name + "\" version=\"" + version +  "\"> : " + 
                    e.getMessage(),
                    ERROR_IN_CAD.value);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return lang;
    }

    

    public Requirement getCompiler(String implUUID) 
        throws CreateFailure
    {
        Requirement compiler = null;
        try {
            // get name of compiler
            NodeDescription[] nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_COMPILER_NAME);
            if (nodes.length == 0) {
                // no compiler specified
                return null;
            }
            AttrDescription nameAttr = (AttrDescription) nodes[0];
            String name = XMLHelper.toString(nameAttr.value());

            // get version of compiler
            nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_COMPILER_VERSION);
            String version = "";
            if (nodes.length != 0) {
                AttrDescription verAttr = (AttrDescription) nodes[0];
                version = XMLHelper.toString(verAttr.value());
            }

            // create Requirement
            try {
                compiler = new Requirement(name, version);
            } catch (cdmw.common.BadParameterException e) {
                // error in requirement
                throw new CreateFailure(
                    "Error while reading <compiler name=\"" + 
                    name + "\" version=\"" + version +  "\"> : " + 
                    e.getMessage(),
                    ERROR_IN_CAD.value);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
        return compiler;
    }


    public Requirement getRuntime(String implUUID)
        throws CreateFailure
    {
        Requirement runtime = null;
        try {
            // get name of runtime
            NodeDescription[] nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_RUNTIME_NAME);
            if (nodes.length == 0) {
                // no compiler specified
                return null;
            }
            AttrDescription nameAttr = (AttrDescription) nodes[0];
            String name = XMLHelper.toString(nameAttr.value());

            // get version of runtime
            nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_RUNTIME_VERSION);
            String version = "";
            if (nodes.length != 0) {
                AttrDescription verAttr = (AttrDescription) nodes[0];
                version = XMLHelper.toString(verAttr.value());
            }

            // create Requirement
            try {
                runtime = new Requirement(name, version);
            } catch (cdmw.common.BadParameterException e) {
                // error in requirement
                throw new CreateFailure(
                    "Error while reading <runtime name=\"" + 
                    name + "\" version=\"" + version +  "\"> : " + 
                    e.getMessage(),
                    ERROR_IN_CAD.value);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
        return runtime;
    }


    public String getCodeType(String implUUID) {
        String codeType = null;
        try {
            NodeDescription[] nodes = selectNodes(
                XPATH_IMPLEMENTATION + "[@id=\"" + implUUID + "\"]/" +
                XPATH_CODE_TYPE);
            if (nodes.length != 0) {
                AttrDescription codeTypeAttr = (AttrDescription) nodes[0];
                codeType = XMLHelper.toString(codeTypeAttr.value());
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return codeType;
    }
    
    public String getEntrypoint(String implUUID) {
        String entrypoint = null;
        try {
            NodeDescription[] nodeDescr = selectNodes(
                "/softpkg/implementation[@id=\"" + implUUID +
                "\"]/code/entrypoint/text()");
            if (nodeDescr.length != 0) {
                TextDescription txtDescr = (TextDescription) nodeDescr[0];
                entrypoint = XMLHelper.toString(txtDescr.data());
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
        return entrypoint;
    }

}

