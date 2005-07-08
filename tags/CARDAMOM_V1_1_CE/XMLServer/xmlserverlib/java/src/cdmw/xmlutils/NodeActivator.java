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


package cdmw.xmlutils;

import org.w3c.dom.Node;
import org.omg.PortableServer.Servant;

/**
 * This class manages the correspondences between the DOM nodes 
 * and their equivalent CdmwXML node. It owns a table with DOM nodes indexed 
 * with ObjectId. Thus, each DOM node has a corresponding CdmwXML node.
 *
 */
public class NodeActivator {

    /**
     * Table needed to maintain the correspondences 
     * between the DOM nodes and the ObjectId
     */
    private java.util.Hashtable nodesTable = new java.util.Hashtable();
    
    /**
     * POA managing CdmwXMl objects
     */
    private org.omg.PortableServer.POA xmlPoa;

    /**
     * Constructor
     *
     * @param poa Reference to the CdmwXML POA
     */
    public NodeActivator(org.omg.PortableServer.POA poa) {
        this.xmlPoa = poa;
    }
    
    /**
     * Returns the DOM node corresponding to the specified ObjectId
     *
     * @param oid an Object Id 
     * @return The corresponding DOM node if it exists, null otherwise.
     */
    public Node getDomNode(byte[] oid) {
        return (Node) nodesTable.get(new String(oid));
    }
    
    /**
     * Returns the ObjectId corresponding to the specified DOM Node
     *
     * @param domNode a DOM node
     * @return The corresponding ObjectId if it exists, null otherwise.
     */
    public byte[] getObjectId(Node domNode) {
        
        java.util.Enumeration e = nodesTable.keys();
        while ( e.hasMoreElements() ) {
            String key = (String) e.nextElement();
            Node node = (Node) nodesTable.get(key);
            if (node.equals(domNode)) return key.getBytes();
         }
         return null;
        
    }
    
    /**
     * Returns the CdmwXML node corresponding to the specified DOM node.
     *
     * @param domNode a DOM node
     * @return The corresponding CdmwXML node
     */
    public com.thalesgroup.CdmwXML.Node 
        getCdmwNode(Node domNode) {
        
        byte[] oid = getObjectId(domNode);
        org.omg.CORBA.Object obj = null;
        if (oid != null) {
            // we try to retrieve the object
            try {
                obj = this.xmlPoa.id_to_reference(oid);
            } catch(org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
                ona.printStackTrace();
            } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
                wp.printStackTrace();
            }
        }

        // we return the CDMW node
        return com.thalesgroup.CdmwXML.NodeHelper.narrow(obj);
        
    }
    
    /**
     * Activates a new CdmwXML node if it doesn't already exist and stores 
     * the corresponding DOM node in the table with the ObjectId as index.
     *
     * @param node a DOM node
     * @param servant The servant which calls this method
     * @return the newly activated CdmwXMl node
     */
    public com.thalesgroup.CdmwXML.Node 
        activateNode(Node node, Servant servant) {
        
        byte[] oid = nodeToObjectId(node);
        
        // we try to activate the corresponding node
        try {
            this.xmlPoa.activate_object_with_id(oid, servant);
        } catch(org.omg.PortableServer.POAPackage.ServantAlreadyActive saa) {
            //displayNodesTable();
            saa.printStackTrace();
        } catch(org.omg.PortableServer.POAPackage.ObjectAlreadyActive oaa) {
            // the node is already active
            // no need to report this exception
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            //displayNodesTable();
            wp.printStackTrace();
        }
        
        // we try to retrieve the object
        org.omg.CORBA.Object obj = null;
        try {
            obj = this.xmlPoa.id_to_reference(oid);
        } catch(org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            ona.printStackTrace();
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            wp.printStackTrace();
        }

        // we return the CDMW node
        return com.thalesgroup.CdmwXML.NodeHelper.narrow(obj);
        
    }

    /**
     * This objectId must be unique for the node passed as parameter.
     * If this method is invoked more than once for the same node, 
     * it must return the same ObjectId.
     *
     * @param domNode a DOM node
     * @return A generated ObjectId
     */
    protected byte[] nodeToObjectId(Node domNode) {
    
        byte[] oid = getObjectId(domNode);
                
        // the node is not in the table
        if (oid == null) {
            // we generate a new object id
            String size = Integer.toString(nodesTable.size());
            String type = Short.toString(domNode.getNodeType());
            String strOid = String.valueOf(domNode.hashCode()) + size + type;
            oid = strOid.getBytes();
            
            // we put the node in the table
            nodesTable.put(strOid, domNode);
        }
        
        return oid;
        
    }
    
    /**
     * Deactivates all the nodes in the table and removes them.
     */
    protected void deactivateAll() {
        
        try {
            java.util.Enumeration e = this.nodesTable.keys();
            while( e.hasMoreElements() ) {
                String key = (String) e.nextElement();
                this.xmlPoa.deactivate_object(key.getBytes());
                this.nodesTable.remove(key);
            }
        } catch(org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            ona.printStackTrace();
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            wp.printStackTrace();
        }
    
    }

}

