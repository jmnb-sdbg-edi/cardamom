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


package cdmw.xmlutils;

import org.apache.xerces.parsers.DOMParser;
import org.apache.xerces.jaxp.DocumentBuilderFactoryImpl;
import org.xml.sax.InputSource;

import org.w3c.dom.Node;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.NodeList;
import org.w3c.dom.DocumentType;
import org.w3c.dom.DOMImplementation;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.PolicyList;

import com.thalesgroup.CdmwXML.DocumentDescription;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;

/**
 * Implementation class of XMLParser.
 *
 */
public class XMLParserImpl extends com.thalesgroup.CdmwXML.XMLParserPOA {

    /**
     * Reference to the current ORB
     */
    private org.omg.CORBA.ORB orb;

    /**
     * POA managing the CdmwXML Objects
     */
    private org.omg.PortableServer.POA xmlPoa;

    /**
     * Command line arguments
     */
    private String[] args;

    /**
     * Name of the CdmwXML POA
     */
    public static final String XML_PARSER_POA = "XMLParserPOA";

    /**
     * Name of the CdmwXML Parser
     */
    public static final String XML_PARSER_NAME = "XMLParser";

    /**
     * Corbaloc ID for the CdmwXML Parser
     */
    public static final String XML_PARSER_CORBALOC_ID = "XMLService";

    /**
     * Command line option to give a specific directory for DTDs
     */
    public static final String DTD_DIRNAME_OPTION = "--doctypedir";

    /**
     * Command line option to validate the XML file
     */
    public static final String VALIDATING = "--validate";

    /**
     * Command line optioon to display help message
     */
    public static final String HELP = "--help";

    /**
     * Doctype dir.
     */
    private String m_doctypeDir = "";

    /**
     * Initialize CdmwXML environment by creating a relevant POA
     * and returning an instance of XMLParserImpl
     *
     * @param orb Reference to the current ORB
     * @param parent Parent POA
     * @return A reference to the XML Parser
     */
    public static com.thalesgroup.CdmwXML.XMLParser init(
        org.omg.CORBA.ORB orb, org.omg.PortableServer.POA parent, String[] args) {

        try {
            // Create policy list for simple persistence
            org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[6];

            pl[0] = parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
            pl[1] = parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.TRANSIENT);
            pl[2] = parent.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
            pl[3] = parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.MULTIPLE_ID);
            pl[4] = parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_ACTIVE_OBJECT_MAP_ONLY);
            pl[5] = parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

            // Create new POA
            StrategyList poaStrategies = new StrategyList();
            // Strategies with MULTITHREAD policy will accept Single Thread and
            // Multi Thread strategies. However, SERIALIZE policy will only allow
            // Single Thread policy.
            org.omg.PortableServer.POA poa = ORBSupport.createPOA(parent,
                XML_PARSER_POA, parent.the_POAManager(), new PolicyList(pl), poaStrategies);

            // create instance
            XMLParserImpl instance = new XMLParserImpl(orb, poa, args);

            // activate object
            byte[] oid = XML_PARSER_NAME.getBytes();
            poa.activate_object_with_id(oid, instance);
            org.omg.CORBA.Object obj = poa.id_to_reference(oid);
            return com.thalesgroup.CdmwXML.XMLParserHelper.narrow(obj);
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(aae.getMessage());
        } catch (org.omg.PortableServer.POAPackage.ObjectAlreadyActive oaa) {
            throw new org.omg.CORBA.INTERNAL(oaa.getMessage());
        } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive saa) {
            throw new org.omg.CORBA.INTERNAL(saa.getMessage());
        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            throw new org.omg.CORBA.INTERNAL(ona.getMessage());
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(ip.getMessage());
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new org.omg.CORBA.INTERNAL(wp.getMessage());
        } catch (org.omg.CORBA.SystemException se) {
            throw new org.omg.CORBA.INTERNAL(se.getMessage());
        }

    }

    /**
     * Constructor.
     *
     * @param orb Reference to the current ORB.
     * @param poa POA managing the CdmwXML objects
     * @param args Command line arguments
     */
    public XMLParserImpl(
        org.omg.CORBA.ORB orb, org.omg.PortableServer.POA poa, String[] args) {
        this.orb = orb;
        this.xmlPoa = poa;
        this.args = args;
    }

    /**
     * Returns the default POA.
     *
     * @return Default POA.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return this.xmlPoa;
    }

    //
    // IDL:thalesgroup.com/CdmwXML/XMLParser/parse:1.0
    //
    /**
     * Parses an XML file and return a DocumentDescription for this file.
     *
     * @param filename Name of the file to be parsed.
     * @exception ParsingException Error during parsing
     * @return The DocumentDescription object of the parsed file.
     */
    public DocumentDescription parse(String filename) throws ParsingException {

        /********** PCR-0640 ************
        java.io.File tmpFile = null;
        ********************************/
        
        // convert filename into URL
        java.net.URL url = null;
        try {
            url = toURL(filename);
        } catch (cdmw.common.BadParameterException e) {
            throw new ParsingException(e.getMessage());
        }
        
        // open InputStream to URL
        java.io.InputStream is = null;
        try {
            is = url.openStream();
        } catch (java.io.IOException e) {
            throw new ParsingException("Cannot open stream for " + filename + 
                                       ": " + e.toString());
        }

        /********** PCR-0640 ************
           // There is a bug with jdk 1.4.1 on linux:
           // When opening an FTP URL which point to the localhost,
           // it may happen that some ending characters are truncated.
           // This seems to depend on the file size (a 2050 bytes fails) and on the
           // extension of the file (.txt works, .ccd fails)
           // With a debugger, we can see that the sun.net.www.MeterdedStream
           // returned by the URL.openStream() operation has a wrong count of 8
           // (count is 0 in working case)
           //
           // When the bug occurs, the XMLServer throws a parsing exception, indicating
           // that the "XML document structures must start and end within the same entity".
           //
           // The work-around consist in using another FTP client.
           // Here, we use the FTP client from enterprisedt.com.
        
        if (url.getProtocol().equals("ftp")) {
            try {
                is.close();
                tmpFile = copyURL(url);
            } catch (java.io.IOException e) {
                throw new ParsingException("Error while reading stream for " + filename + 
                                           ": " + e.toString());
            }
            
            try {
                is = toURL("file:" + tmpFile.getAbsolutePath()).openStream();
            } catch (cdmw.common.BadParameterException e) {
                if (tmpFile != null)
                    tmpFile.delete();
                throw new ParsingException(e.getMessage());
            } catch (java.io.IOException e) {
                if (tmpFile != null)
                    tmpFile.delete();
                throw new ParsingException("Cannot open stream to file:" + tmpFile.getAbsolutePath());
            }
        }
        ********************************/
        
        // create InputSource with InputStream
        InputSource source = new InputSource(is);

        // try to find the DTD file in another directory if specified
        String dirname = OS.getOptionValue(args, DTD_DIRNAME_OPTION);

        // String userDir = null;

        // if doctypedir specified, cd doctypedir
        if ( !dirname.equals("yes") && !dirname.equals("no") ) {
            /*
            // save current user.dir property to retrieve it after change
            java.util.Properties props = System.getProperties();
            userDir = props.getProperty("user.dir");
            */

            // get absolute path of DTD directory
            java.io.File dir = new java.io.File(dirname);
            //String dirPath = dir.getAbsolutePath();
            m_doctypeDir = dir.getAbsolutePath();

            /*
            // change current directory into DTD dir
            props.put("user.dir", dirPath);
            */
        }
        
        // parse InputSource
        try {
            return tryToParse(source, url.toString());
        } catch(org.xml.sax.SAXException se) {
            throw new ParsingException(formatErrorMessage(se));

        } catch(java.io.IOException ioe) {
            throw new ParsingException(ioe.getMessage());
        
        } finally {
            if (is != null) {
                try {
                    is.close();
                } catch (java.io.IOException e) {
                    // ignore
                }
            }
            /*
            if (userDir != null) {
                // change current directory into userDir
                System.getProperties().put("user.dir", userDir);
            }
            */
            /********** PCR-0640 ************
            if (tmpFile != null) {
                tmpFile.delete();
            }
            ********************************/
        }
    }
        

    /**
     * Method for PCR-0264 (see comment above)
     */
    /*
    private java.io.File copyURL(java.net.URL url) 
        throws java.io.IOException
    {
        java.io.File tmpFile = null;
        
        try {
            tmpFile = java.io.File.createTempFile("tmpCopiedFile", "xml");

            com.enterprisedt.net.ftp.FTPClient ftp = 
                new com.enterprisedt.net.ftp.FTPClient(url.getHost());
                
            ftp.login("anonymous", "xmlServer@cdmw");
            ftp.setType(com.enterprisedt.net.ftp.FTPTransferType.BINARY);
            
            java.io.FileOutputStream os =
                new java.io.FileOutputStream(tmpFile);

            ftp.get(os, url.getPath());
                        
            os.close();
            ftp.quit();

            return tmpFile;

        } catch (com.enterprisedt.net.ftp.FTPException e) {
            if (tmpFile != null)
                tmpFile.delete();
            throw new java.io.IOException(e.getMessage());
        
        } catch (java.io.IOException e) {
            if (tmpFile != null)
                tmpFile.delete();
            // re-throw exception
            throw e;
        }
    }
    */
    
    
    /**
     * Tries to parse the input source given as parameter.
     *
     * @param source The input source to try to parse.
     * @return The document description corresponding to the source
     * @exception org.xml.sax.SAXException no Xerces description
     * @exception java.io.IOException no Xerces description
     */
    private DocumentDescription tryToParse(InputSource source, String file)
        throws org.xml.sax.SAXException, java.io.IOException {

        DOMParser parser = new DOMParser();
        ErrorHandlerImpl errorHandler = new ErrorHandlerImpl();
        parser.setErrorHandler(errorHandler);
        String validating = OS.getOptionValue(args, VALIDATING);
        if ( validating.equals("yes") ) {
            parser.setFeature("http://xml.org/sax/features/validation", true);
            parser.setFeature("http://apache.org/xml/features/validation/dynamic", true);

            parser.setEntityResolver(
                (org.xml.sax.EntityResolver) new LocalDTDResolver());
        }
        parser.parse(source);
        Document domDoc = parser.getDocument();
        return (new DOMCommonImpl(orb, xmlPoa)).setDomDocument(domDoc, file);

    }


    /**
     * Converts a location into a URL. The location could be a URL,
     * an absolute path or a relative path. In case of a path, the created URL
     * follows the format: file:/<absolute_path_location>
     *
     * @param location The location of the file.
     * @return An URL object corresponding to the location
     * @exception cdmw.common.BadParameterException if the location is null
     * or is an invalid URL
     */
    private static java.net.URL toURL(String location)
        throws cdmw.common.BadParameterException {

        if (location == null) {
            throw new cdmw.common.BadParameterException(
                "Location is null.");
        }

        java.net.URL url = null;
        try {
            // try direct convertion into URL
            url = new java.net.URL(location);
        } catch (java.net.MalformedURLException e) {
            // if it's not an URL, test is it's a local file
            java.io.File file = new java.io.File(location);

            if (!file.exists()) {
                throw new cdmw.common.BadParameterException(
                    "file " + location + " doesn't exist");
            }
            if (!file.canRead()) {
                throw new cdmw.common.BadParameterException(
                    "cannot read file " + location);
            }
            try {
                // convert file into URL
                url = file.toURL();
            } catch (java.net.MalformedURLException ex) {
                // should not happen
                cdmw.common.Assert.check(false);
            }
        }
        return url;
    }

    /**
     * Formats an explicit error message corresponding to a SAXException.
     *
     * @param se SAXException whose message shall be formatted.
     * @return Explicit error message
     */
    private String formatErrorMessage(org.xml.sax.SAXException se) {

        String msg = "";
        if (se instanceof org.xml.sax.SAXParseException) {
            org.xml.sax.SAXParseException spe
                = (org.xml.sax.SAXParseException) se;
            msg += (spe.getSystemId() != null ? spe.getSystemId() : "");
            msg += (spe.getPublicId() != null ? spe.getPublicId() : "");
            msg += ":" + spe.getLineNumber();
            msg += ":" + spe.getColumnNumber() + ": ";
        }
        return msg + se.getMessage();

    }

    //
    // IDL:thalesgroup.com/CdmwXML/XMLParser/create_empty_document:1.0
    //
    /**
     * Creates an empty DOM document.
     *
     * @param xmlFilename Name of the new XML file
     * @param doctype Name of the doctype
     * @param dtdFilename Name of the file of the DTD
     * @return The DocumentDescription of the newly created document.
     */
    public DocumentDescription create_empty_document(
        String xmlFilename, short[] doctype, String dtdFilename) {

        try {
            String qualifiedName = XMLHelper.toString(doctype);
            DOMImplementation domImplem = DocumentBuilderFactoryImpl
                .newInstance().newDocumentBuilder().getDOMImplementation();
            DocumentType dtd = null;
            if (dtdFilename.length() != 0) {
                dtd = domImplem.createDocumentType(
                    qualifiedName, null, dtdFilename);
            }
            Document domDoc = domImplem.createDocument(
                null, qualifiedName, dtd);
            DOMCommonImpl commonInterface = new DOMCommonImpl(orb, xmlPoa);
            
            // xmlFilename should be converted as "file:" URL
            return commonInterface.setDomDocument(domDoc, "file:" + xmlFilename);
        } catch(javax.xml.parsers.ParserConfigurationException pce) {
            pce.printStackTrace();
            return null;
        }

    }


    /**
     * Resolve external DTD.
     */
    class LocalDTDResolver implements org.xml.sax.EntityResolver {

        public InputSource resolveEntity(String publicID, String systemID)
            throws org.xml.sax.SAXException {

            java.io.File f;
            String path = "";

            try {
                f = new java.io.File(new java.net.URL(systemID).getFile());
            } catch (java.net.MalformedURLException mue) {
                if (systemID != null) {
                    f = new java.io.File(systemID);
                } else {
                    // May happen when parsed by an older version of Xerces.
                    mue.printStackTrace();
                    return null;
                }
            }

            if ((m_doctypeDir == null) || m_doctypeDir.trim().equals("")) {
                path = f.getName();
            } else {
                path = m_doctypeDir + java.io.File.separator + f.getName();
            }

            return new InputSource(path);
        }
    }

}

