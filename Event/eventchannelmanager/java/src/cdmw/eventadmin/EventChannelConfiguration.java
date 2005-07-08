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


package cdmw.eventadmin;

import cdmw.common.Assert;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import org.apache.xerces.parsers.DOMParser;

import org.xml.sax.SAXException;
import org.xml.sax.ErrorHandler;
import org.w3c.dom.DOMException;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This class serves as a facility for accessing to configuration information
 * that is provided in an XML file and that is needed by the CDMW init.
 *
 */
public class EventChannelConfiguration {

    /**
     * rootElement Name
     */
    public static final String CONF_ROOT_ELEMENT_NAME 
        = "EventChannelsConfiguration";
    
    /**
     * The DOM parser
     */ 
    protected DOMParser parser;

    /**
     * The DOM error handler
     */ 
    protected ErrorHandler errorReporter;


    /**
     * The Parent Node for all EventChannelProfile nodes. This node
     * is the starting point for the getEventChannelName and
     * getEventChannelProperties methods.
     */ 
    protected Node eventChannelConfigurationNode;

    /**
     * Sets up the init configuration with the
     * specified XML configuration file
     * 
     * @param XMLConfigurationFile the XML configuration file
     * @param doValidate Validation option for the XML file
     * @exception InternalErrorException Internal error
     * @exception ConfigurationException Error in the configuration file
     */ 
    public static EventChannelConfiguration 
        readConfiguration(String XMLConfigurationFile,boolean doValidate)
        throws InternalErrorException, ConfigurationException {
        
        EventChannelConfiguration conf 
            = new EventChannelConfiguration(doValidate);
        conf.readConfigurationFile(XMLConfigurationFile);
        return conf;
        
    }

    /**
     * Returns the event channel name at the eventChannelIndex index 
     *
     * @param eventChannelIndex The index of the event channel name to get
     * @exception BadParameterException if no name at index
     * @exception XMLErrorException if an error occured during parsing
     * @return The event channel name at eventChannelIndex index
     */ 
    public String getNextEventChannelName(int eventChannelIndex)
        throws BadParameterException, XMLErrorException {
        
        String eventChannelName = "";
        boolean eventChannelProfileNodeFound = false;
        boolean eventChannelNameFound = false;
        Node eventChannelProfileNode;

        Assert.check(eventChannelConfigurationNode!=null);

        try {
            eventChannelProfileNode 
                = getActualChildNode(eventChannelConfigurationNode, 
                                                    eventChannelIndex);

            if (eventChannelProfileNode!=null) {
                eventChannelProfileNodeFound = true;

                // search the event channel name
                Node eventChannelNameNode 
                    = findNode(eventChannelProfileNode, "ChannelName");

                if (eventChannelNameNode!=null) {
                    // attribute found
                    eventChannelNameFound = true;
                    eventChannelName = getNodeValue(eventChannelNameNode);
                }

          }

        } catch (DOMException de) {
            throw new XMLErrorException(de.getMessage());
        }

        if (!eventChannelProfileNodeFound) {
            throw new BadParameterException(
                "Event channel profile not found for channel", 
                String.valueOf(eventChannelIndex));
        }

        if (!eventChannelNameFound) {
            throw new BadParameterException(
                "Event channel name not found", 
                String.valueOf(eventChannelIndex));
        }
        
           return eventChannelName;
        
    }

    /**
     * Returns the number of event channel profile
     *
     * @exception XMLErrorException if an error occured during parsing
     * @return The number of event channel profile
     */
    public int getNbEventChannelProfile() throws XMLErrorException {
    
        Assert.check(eventChannelConfigurationNode!=null);

        int nbProfile = 0;

        try {
           NodeList children = eventChannelConfigurationNode.getChildNodes();
           int nbChildren = children.getLength();

           for (int i = 0; i < nbChildren; ++i) {
                Node child = children.item(i);

                if (child.getNodeType() == Node.ELEMENT_NODE) {
                    // this is a profile!
                    nbProfile++;
                }
            }
        } catch (DOMException de) {
            throw new XMLErrorException(de.getMessage());
        }
        
        return nbProfile;
    
    }

    /**
     * Returns the profile of an event channel 
     *
     * @param channelName The name of the event channel
     * @exception BadParameterException if no event channel profile found
     * @exception XMLErrorException if an error occured during parsing
     * @return a NULL object if there is no profile for channelName event channel
     * otherwise a list of channel property as a Vector object
     */ 
    public java.util.Vector getEventChannelProfileValue(String channelName)
        throws BadParameterException, XMLErrorException {
        
        java.util.Vector channelProfile = null;
        String currentChannelName;
        boolean eventchannelProfileFound = false;

        // get the number of Event channel profile in XML doc
        NodeList profiles = eventChannelConfigurationNode.getChildNodes();
        int nbProfiles = profiles.getLength();

        try {
            int i = 0;
            // search for the right event channel profile
            for ( ; i<nbProfiles; i++) {
                currentChannelName = getNextEventChannelName(i);

                if ( currentChannelName.equals(channelName) ) {
                    // eventchannelProfile found
                    eventchannelProfileFound = true;
                    break;
                }
            }

            if (eventchannelProfileFound) {
                Node eventChannelProfile 
                    = getActualChildNode(eventChannelConfigurationNode,i);

                Node propertyNode = findNode(eventChannelProfile, "Property");
                Node name;  // property name
                Node value; // property value

                com.thalesgroup.CdmwEventAdmin.EventChannelProperty property
                    = null;
                channelProfile = new java.util.Vector();

                if (propertyNode!=null) {
                
                    do {
                        name = findNode(propertyNode, "Name");
                        value = findNode(propertyNode,"Value");

                        if ( (name!=null) && (value!=null) ) {
                            property = new com.thalesgroup.CdmwEventAdmin
                                .EventChannelProperty(getNodeValue(name),
                                                        getNodeValue(value));
                        }

                        channelProfile.add(property);

                        // find next property node
                        do {
                            propertyNode = propertyNode.getNextSibling();
                        } while ((propertyNode!=null) &&
                            (propertyNode.getNodeType() != Node.ELEMENT_NODE));
                                 
                    } while (propertyNode!=null);
                }
            }
        } catch (DOMException de) {
            throw new XMLErrorException(de.getMessage());
        }

        if (!eventchannelProfileFound) {
            // eventchannelProfile not found
            throw new BadParameterException( channelName, 
                " event channel profile not found");
        }
        
        return channelProfile;
        
    }

    /**
     * Constructs a EventChannelConfiguration
     * by initializing the underlying XML parser.
     *
     * @param doValidate Do the validation of the XML file or not
     * @exception InternalErrorException Internal error
     */
    protected EventChannelConfiguration(boolean doValidate)
        throws InternalErrorException {
    
        try {
            //
            //  Create our parser, then attach an error handler to the parser.
            //  The parser will call back to methods of the ErrorHandler if it
            //  discovers errors during the course of parsing the XML document.
            //
            this.parser = new DOMParser();

            if (doValidate) { // TODO: Check exceptions to be thrown...
                parser.setFeature(
                    "http://apache.org/xml/features/validation/schema", true );
                parser.setFeature( 
                    "http://xml.org/sax/features/namespaces", false );
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
     * @param XMLConfigurationFile Name of the XML configuration file
     * @exception XMLErrorException If an error occured during parsing
     */ 
    protected void readConfigurationFile(String XMLConfigurationFile)
        throws XMLErrorException {
        
        try {
            if (parser!=null) {
                parser.parse(XMLConfigurationFile);

                org.w3c.dom.Document document = parser.getDocument();
                org.w3c.dom.Element rootElement 
                    = document.getDocumentElement();

                    if (rootElement!=null) {
                        String nodeName = rootElement.getNodeName();
                        if (nodeName.equals(CONF_ROOT_ELEMENT_NAME)) {
                            eventChannelConfigurationNode = rootElement;
                        } else {
                            throw new XMLErrorException("Invalid root element in xml file");
                        } 
                    } else {
                        throw new XMLErrorException("Root element missing in xml file");
                    }
            }
        } catch (DOMException de) {
            throw new XMLErrorException(de.getMessage());
        } catch (SAXException se) {
            throw new XMLErrorException(se.getMessage());
        } catch (java.io.IOException ioe) {
            throw new XMLErrorException(ioe.getMessage());
        }
        
    }

    /**
     * Finds the actual child node that is of the specified element type
     *
     * @param startingNode The node where to find the child
     * @param element The element type
     * @return a NULL node if childIndex doesn't denote a valid child
     * otherwise the child node of the specified element type
     */ 
    protected Node findNode(Node startingNode, String element) {
        
        Node nodeToFind = null;
        Assert.check(startingNode!=null);

        NodeList children = startingNode.getChildNodes();
        int nbChildren = children.getLength();

        for (int i = 0; i < nbChildren; ++i) {

            Node node = children.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE) {

                if ( node.getNodeName().equals(element) ) {
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
     * @param node The parent of the child to get
     * @param childIndex The index of the child to get
     * @return a NULL node if childIndex doesn't denote a valid child
     * otherwise the actual child node
     */ 
    protected Node getActualChildNode(Node node, int childIndex) {
        
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

    /**
     * Returns the text value of the specified node (ELEMENT TYPE).
     *
     * @param node The node containing a text node
     * @return The string value of the spesified node
     * @exception XMLErrorException If the specified node 
     * doesn't contain a text Node
     */ 
    protected String getNodeValue(Node node) throws XMLErrorException {
          
        String nodeValue = "";

        Assert.check(node!=null);

        Node textNode = node.getFirstChild();

        if (textNode.getNodeType() == Node.TEXT_NODE) {
            try {
                nodeValue = textNode.getNodeValue();
            } catch(DOMException de) {
                throw new XMLErrorException(de.getMessage());
            }
        } else {
            throw new XMLErrorException("No value for the node " 
                                            + node.getNodeName());
        }
              
        return nodeValue;
          
    }

}


