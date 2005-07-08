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


package cdmw.cdmwinit;

import cdmw.common.InternalErrorException;
import cdmw.common.BadParameterException;
import cdmw.common.Assert;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.apache.xerces.parsers.DOMParser;

/**
 * This class serves as a facility for accessing to configuration information
 * that is provided in an XML file and that is needed by the CDMW init.
 *
 */
public class CdmwInitConfiguration {

    /**
     * Default XML parser feature
     */
    private static boolean setNameSpaces    = false;

    /**
     * Default XML parser feature
     */
    private static boolean setSchemaSupport = true;

    /**
     * The DOM parser
     */
    protected DOMParser parser;

    /**
     * The DOM error handler
     */
    protected ErrorHandler errorReporter;

    /**
     * The parent node for all services. This node
     * is the starting point for the getServiceAttribute method.
     */
    protected Node servicesNode = null;

    /**
     * The parent node for all factories. This node
     * is the starting point for the getFactoryAttribute method.
     */
    protected Node lifeCycleServiceNode = null;

    /**
     * Constructs a CdmwInitConfiguration
     * by initializing the underlying XML parser.
     *
     * @exception ConfigurationException Configuration exception
     * @exception InternalErrorException Error during XML parsing
     */
    protected CdmwInitConfiguration()
        throws ConfigurationException, InternalErrorException {

        this(false);

    }

    /**
     * Constructs a CdmwInitConfiguration
     * by initializing the underlying XML parser.
     *
     * @exception ConfigurationException Configuration exception
     * @exception InternalErrorException Error during XML parsing
     */
    protected CdmwInitConfiguration(boolean doValidate)
        throws ConfigurationException, InternalErrorException {

        try {
            //
            //  Create our parser, then attach an error handler to the parser.
            //  The parser will call back to methods of the ErrorHandler if it
            //  discovers errors during the course of parsing the XML document.
            //
            this.parser = new DOMParser();

            if (doValidate) { // TODO: Check exceptions to be thrown...
                parser.setFeature(
                    "http://xml.org/sax/features/namespaces", setNameSpaces );
                parser.setFeature(
                    "http://apache.org/xml/features/validation/schema", setSchemaSupport );
            }

            DOMTreeErrorReporter errorHandler = new DOMTreeErrorReporter();
            parser.setErrorHandler(errorHandler);

        } catch(SAXException se) {
            throw new InternalErrorException();
        }

    }

    /**
     * Sets up the init configuration with the
     * specified XML configuration file
     *
     * @return A CdmwInitConfiguration object.
     *
     * @param XMLConfigurationFile the XML configuration file
     * @param doValidate XML file validation option
     *
     * @exception ConfigurationException Configuration exception
     * @exception InternalErrorException Error during XML parsing
     */
    public static CdmwInitConfiguration
        readConfiguration(String XMLConfigurationFile,boolean doValidate)
        throws ConfigurationException, InternalErrorException {

        CdmwInitConfiguration conf = new CdmwInitConfiguration(doValidate);
        conf.readConfigurationFile(XMLConfigurationFile);
        return conf;

    }


    /**
     * Gets the value of the specified service's attribute
     *
     * @param service   The service that owns the attribute
     * @param attribute The attribute whose value is queried
     *
     * @return The value of the specified attribute
     *
     * @exception BadParameterException Bad parameter value
     * @exception XMLErrorException Error during XML parsing
     */
    public String getServiceAttributeValue(String service, String attribute)
        throws BadParameterException,XMLErrorException {

        boolean serviceFound = false;
        boolean attributeFound = false;

        String attributeValue = null;

        Assert.check(servicesNode != null);

        // search the specified service among the children nodes
        Node serviceNode = findNode(servicesNode, service);

        if (serviceNode != null) {
            serviceFound = true;

            // search the specified attribute
            NamedNodeMap attributes = serviceNode.getAttributes();
            Node attribName = attributes.getNamedItem(attribute);

            if (attribName!=null) {
                // attribute found
                attributeFound = true;
                attributeValue = attribName.getNodeValue();
            }

        }

        if (!serviceFound) {
            throw new BadParameterException("service not found");
        }

        if (!attributeFound) {
            throw new BadParameterException("attribute not found");
        }

        return attributeValue;

    }


    /**
     * Gets the value of the specified factory's attribute corresponding to :
     * - the specified factory type
     * - specified service's attribute
     *
     * @param framework         The factory type
     * @param frameworkInstance The index of the factory instance
     * @param attribute           The attribute
     *
     * @return The value of the specified attribute
     *
     * @exception BadParameterException Bad parameter value
     * @exception XMLErrorException Error during XML parsing
     */
    public String getFrameworkAttributeValue(String framework,
            int frameworkInstance, String attribute)
        throws BadParameterException,XMLErrorException {

        boolean frameworkFound = false;
        boolean frameworkInstanceFound = false;
        boolean attributeFound = false;

        String attributeValue = null;

        Assert.check(lifeCycleServiceNode!=null);

        // search the specified service among the children nodes
        Node frameworkNode = findNode(lifeCycleServiceNode, framework);

        if (frameworkNode!=null) {

            frameworkFound = true;

            // search the specified factory instance
            Node instance
                = getActualChildNode(frameworkNode, frameworkInstance);

            if (instance != null) {

                frameworkInstanceFound = true;

                // search the specified attribute
                NamedNodeMap attributes = instance.getAttributes();
                Node attribName = attributes.getNamedItem(attribute);

                if (attribName!=null) {
                    // attribute found
                    attributeFound = true;
                    attributeValue = attribName.getNodeValue();
                }

            }

        }

        if (!frameworkFound) {
            throw new BadParameterException(framework + " framework not found");
        }

        if (!frameworkInstanceFound) {
            throw new BadParameterException("frameworkInstance invalid index");
        }

        if (!attributeFound) {
            throw new BadParameterException(attribute + " attribute not found");
        }

        return attributeValue;

    }

    /**
     * Gets the property (the unique attribute of the specified factory)
     * value of the specified factory's attribute corresponding to :
     * - the specified factory type
     * - specified service's attribute
     *
     * @param framework     The factory type
     * @param frameworkInstance The index of the factory instance
     *
     * @return The values of the specified attribute, the property
     * and the attribute in a String table
     *
     * @exception BadParameterException Bad parameter value
     * @exception XMLErrorException Error during XML parsing
     */
    public String[] getFrameworkMultithreadAttribute(String framework,
                                               int frameworkInstance)
        throws BadParameterException,XMLErrorException {

        boolean frameworkFound = false;
        boolean frameworkInstanceFound = false;
        boolean multithreadFound = false;
        boolean policyFound = false;

        String threadingPolicy = null;
        String threadingAttribute = null;
        String attributeValue = null;

        Assert.check(lifeCycleServiceNode!=null);

        // search the specified service among the children nodes
        Node frameworkNode = findNode(lifeCycleServiceNode, framework);

        if (frameworkNode!=null) {

            frameworkFound = true;

            // search the specified factory instance
            Node instance
                = getActualChildNode(frameworkNode, frameworkInstance);

            if (instance!=null) {

                frameworkInstanceFound = true;

                Node multithreadNode = findNode(instance, "multithread");

                if (multithreadNode!=null) {

                    multithreadFound = true;

                    Node policyNode
                        = getActualChildNode(multithreadNode, 0);
                    if (policyNode!=null) {
                        policyFound = true;
                        threadingPolicy = policyNode.getNodeName();

                        // search the unique attribute
                        NamedNodeMap attributes
                            = policyNode.getAttributes();
                        Node attribute = attributes.item(0);

                        if (attribute!=null) {
                            threadingAttribute = attribute.getNodeName();
                            attributeValue = attribute.getNodeValue();
                        } else {
                            // the threading attribute is irrelevant
                            threadingAttribute = "";
                            attributeValue = "";
                        }

                    }

                }

            }

        }

        if (!frameworkFound) {
            throw new BadParameterException(framework + " framework not found");
        }

        if (!frameworkInstanceFound) {
            throw new BadParameterException("frameworkInstance invalid index");
        }

        if (!multithreadFound) {
            throw new BadParameterException(framework + " not multithreaded framework");
        }

        if (!policyFound) {
            throw new BadParameterException("Threading policy: policy not found");
        }

        String[] values = {
            threadingPolicy,
            threadingAttribute,
            attributeValue
        };
        return values;

    }

    /**
     * Sets up the init configuration with the
     * specified XML configuration file
     *
     * @param XMLConfigurationFile the XML configuration file
     *
     * @exception XMLErrorException Error during XML parsing
     */
    protected void readConfigurationFile(String XMLConfigurationFile)
        throws XMLErrorException {

        try {
            parser.parse(XMLConfigurationFile);

            Document document = parser.getDocument();

            Element rootElement = document.getDocumentElement();

            servicesNode = findNode(rootElement, "services");

            lifeCycleServiceNode = findNode(servicesNode, "lifecycle");

        } catch (SAXException saxe) {
            saxe.printStackTrace();
            throw new XMLErrorException(saxe.getMessage());
        } catch (java.io.IOException ioe) {
            ioe.printStackTrace();
            throw new XMLErrorException(ioe.getMessage());
        }

    }

    /**
     * Finds the actual child node that is of the specified element type
     *
     * @param startingNode The node where to find the child
     * @param element      The element type
     *
     * @return a NULL node if childIndex doesn't denote a valid child
     *
     * @exception XMLErrorException Error during XML parsing
     */
    protected Node findNode(Node startingNode, String element)
        throws XMLErrorException {

        Node nodeToFind = null;

        Assert.check(startingNode!=null);

        NodeList children = startingNode.getChildNodes();
        int nbChildren = children.getLength();

        for (int i = 0; i < nbChildren; ++i) {
            Node node = children.item(i);

            if (node.getNodeType() == Node.ELEMENT_NODE) {

                String nodeName = node.getNodeName();

                if (nodeName.equals(element)) {
                    nodeToFind = node;
                    break;
                }

            }

        }

        return nodeToFind;

    }

    /**
     * Returns the actual child node (ELEMENT TYPE) of the specified node.
     *
     * @param node       The parent of the child to get
     * @param childIndex The index of the child to get
     *
     * @return a NULL node if childIndex doesn't denote a valid child
     *
     * @exception XMLErrorException Error during XML parsing
     */
    protected Node getActualChildNode(Node node, int childIndex)
        throws XMLErrorException {

        Node actualChild = null;
        int actualChildIndex = 0;

        Assert.check(node!=null);

        NodeList children = node.getChildNodes();
        int nbChildren = children.getLength();

        for (int i = 0; i < nbChildren; ++i) {

            Node child = children.item(i);

            if (child.getNodeType() == Node.ELEMENT_NODE) {

                if (childIndex != actualChildIndex) {
                    ++actualChildIndex;
                } else {
                   actualChild = child;
                   break;
                }

            }

        }

        return actualChild;

    }

}

