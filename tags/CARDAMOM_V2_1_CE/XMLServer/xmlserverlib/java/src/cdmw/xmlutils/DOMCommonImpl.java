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

import cdmw.xmlutils.XMLHelper;
import cdmw.xmlutils.DocumentDescriptionImpl;
import cdmw.xmlutils.ElementDescriptionImpl;
import cdmw.xmlutils.AttrDescriptionImpl;
import cdmw.xmlutils.TextDescriptionImpl;
import cdmw.xmlutils.CDATASectionDescriptionImpl;

import org.apache.xml.serialize.OutputFormat;
import org.apache.xml.serialize.XMLSerializer;

import org.apache.xpath.XPathAPI;

import com.thalesgroup.CdmwXML.Node;
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.Attr;
import com.thalesgroup.CdmwXML.Text;
import com.thalesgroup.CdmwXML.CDATASection;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.DocumentDescription;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.CDATASectionDescription;

import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.NOT_FOUND_ERR;
import com.thalesgroup.CdmwXML.INVALID_CHARACTER_ERR;
import com.thalesgroup.CdmwXML.NOT_SUPPORTED_ERR;
import com.thalesgroup.CdmwXML.HIERARCHY_REQUEST_ERR;
import com.thalesgroup.CdmwXML.WRONG_DOCUMENT_ERR;
import com.thalesgroup.CdmwXML.INUSE_ATTRIBUTE_ERR;
import com.thalesgroup.CdmwXML.DocumentPackage.SaveException;
import com.thalesgroup.CdmwXML.XPathFormatException;

/**
 * Implementation of all CORBA objects composing the CORBA representation
 * of a DOM tree.
 * This class implements com.thalesgroup.CdmwXMLPrivate.DOMCommonInterface.
 * There is one instance of this class per opened and parsed XML document.
 *
 */
public class DOMCommonImpl
    extends com.thalesgroup.CdmwXMLPrivate.DOMCommonInterfacePOA {

    /**
     * System id used to save the file
     */
    private String systemId;

    /**
     * Instance of the NodeActivator
     */
    private NodeActivator activator;

    /**
     * Reference to the current ORB
     */
    private org.omg.CORBA.ORB orb;

    /**
     * POA managing the CdmwXML objects
     */
    private org.omg.PortableServer.POA xmlPoa;

    /**
     * Constructor.
     *
     * @param orb Reference to the current ORB
     * @param xmlPoa POA managing the CdmwXML objects
     */
    public DOMCommonImpl(
        org.omg.CORBA.ORB orb, org.omg.PortableServer.POA xmlPoa) {
        this.activator = new NodeActivator(xmlPoa);
        this.orb = orb;
        this.xmlPoa = xmlPoa;
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Node/get_description:1.0
    //
    /**
     * Returns a description of the node. This description is the appropriate
     * valuetype:<UL>
     * <LI>a DocumentDescription for a Document node,</LI>
     * <LI>a ElementDescription for an Element node,</LI>
     * <LI>an AttrDescription for an Attr node,</LI>
     * <LI>a TextDescription for a Text node and</LI>
     * <LI>a CDATASectionDescription for a CDATASection node.</LI></UL>
     *
     * @return A Description of this node.
     */
    public NodeDescription get_description() {
      return getDescription(activator.getDomNode(this._object_id()));
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Node/get_parent:1.0
    //
    /**
     * Returns a description of the parent of this node.
     *
     * @return A description of the parent of this node.
     * @exception DOMException If the node has no parent
     * (the node is a Document or a newly created node).
     */
    public NodeDescription get_parent() throws DOMException {

        org.w3c.dom.Node node = activator.getDomNode(this._object_id());
        org.w3c.dom.Node parent = null;

        if (node.getNodeType() == org.w3c.dom.Node.ATTRIBUTE_NODE) {
            parent
                = ((org.w3c.dom.Attr) node).getOwnerElement().getParentNode();
        } else {
            parent = node.getParentNode();
        }

        if (parent != null) {
         NodeDescription description = getDescription(parent);
         if ( description != null) {
            return description;
         } else {
            throw new DOMException(
                    NOT_FOUND_ERR.value, "Node type not supported");
         }
      } else {
            throw new DOMException(
                NOT_FOUND_ERR.value, "Current node has no parent");
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/close:1.0
    //
    /**
     * This method closes the XML document, deallocates the associated DOM tree
     * and destroys every CORBA objects associated to this DOM tree.
     */
    public void close() {
        activator.deactivateAll();
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/save:1.0
    //
    /**
     * Saves the document.
     * If the document is local (the filename is not a remote URL),
     * the XML document is saved with its original filename.
     *
     * @exception SaveException Unable to save the document.
     */
    public void save() throws SaveException {
        // check if SystemId is a local file
        java.net.URL url;
        try {
            url = new java.net.URL(this.systemId);
        } catch (java.net.MalformedURLException e) {
            throw new SaveException("Cannot save document as \""+ this.systemId +
                                    "\". This is a malformed URL !!");
        }
        
        if (url.getProtocol().equals("file")) {
            String filename = url.getFile();
            save_as(filename);
        } else {
            throw new SaveException("Cannot save document as \""+ this.systemId + 
                                    "\". This is not a valid local filename.");
        }
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/save_as:1.0
    //
    /**
     * Saves the document with the specified filename.
     *
     * @param filename New filename of the document.
     * @exception SaveException Unable to save the file.
     */
    public void save_as(String filename) throws SaveException {

        org.w3c.dom.Document doc = getDomDocument();
        try {
            OutputFormat output = new OutputFormat(doc);
            output.setPreserveSpace(true);
            output.setIndenting(true);
            java.io.FileWriter writer = new java.io.FileWriter(filename);
            XMLSerializer serializer = new XMLSerializer(writer, output);
            serializer.asDOMSerializer();
            serializer.serialize(doc);
            writer.flush();
            writer.close();
        } catch (java.io.IOException ioe) {
            throw new SaveException(ioe.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/get_document_element:1.0
    //
    /**
     * This method returns the ElementDescription
     * of the root element of this document.
     *
     * @return A description of the root element of the document.
     */
    public ElementDescription get_document_element() {
        org.w3c.dom.Document doc = getDomDocument();
        org.w3c.dom.Element rootElement = doc.getDocumentElement();
        return createElementDescription(rootElement);
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/select_nodes_from_root:1.0
    //
    /**
     * Uses an XPath query from the root node to select a node list.
     *
     * @param xpath XPath query to be evaluated.
     * @return List of the description of the selected nodes.
     * @exception XPathFormatException Exception thrown during XPath query.
     */
    public NodeDescription[] select_nodes_from_root(short[] xpath)
        throws XPathFormatException {
        try {
            org.w3c.dom.Document doc = getDomDocument();
            org.w3c.dom.NodeList list
                = XPathAPI.selectNodeList(doc, XMLHelper.toString(xpath));
            NodeDescription[] result = new NodeDescription[list.getLength()];
            for (int i=0; i<result.length; i++) {
                result[i] = getDescription(list.item(i));
            }
            return result;
        } catch(javax.xml.transform.TransformerException te) {
            throw new XPathFormatException(te.getMessage());
        }
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/create_element:1.0
    //
    /**
     * Creates an element in this document. The created element is not linked
     * to any other element. In addition, if there are known attributes
     * with default values, Attr nodes representing them are automatically
     * created and attached to the element.
     *
     * @param tagName Name of the element to be created.
     * @return A new Element with the specified tag name.
     * @exception DOMException If tagName contains an illegal character.
     */
    public Element create_element(short[] tagName) throws DOMException {
        try {
            org.w3c.dom.Document doc = getDomDocument();
            org.w3c.dom.Element elem
                = doc.createElement(XMLHelper.toString(tagName));
            return activateElement(elem);
        } catch(org.w3c.dom.DOMException de) {
            throw new DOMException(
                INVALID_CHARACTER_ERR.value, de.getMessage());
        }
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/create_attribute:1.0
    //
    /**
     * Creates an attribute in this document. This attribute is not linked
     * to any other element.
     * It can be set on an Element using the append_child method.
     *
     * @param name name of the attribute
     * @param value value of the attribute
     * @return A new Attr object with specified name and value.
     * @exception DOMException If name contains an illegal character.
     */
    public Attr create_attribute(short[] name, short[] value)
        throws DOMException {
        try {
            org.w3c.dom.Document doc = getDomDocument();
            org.w3c.dom.Attr attr
                = doc.createAttribute(XMLHelper.toString(name));
            attr.setValue(XMLHelper.toString(value));
            return activateAttr(attr);
        } catch(org.w3c.dom.DOMException de) {
            throw new DOMException(
                INVALID_CHARACTER_ERR.value, de.getMessage());
        }
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/create_text_node:1.0
    //
    /**
     * Creates a text in this document.
     * The created text is not linked to any other element.
     *
     * @param data value of the Text
     * @return A new Text object with specified data.
     */
    public Text create_text_node(short[] data) {
        org.w3c.dom.Document doc = getDomDocument();
        org.w3c.dom.Text text = doc.createTextNode(XMLHelper.toString(data));
        return activateText(text);
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Document/create_CDATASection:1.0
    //
    /**
     * Creates a CDATASection in this document.
     * The created CDATASection is not linked to any other element.
     *
     * @param data value of the CDATASection
     * @return A new CDATASection object with specified data.
     * @exception DOMException If this document is an HTML document.
     */
    public CDATASection create_CDATASection(short[] data) throws DOMException {
        try {
            org.w3c.dom.Document doc = getDomDocument();
            org.w3c.dom.CDATASection cdata
                = doc.createCDATASection(XMLHelper.toString(data));
            return activateCDATASection(cdata);
        } catch(org.w3c.dom.DOMException de) {
            throw new DOMException(NOT_SUPPORTED_ERR.value, de.getMessage());
        }
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/insert_before:1.0
    //
    /**
     * Inserts the node newChild before the existing child node refChild.
     * If the newChild is already in the tree, it is first removed.
     *
     * @param newChild node to be inserted
     * @param refChild reference node, i.e. the node before the new node
     * is inserted
     * @return The node being inserted
     * @exception DOMException The following codes could be thrown:<UL>
     * <LI>HIERARCHY_REQUEST_ERR: if the node to be inserted is one
     * of its node's ancestors,</LI>
     * <LI>WRONG_DOCUMENT_ERR: if newChild was created from a different
     * document than the one that created this node</LI>
     * <LI>NOT_FOUND_ERR: if refChild is not a child of this node</LI></UL>
     */
    public Node insert_before(Node newChild, Node refChild)
        throws DOMException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            org.w3c.dom.Node domNew
                = activator.getDomNode(this.xmlPoa.reference_to_id(newChild));
            org.w3c.dom.Node domRef
                = activator.getDomNode(this.xmlPoa.reference_to_id(refChild));
            org.w3c.dom.Node node = elem.insertBefore(domNew, domRef);
            return activator.activateNode(node, this);
        } catch(org.w3c.dom.DOMException de) {
            switch(de.code) {
            case org.w3c.dom.DOMException.HIERARCHY_REQUEST_ERR:
                throw new DOMException(
                    HIERARCHY_REQUEST_ERR.value, de.getMessage());
            case org.w3c.dom.DOMException.WRONG_DOCUMENT_ERR:
                throw new DOMException(
                    WRONG_DOCUMENT_ERR.value, de.getMessage());
            case org.w3c.dom.DOMException.NOT_FOUND_ERR:
                throw new DOMException(NOT_FOUND_ERR.value, de.getMessage());
            default:
                throw new DOMException(de.code, de.getMessage());
            }
        } catch(org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new DOMException((short)0, wa.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new DOMException((short)0, wp.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/replace_child:1.0
    //
    /**
     * Replaces the child node oldChild with newChild in the list of children
     * and returns the oldChild node. If the newChild is already in the tree,
     * it is first removed.
     *
     * @param newChild new node to be put in the child list
     * @param refChild node being replaced in the list
     * @return The node replaced
     * @exception DOMException The following codes could be thrown:<UL>
     * <LI>HIERARCHY_REQUEST_ERR: if the node to be replaced is one
     * of its node's ancestors,</LI>
     * <LI>WRONG_DOCUMENT_ERR: if newChild was created from a different document
     * than the one that created this node</LI>
     * <LI>NOT_FOUND_ERR: if oldChild is not a child of this node</LI></UL>
     */
    public Node replace_child(Node newChild, Node oldChild)
        throws DOMException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            org.w3c.dom.Node domNew
                = activator.getDomNode(this.xmlPoa.reference_to_id(newChild));
            org.w3c.dom.Node domOld
                = activator.getDomNode(this.xmlPoa.reference_to_id(oldChild));
            org.w3c.dom.Node node = elem.replaceChild(domNew, domOld);            
            return activator.activateNode(node, this);
        } catch(org.w3c.dom.DOMException de) {
            switch(de.code) {
            case org.w3c.dom.DOMException.HIERARCHY_REQUEST_ERR:
                throw new DOMException(
                    HIERARCHY_REQUEST_ERR.value, de.getMessage());
            case org.w3c.dom.DOMException.WRONG_DOCUMENT_ERR:
                throw new DOMException(
                    WRONG_DOCUMENT_ERR.value, de.getMessage());
            case org.w3c.dom.DOMException.NOT_FOUND_ERR:
                throw new DOMException(NOT_FOUND_ERR.value, de.getMessage());
            default:
                throw new DOMException(de.code, de.getMessage());
            }
        } catch(org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new DOMException((short)0, wa.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new DOMException((short)0, wp.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/remove_child:1.0
    //
    /**
     * Removes the child node indicated by oldChild from the list of children
     * and returns it.
     *
     * @param oldChild node being removed
     * @return The node removed
     * @exception DOMException If oldChild is not a child of this node.
     */
    public Node remove_child(Node oldChild) throws DOMException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            org.w3c.dom.Node domOld
                = activator.getDomNode(this.xmlPoa.reference_to_id(oldChild));
            org.w3c.dom.Node node = elem.removeChild(domOld);
            return activator.activateNode(node, this);
        } catch(org.w3c.dom.DOMException de) {
            throw new DOMException(NOT_FOUND_ERR.value, de.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new DOMException((short)0, wa.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new DOMException((short)0, wp.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/append_child:1.0
    //
    /**
     * Adds the node newChild to the end of the list of children of this node.
     * If the newChild is already in the tree, it is first removed.
     *
     * @param newChild node to be added.
     * @return The node added.
     * @exception DOMException The following codes could be thrown:<UL>
     * <LI>HIERARCHY_REQUEST_ERR: if this node is of a type that does not allow
     * children of the type of the newChild node, or if the node to append
     * is one of this node's ancestors</LI>
     * <LI>WRONG_DOCUMENT_ERR: if newChild was created from a different document
     * than the one that created this node.</LI><UL/>
     */
    public Node append_child(Node newChild) throws DOMException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            org.w3c.dom.Node domNew = activator.getDomNode(
                this.xmlPoa.reference_to_id(newChild));
            if (newChild.get_description().type() ==
                com.thalesgroup.CdmwXML.ATTRIBUTE_NODE.value) {
                org.w3c.dom.Attr attr = (org.w3c.dom.Attr) domNew;
                elem.setAttributeNode(attr);
                return activator.activateNode(attr, this);
            } else {
                org.w3c.dom.Node node = elem.appendChild(domNew);
                return activator.activateNode(node, this);
            }
        } catch(org.w3c.dom.DOMException de) {
            switch(de.code) {
            case org.w3c.dom.DOMException.HIERARCHY_REQUEST_ERR:
                throw new DOMException(
                    HIERARCHY_REQUEST_ERR.value, de.getMessage());
            case org.w3c.dom.DOMException.WRONG_DOCUMENT_ERR:
                throw new DOMException(
                    WRONG_DOCUMENT_ERR.value, de.getMessage());
            default:
                throw new DOMException(de.code, de.getMessage());
            }
        } catch(org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new DOMException((short)0, wa.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new DOMException((short)0, wp.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/select_nodes:1.0
    //
    /**
     * Uses an XPath from this Element to select a node list. Returns a list
     * of description of the selected nodes.
     *
     * @param xpath The XPath query to be evaluated
     * @return A list of description of selected nodes
     * @exception XPathFormatException Exception thrown during XPath query.
     */
    public NodeDescription[] select_nodes(short[] xpath)
        throws XPathFormatException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            org.w3c.dom.NodeList list
                = XPathAPI.selectNodeList(elem, XMLHelper.toString(xpath));
            NodeDescription[] result = new NodeDescription[list.getLength()];
            for (int i=0; i<result.length; i++) {
                result[i] = getDescription(list.item(i));
            }
            return result;
        } catch(javax.xml.transform.TransformerException te) {
            throw new XPathFormatException(te.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/get_child_elements:1.0
    //
    /**
     * Returns a list containing description of the child elements
     * of this element.
     *
     * @return A list containing description of the child elements.
     */
    public ElementDescription[] get_child_elements() {

        org.w3c.dom.Element elem = getDomElement();
        org.w3c.dom.NodeList list = elem.getChildNodes();
        java.util.ArrayList elemList = new java.util.ArrayList();
        for(int i=0; i<list.getLength(); i++) {
            if (list.item(i).getNodeType() == org.w3c.dom.Node.ELEMENT_NODE) {
                elemList.add(list.item(i));
            }
        }
        ElementDescription[] result = new ElementDescription[elemList.size()];
        for (int j=0; j<result.length; j++) {
            result[j]
                = createElementDescription((org.w3c.dom.Element)elemList.get(j));
        }
        return result;

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/get_texts:1.0
    //
    /**
     * Returns a list containing description of the child texts
     * of this element.
     *
     * @return A list containing description of the child texts.
     */
    public TextDescription[] get_texts() {
        
        org.w3c.dom.Element elem = getDomElement();
        org.w3c.dom.NodeList list = elem.getChildNodes();
        java.util.ArrayList textList = new java.util.ArrayList();
        for(int i=0; i<list.getLength(); i++) {
            short nodeType = list.item(i).getNodeType();
            if (nodeType == org.w3c.dom.Node.TEXT_NODE) {
                TextDescription descr =
                    createTextDescription((org.w3c.dom.Text)list.item(i));
                textList.add(descr);
            } else if (nodeType == org.w3c.dom.Node.CDATA_SECTION_NODE) {
                CDATASectionDescription descr =
                    createCDATASectionDescription((org.w3c.dom.CDATASection)list.item(i));
                textList.add(descr);
            }
        }
        TextDescription[] result = new TextDescription[textList.size()];
        for (int j=0; j<result.length; j++) {
            result[j] = (TextDescription) textList.get(j);
        }
        return result;

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/get_attributes:1.0
    //
    /**
     * Returns a list containing description of the child attributes
     * of this element.
     *
     * @return A list containing description of the child texts.
     */
    public AttrDescription[] get_attributes() {

        org.w3c.dom.Element elem = getDomElement();
        org.w3c.dom.NamedNodeMap map = elem.getAttributes();
        AttrDescription[] result = new AttrDescription[map.getLength()];
        for(int i=0; i<result.length; i++) {
            result[i] = createAttrDescription((org.w3c.dom.Attr) map.item(i));
        }
        return result;

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/get_attribute:1.0
    //
    /**
     * Returns the description of the specified attribute of this element.
     *
     * @param name Name of the required attribute.
     * @return Description of the attribute.
     * @exception DOMException If the element has no attribute with this name.
     */
    public AttrDescription get_attribute(short[] name)
        throws DOMException {

        org.w3c.dom.Element elem = getDomElement();
        org.w3c.dom.Attr attr
            = elem.getAttributeNode(XMLHelper.toString(name));
        if (attr != null) {
            return createAttrDescription(attr);
        } else {
            throw new DOMException(NOT_FOUND_ERR.value, "No attribute " +
                "with name " + XMLHelper.toString(name) + " found");
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/set_attribute:1.0
    //
    /**
     * Sets a new attribute as child of this element.
     * If an attribute with that name is already present in the element,
     * its value is changed to be that of the value parameter.
     *
     * @param name Name of the attribute
     * @param value Value of the attribute
     * @exception DOMException If the specified name contains
     * an illegal character.
     */
    public void set_attribute(short[] name, short[] value)
        throws DOMException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            String stringName = XMLHelper.toString(name);
            String stringValue = XMLHelper.toString(value);
            elem.setAttribute(stringName, stringValue);
        } catch(org.w3c.dom.DOMException de) {
            throw new DOMException(
                INVALID_CHARACTER_ERR.value, de.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Element/set_attribute_node:1.0
    //
    /**
     * Adds a new attribute node to the current element.
     *
     * @param newAttr The new attribute to add.
     * @return If the newAttr attribute replaces an existing attribute,
     * the replaced Attr node is returned, otherwise null is returned.
     * @exception DOMException The following codes could be thrown:<UL>
     * <LI>INUSE_ATTRIBUTE_ERR: Raised if newAttr is already an attribute
     * of another Element object. The DOM user must explicitly clone Attr nodes
     * to re-use them in other elements</LI>
     * <LI>WRONG_DOCUMENT_ERR: Raised if newAttr was created from a different
     * document than the one that created the element.</LI>
     * <LI>other values: CORBA Exceptions.</LI><UL/>
     */
    public AttrDescription set_attribute_node(Attr newAttr)
        throws DOMException {

        try {
            org.w3c.dom.Element elem = getDomElement();
            org.w3c.dom.Attr attr = (org.w3c.dom.Attr) activator.getDomNode(
                this.xmlPoa.reference_to_id(newAttr));
            org.w3c.dom.Attr oldAttr = elem.setAttributeNode(attr);
            if (oldAttr != null) {
                return createAttrDescription(oldAttr);
            } else {
                return null;
            }
        } catch(org.w3c.dom.DOMException de) {
            switch(de.code) {
            case org.w3c.dom.DOMException.WRONG_DOCUMENT_ERR:
                throw new DOMException(
                    WRONG_DOCUMENT_ERR.value, de.getMessage());
            case org.w3c.dom.DOMException.INUSE_ATTRIBUTE_ERR:
                throw new DOMException(
                    INUSE_ATTRIBUTE_ERR.value, de.getMessage());
            default:
                throw new DOMException(de.code, de.getMessage());
            }
        } catch(org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new DOMException((short)0, wa.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new DOMException((short)0, wp.getMessage());
        }

    }

    //
    // IDL:thalesgroup.com/CdmwXML/Attr/set_value:1.0
    //
    /**
     * Modify the value of the attribute.
     *
     * @param value New value of the attribute.
     */
    public void set_value(short[] value) {
        org.w3c.dom.Attr attr = getDomAttr();
        attr.setValue(XMLHelper.toString(value));
    }

    //
    // IDL:thalesgroup.com/CdmwXML/Text/set_data:1.0
    //
    /**
     * Modify the data of the text node.
     *
     * @param data New data of the text node.
     */
    public void set_data(short[] data) {
        org.w3c.dom.Text text = getDomText();
        try {
            text.setData(XMLHelper.toString(data));
        } catch(org.w3c.dom.DOMException de) {
            // should not happen
        }
    }

    /**
     * Associates a DOM Document with this implementation.
     * This method should be invoked only once, just after constructor.
     * It activates the CdmwXML Document object and returns
     * a DocumentDescription of the specified document.
     *
     * @param domDoc DOM Document to be associated
     * @param systemId System Id to enable save feature.
     * @return A DocumentDescription of the specified document
     * containing a reference to the newly activated CdmwXML Document.
     */
    public DocumentDescription setDomDocument(
        org.w3c.dom.Document domDoc, String systemId) {
        this.systemId = systemId;
        return createDocumentDescription(domDoc);
    }

    /**
     * Creates a DocumentDescription corresponding to the given DOM document.
     *
     * @param domDocument Reference used to create the DocumentDescription.
     */
    protected DocumentDescription createDocumentDescription(
        org.w3c.dom.Document domDocument) {
        Document cdmwDocument = activateDocument(domDocument);
        short[] doctype;
        if (domDocument.getDoctype() != null) {
            doctype = XMLHelper.toShortArray(domDocument.getDoctype().getName());
        } else {
            doctype = new short[0];
        }
        return new DocumentDescriptionImpl(cdmwDocument, doctype);
    }

    /**
     * Creates a ElementDescription corresponding to the given DOM element.
     *
     * @param domElement Reference used to create the ElementDescription.
     */
    protected ElementDescription createElementDescription(
        org.w3c.dom.Element domElement) {
        Element cdmwElement = activateElement(domElement);
        short[] name = XMLHelper.toShortArray(domElement.getTagName());
        return new ElementDescriptionImpl(cdmwElement, name);
    }

    /**
     * Creates a AttrDescription corresponding to the given DOM attribute.
     *
     * @param domAttr Reference used to create the AttrDescription.
     */
    protected AttrDescription createAttrDescription(
        org.w3c.dom.Attr domAttr) {
        Attr cdmwAttr = activateAttr(domAttr);
        short[] name = XMLHelper.toShortArray(domAttr.getName());
        short[] value = XMLHelper.toShortArray(domAttr.getValue());
        return new AttrDescriptionImpl(cdmwAttr, name, value);
    }

    /**
     * Creates a TextDescription corresponding to the given DOM text.
     *
     * @param domText Reference used to create the TextDescription.
     */
    protected TextDescription createTextDescription(
        org.w3c.dom.Text domText) {
        Text cdmwText = activateText(domText);
        short[] data = new short[0];
        try {
            data = XMLHelper.toShortArray(domText.getData());
        } catch(org.w3c.dom.DOMException de) {}
        return new TextDescriptionImpl(cdmwText, data);
    }

    /**
     * Creates a CDATASectionDescription corresponding
     * to the given DOM CDATA section.
     *
     * @param domCDATA Reference used to create the CDATASectionDescription.
     */
    protected CDATASectionDescription createCDATASectionDescription(
        org.w3c.dom.CDATASection domCDATA) {
        CDATASection cdmwCDATASection = activateCDATASection(domCDATA);
        short[] data = new short[0];
        try {
            data = XMLHelper.toShortArray(domCDATA.getData());
        } catch(org.w3c.dom.DOMException de) {}
        return new CDATASectionDescriptionImpl(cdmwCDATASection, data);
    }

    /**
     * Returns the current DOM document
     *
     * @return The current DOM document.
     */
    private org.w3c.dom.Document getDomDocument() {
        try {
            return (org.w3c.dom.Document)
                activator.getDomNode(this._object_id());
        } catch(ClassCastException cce) {
            return null;
        }
    }

    /**
     * Returns the current DOM element
     *
     * @return The current DOM element.
     */
    private org.w3c.dom.Element getDomElement() {
        try {
            return (org.w3c.dom.Element)
                activator.getDomNode(this._object_id());
        } catch(ClassCastException cce) {
            return null;
        }
    }

    /**
     * Returns the current DOM attribute
     *
     * @return The current DOM attribute.
     */
    private org.w3c.dom.Attr getDomAttr() {
        try {
            return (org.w3c.dom.Attr)
                activator.getDomNode(this._object_id());
        } catch(ClassCastException cce) {
            return null;
        }
    }

    /**
     * Returns the current DOM text
     *
     * @return The current DOM text.
     */
    private org.w3c.dom.Text getDomText() {
        try {
            return (org.w3c.dom.Text)
                activator.getDomNode(this._object_id());
        } catch(ClassCastException cce) {
            return null;
        }
    }

    /**
     * Returns the current DOM CDATA section
     *
     * @return The current DOM CDATA section.
     */
    private org.w3c.dom.CDATASection getDomCDATASection() {
        try {
            return (org.w3c.dom.CDATASection)
                activator.getDomNode(this._object_id());
        } catch(ClassCastException cce) {
            return null;
        }
    }

    /**
     * Activates the node passed in argument
     * and returns the corresponding CDMW document.
     *
     * @param node Node to activate
     * @return The CDMW node corresponding to the node passed in argument.
     */
    private Document activateDocument(org.w3c.dom.Node node) {
        Node cdmwNode = activator.activateNode(node, this);
        return com.thalesgroup.CdmwXML.DocumentHelper.narrow(cdmwNode);
    }

    /**
     * Activates the node passed in argument
     * and returns the corresponding CDMW element.
     *
     * @param node Node to activate
     * @return The CDMW node corresponding to the node passed in argument.
     */
    private Element activateElement(org.w3c.dom.Node node) {
        Node cdmwNode = activator.activateNode(node, this);
        return com.thalesgroup.CdmwXML.ElementHelper.narrow(cdmwNode);
    }

    /**
     * Activates the node passed in argument
     * and returns the corresponding CDMW attribute.
     *
     * @param node Node to activate
     * @return The CDMW node corresponding to the node passed in argument.
     */
    private Attr activateAttr(org.w3c.dom.Node node) {
        Node cdmwNode = activator.activateNode(node, this);
        return com.thalesgroup.CdmwXML.AttrHelper.narrow(cdmwNode);
    }

    /**
     * Activates the node passed in argument
     * and returns the corresponding CDMW text.
     *
     * @param node Node to activate
     * @return The CDMW node corresponding to the node passed in argument.
     */
    private Text activateText(org.w3c.dom.Node node) {
        Node cdmwNode = activator.activateNode(node, this);
        return com.thalesgroup.CdmwXML.TextHelper.narrow(cdmwNode);
    }

    /**
     * Activates the node passed in argument
     * and returns the corresponding CDMW CDATA section.
     *
     * @param node Node to activate
     * @return The CDMW node corresponding to the node passed in argument.
     */
    private CDATASection activateCDATASection(org.w3c.dom.Node node) {
        Node cdmwNode = activator.activateNode(node, this);
        return com.thalesgroup.CdmwXML.CDATASectionHelper.narrow(cdmwNode);
    }

    /**
     * Returns the description corresponding to the given node.
     *
     * @param node DOM node from which the description is returned.
     */
    private NodeDescription getDescription(org.w3c.dom.Node node) {

        switch(node.getNodeType()) {
            case org.w3c.dom.Node.ATTRIBUTE_NODE:
                return createAttrDescription((org.w3c.dom.Attr)node);

            case org.w3c.dom.Node.CDATA_SECTION_NODE:
                return createCDATASectionDescription(
                    (org.w3c.dom.CDATASection)node);

            case org.w3c.dom.Node.DOCUMENT_NODE:
                return createDocumentDescription((org.w3c.dom.Document)node);

            case org.w3c.dom.Node.ELEMENT_NODE:
                return createElementDescription((org.w3c.dom.Element)node);

            case org.w3c.dom.Node.TEXT_NODE:
                return createTextDescription((org.w3c.dom.Text)node);

            default:
                return null;
        }

    }

}

