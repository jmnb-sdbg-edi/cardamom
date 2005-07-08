/* =========================================================================== *
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
 * =========================================================================== */

package cdmw.xmlutils.test;


import cdmw.xmlutils.XMLHelper;


import com.thalesgroup.CdmwXML.*;
import com.thalesgroup.CdmwXML.DocumentPackage.SaveException;
import cdmw.xmlutils.XMLHelper;



public class TestClient 
    extends cdmw.testutils.Testable
{
    private static final int POA_THREAD_POOL_SIZE = 10;

    private static final String XML_FILE = "Q-030-file.xml";
    private static String FTP_HOME_DIR;

    private String remoteHostName;
    
    private com.thalesgroup.CdmwXML.XMLParser xmlParser;


    
    public TestClient(String ftpDirPath) {
        this.out = System.out;
        this.name = "XMLService validation tests";
        xmlParser = null;
	FTP_HOME_DIR = ftpDirPath;
    }
   
    void setXmlParser(com.thalesgroup.CdmwXML.XMLParser xmlParser) {
        this.xmlParser = xmlParser;
    }
   
    
    public void run(String[] args) {

        org.omg.CORBA.ORB orb = null;
        
        // parse the args to get the remote hostname
        if (args.length >= 2) {
            this.remoteHostName = args[0];
        } else {
            System.out.println("TestClient must have a remote hostname and the ftp directory " 
                + "as first argument !");
            System.exit(-1);
        }

        try {
            // init orb
            cdmw.orbsupport.StrategyList orbStrategies = 
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);
            // register XML value factories
            XMLHelper.registerValueFactories(orb);
            
            // get rootPOA and activate its POA Manager
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();
            
            // initialise the platform interface
            cdmw.platformmngt.PlatformInterface.setup(orb, args);

            // create TestClientBehaviour
            TestClientBehaviour clientBehaviour =
                new TestClientBehaviour(this, orb);

            // acknowledge the creation of the process
            cdmw.platformmngt.PlatformInterface.acknowledgeCreation(clientBehaviour);

            orb.run();
            //orb.shutdown() is done by the ProcessBehaviour
            orb.destroy();
        
        } catch (Exception ex) {
            if (orb != null) {
                orb.shutdown(false);
                orb.destroy();
            }
            ex.printStackTrace();
        }
    }    
    

    public void doTests() {    
        println("Testing Cdmw XML Service");
	setNbOfRequestedTestOK(39);
        try {
            // load remote file from URL on localhost ftp (xmlServer is remote)
            java.io.File xmlFile = new java.io.File(FTP_HOME_DIR + "/" + XML_FILE);
            if (!xmlFile.canRead()) {
                println("Can't find \"" + xmlFile.toString() + "\". " +
                        "Cdmw XML Service cannot be tested.");
                return;
            }
            
            String xmlUrl = "ftp://" + cdmw.ossupport.OS.getHostname() + 
                            "/" + XML_FILE;
            
            // --- REQ 120 & 123 ---
            println("Load remote XML file: " + xmlUrl);
            DocumentDescription docDescr = xmlParser.parse(xmlUrl);
            Document doc = DocumentHelper.narrow(docDescr.reference());
            succeed();
            
            // --- REQ 122 ---
            println("Get root element");
            ElementDescription rootEltDescr = doc.get_document_element();
            Element rootElt = ElementHelper.narrow(rootEltDescr.reference());
            String rootEltName = XMLHelper.toString(rootEltDescr.name());
            check("Q-030-validation".equals(rootEltName));
        
            println("Get first child element");
            ElementDescription[] childEltsDescr = rootElt.get_child_elements();
            check(childEltsDescr.length == 1);
            Element childElt = ElementHelper.narrow(childEltsDescr[0].reference());
            String childEltName =  XMLHelper.toString(childEltsDescr[0].name());
            check("childElement".equals(childEltName));
            
            println("Get attribute of first child element");
            AttrDescription attrDescr = childElt.get_attribute(
                XMLHelper.toShortArray("test_attr"));
            Attr attr = AttrHelper.narrow(attrDescr.reference());
            String attrValue = XMLHelper.toString(attrDescr.value());
            check("AttrValue".equals(attrValue));
            
            println("Get text of first child element");
            TextDescription[] textsDescr = childElt.get_texts();
            check(textsDescr.length == 1);
            Text text = TextHelper.narrow(textsDescr[0].reference());
            String textValue = XMLHelper.toString(textsDescr[0].data());
            check("TextValue".equals(textValue));
            
            println("Get CDATASection of root element");
            textsDescr = rootElt.get_texts();
            check(textsDescr.length > 0);
            check(textsDescr[0] instanceof CDATASectionDescription);
            CDATASection cdata = null;
            if (textsDescr[0] instanceof CDATASectionDescription) {
                cdata = CDATASectionHelper.narrow((textsDescr[0].reference()));
                String cdataValue = XMLHelper.toString(textsDescr[0].data());
                check("CDATASectionValue".equals(cdataValue));
           }
            
            // --- REQ 125 ---
            println("Get first child element by XPath");
            NodeDescription[] nodes1 = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/childElement"));
            check(nodes1.length == 1);
            Element childElt2 = ElementHelper.narrow(
                ((ElementDescription) nodes1[0]).reference());
            check(childElt2._is_equivalent(childElt));
            
            println("Get attribute of first child element by XPath");
            NodeDescription[] nodes2 = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/childElement/@test_attr"));
            check(nodes2.length == 1);
            Attr attr2 = AttrHelper.narrow(
                ((AttrDescription) nodes2[0]).reference());
            check(attr2._is_equivalent(attr));

            println("Get text of first child element by XPath");
            NodeDescription[] nodes3 = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/childElement/text()"));
            check(nodes3.length == 1);
            Text text2 = TextHelper.narrow(
                ((TextDescription) nodes3[0]).reference());
            check(text2._is_equivalent(text));
            
            println("Get CDATASection of root element by XPath");
            NodeDescription[] nodes4 = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/text()"));
            check(nodes4.length > 0);
            check(nodes4[0] instanceof CDATASectionDescription);
            CDATASection cdata2 = null;
            if (nodes4[0] instanceof CDATASectionDescription) {
                cdata2 = CDATASectionHelper.narrow(
                    ((CDATASectionDescription) nodes4[0]).reference());
                check(cdata2._is_equivalent(cdata));
            }
            
            // --- REQ 124 ---
            println("Close Document");
            doc.close();
            check(xmlFile.delete());

        } catch (DOMException e) {
            e.printStackTrace();
            fail("" + e.code + ": " + e.reason);
        } catch (XPathFormatException e) {
            e.printStackTrace();
            fail(e.reason);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception");
        }

        
        java.util.Properties props = System.getProperties();
        String userDir = props.getProperty("user.dir");

        try {
            // Load local file (on xmlServer host)
            // We assumes that the validation dir has the same path on both hosts,
            // and that the TestClient and the XMLServer have the same working 
            // directory: /*/validation/data
            String xmlFile = userDir + "/Q-030-data/" + XML_FILE;
            
            // --- REQ 120 & 123 ---
            println("Load local XML file");
            DocumentDescription docDescr = xmlParser.parse(xmlFile);
            Document doc = DocumentHelper.narrow(docDescr.reference());
            succeed();
            
            // --- REQ 129 ---
            println("Modify value of an attribute");
            NodeDescription[] nodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/childElement/@test_attr"));
            Attr attr = AttrHelper.narrow(
                ((AttrDescription) nodes[0]).reference());
            attr.set_value(XMLHelper.toShortArray("NewAttrValue"));
            NodeDescription[] newNodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/childElement/@test_attr"));
            String attrValue = XMLHelper.toString(
                ((AttrDescription) newNodes[0]).value());
            check("NewAttrValue".equals(attrValue));
            
            println("Modify value of text");
            nodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/childElement/text()"));
            Text text = TextHelper.narrow(
                ((TextDescription) nodes[0]).reference());
            text.set_data(XMLHelper.toShortArray("NewTextValue"));
            newNodes = doc.select_nodes_from_root(
                 XMLHelper.toShortArray("/Q-030-validation/childElement/text()"));
            String textValue = XMLHelper.toString(
                ((TextDescription) newNodes[0]).data());
            check("NewTextValue".equals(textValue));
                            
            println("Modify value of CDATASection");
            nodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/text()"));
            CDATASection cdata = CDATASectionHelper.narrow(
                ((CDATASectionDescription) nodes[0]).reference());
            cdata.set_data(XMLHelper.toShortArray("NewCDATAValue"));
            newNodes = doc.select_nodes_from_root(
                 XMLHelper.toShortArray("/Q-030-validation/text()"));
            String cdataValue = XMLHelper.toString(
                ((CDATASectionDescription) newNodes[0]).data());
            check("NewCDATAValue".equals(cdataValue));
            
            // --- REQ 127 ---
            println("Create a new element");
            Element newElt = doc.create_element(XMLHelper.toShortArray("newElement"));
            succeed();
            
            println("Create a new attribute");
            Attr newAttr = doc.create_attribute(
                XMLHelper.toShortArray("newAttr"), XMLHelper.toShortArray("newAttrValue"));
            succeed();
            
            println("Create a new text");
            Text newText = doc.create_text_node(XMLHelper.toShortArray("newText"));
            succeed();
            
            println("Create a new CDATASection");
            CDATASection newCdata = doc.create_CDATASection(
                XMLHelper.toShortArray("newCDATA"));
            succeed();
            
            // --- REQ 128 ---
            println("Insert new Element before first child of root element");
            ElementDescription rootEltDescr = doc.get_document_element();
            Element rootElt = ElementHelper.narrow(rootEltDescr.reference());
            ElementDescription[] childEltsDescr = rootElt.get_child_elements();
            int nbElt = childEltsDescr.length;
            Element childElt = ElementHelper.narrow(childEltsDescr[0].reference());
            Node newNode = rootElt.insert_before(newElt, childElt);
            newElt = ElementHelper.narrow(newNode);
            childEltsDescr = rootElt.get_child_elements();
            check(childEltsDescr.length == nbElt+1);
            check("newElement".equals(XMLHelper.toString(childEltsDescr[0].name())));
            
            println("Append new Attribute to new element");
            newNode = newElt.append_child(newAttr);
            //newAttr = AttrHelper.narrow(newNode);
            nodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/newElement/@newAttr"));
            check("newAttrValue".equals(XMLHelper.toString(
                ((AttrDescription) nodes[0]).value())));

            println("Replace new element by new Text");
            newNode = rootElt.replace_child(newText, newElt);
            newText = TextHelper.narrow(newNode);
            nodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray("/Q-030-validation/text()"));
            Node txtNode = nodes[0].reference();
            check("newText".equals(XMLHelper.toString(
                 ((TextDescription) nodes[0]).data())));
            
            println("Remove new Text");
            newNode = rootElt.remove_child(txtNode);
            childEltsDescr = rootElt.get_child_elements();
            check(childEltsDescr.length == nbElt);
            check("childElement".equals(XMLHelper.toString(childEltsDescr[0].name())));

            // --- REQ 121 ---
            String modifiedFilename = userDir + "/Q-030-data/modified.xml";
            println("Save document as " + modifiedFilename);
            java.io.File modifiedFile = new java.io.File(modifiedFilename);
            if (modifiedFile.exists())
                modifiedFile.delete();
            doc.save_as(modifiedFilename);
            
            java.io.File tmpFile = 
                java.io.File.createTempFile("xml_validation_modifiedFile", "xml");
            tmpFile.delete();
            check(!tmpFile.exists());
            rcopyFile(remoteHostName + ":" + modifiedFilename, 
                tmpFile.getAbsolutePath());
            check(tmpFile.isFile());
            tmpFile.delete();
            
            doc.close();
            
        } catch (DOMException e) {
            e.printStackTrace();
            fail("" + e.code + ": " + e.reason);
        } catch (XPathFormatException e) {
            e.printStackTrace();
            fail(e.reason);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception");
        }
        
        try {
            // --- REQ 126 ---
            String newXmlFile = userDir + "/Q-030-data/newDoc.xml";
            println("Create an empty document: " + newXmlFile);
            DocumentDescription docDescr = 
                xmlParser.create_empty_document(newXmlFile,
                    XMLHelper.toShortArray("newXmlFile"), "");
            Document doc = DocumentHelper.narrow(docDescr.reference());
            succeed();
            
            // --- REQ 121 ---
            println("Save new document");
            doc.save();
            java.io.File xmlFile = new java.io.File(newXmlFile);
            
            java.io.File tmpFile = 
                java.io.File.createTempFile("xml_validation_newFile", "xml");
            tmpFile.delete();
            check(!tmpFile.exists());
            rcopyFile(remoteHostName + ":" + newXmlFile, 
                tmpFile.getAbsolutePath());
            check(tmpFile.isFile());
            tmpFile.delete();

            // --- REQ 124 ---
            doc.close();

        } catch (SaveException e) {
            e.printStackTrace();
            fail(e.reason);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception");
        }

    }
    
    
    private void rcopyFile(String source, String destination) 
        throws java.io.IOException, InterruptedException
    {
        cdmw.ossupport.ThreadedProcess tp =
            cdmw.ossupport.OS.createProcess(
                "/usr/bin/rcp " + source + " " + destination);
        tp.waitProcess();
    }
    
    
    public static void main(String[] args) {
        String ftpDirPath = null;
        // get path of FTP dir if specified

        if (args.length > 2)
            ftpDirPath = args[1];

        if (cdmw.platformmngt.PlatformInterface.
                isLaunchedByPlatformManagement(args)) 
        {
            TestClient client = new TestClient(ftpDirPath);
            
            client.run(args);

        } else {
            System.err.println("TestClient should be launched by PlatformManagement.");
        }
    }
    
    
    
}

