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


package cdmw.ccm.assembly;

import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;

import cdmw.ccm.assembly.AssemblyImpl;
import cdmw.ccm.assembly.HostCollocation;
import cdmw.ccm.assembly.ComponentServer;
import cdmw.ccm.assembly.Container;
import cdmw.ccm.assembly.Home;
import cdmw.ccm.assembly.Component;
import cdmw.ccm.assembly.ObjectRefLoader;
import com.thalesgroup.CdmwDeployment.ComponentKindValue;

import cdmw.xmlutils.XMLParserImpl;
import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;

import org.omg.Components.CreateFailure;


public class ObjectRefLoaderTest
    extends cdmw.testutils.Testable
{

    private static final int POA_THREAD_POOL_SIZE = 10;

    private static com.thalesgroup.CdmwXML.XMLParser parser = null;
    private static final String DTD_DIR = ".";

    private static final String XML_FILENAME = "./ObjectRefLoaderTest/test.cad";

    private static final String HOME_1_ID = "Home1";
    private static final String HOME_2_ID = "Home2";
    private static final String COMP_1_ID = "Component1";
    private static final String COMP_2_ID = "Component2";

    private static final String NAMING_SERVICE_REGISTRATION_NAME = "ComponentName";
    private static final String STRINGIFIED_OBJECT_REF = "corbaloc::myhost:5000/MyObject";
    private static final String HOME_FINDER_REGISTRATION_NAME = "HomeName";
    private static final String PROVIDES_PORT_NAME = "providesPortTest";

    public ObjectRefLoaderTest()
    {
        this.out = System.out;
        this.name = "ObjectRefLoader tests";
    }


    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(9);
        
        // open XML test document
        Element rootElt;
        try {

            java.io.File xmlFile = new java.io.File(XML_FILENAME);
            com.thalesgroup.CdmwXML.DocumentDescription docDescr =
                parser.parse(xmlFile.getAbsolutePath());
            com.thalesgroup.CdmwXML.Document doc =
                com.thalesgroup.CdmwXML.DocumentHelper.narrow(
                    docDescr.reference());
            ElementDescription eltDescr = doc.get_document_element();
            rootElt = ElementHelper.narrow(eltDescr.reference());

        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            fail("Error while parsing " + XML_FILENAME + " : " + e.reason);
            return;
        }

        // create stub AssemblyImpl
        AssemblyImpl assembly = new AssemblyImpl(null);

        // create foo HostCollocation, ComponentServer and Container
        HostCollocation fooHost = new HostCollocation("fooHost", assembly, "fooDest");
        ComponentServer fooServer;
        try {
            fooServer = new ComponentServer("fooServer", fooHost, "fooProcName@fooAppliName");
        } catch (CreateFailure e) {
            fail(e.getMessage());
            return;
        }
        Container fooContainer = new Container("fooContainer", fooServer, ComponentKindValue.SESSION);

        // fill AssemblyImpl with homes and components
        Home h1 = new Home(HOME_1_ID, fooContainer, "", "");
        Home h2 = new Home(HOME_2_ID, fooContainer, "", "");
        SingleComponent c1 = new SingleComponent(COMP_1_ID, h1);
        SingleComponent c2 = new SingleComponent(COMP_2_ID, h2);


        //
        // test loadComponentinstantiationref()
        //
        println("Test loadComponentinstantiationref()");

        // Get all <componentinstantiationref> elements of
        // "test_loadComponentinstantiationref*" <connectinterface>.
        // First should be OK, second should not correspond to a component in cad
        try {
            NodeDescription[] nodes = rootElt.select_nodes(XMLHelper.toShortArray(
                "//connectinterface[starts-with(@id,\"test_loadComponentinstantiationref\")]//componentinstantiationref"));

            if (nodes.length < 2) {
                fail("XML test file " + XML_FILENAME +
                     " is not appropriate for test of loadComponentinstantiationref()");
                return;
            }

            try {
                Element elt = ElementHelper.narrow(nodes[0].reference());
                ComponentInstantiationRef ref =
                    ObjectRefLoader.loadComponentinstantiationref(elt, assembly);

                if (ref.getComponent() == c1) {
                    succeed();
                } else {
                    fail("componentinstantiationref doesn't correspond to component " +
                         COMP_1_ID);
                }
            } catch (CreateFailure e) {
                fail(e.getMessage());
            }

            try {
                Element elt = ElementHelper.narrow(nodes[1].reference());
                ComponentInstantiationRef ref =
                    ObjectRefLoader.loadComponentinstantiationref(elt, assembly);

                fail("componentinstantiationref found : " + ref.getComponent().getId());
            } catch (CreateFailure e) {
                succeed();
            }

        } catch (com.thalesgroup.CdmwXML.XPathFormatException e) {
            fail(e.reason);
            return;
        }


        //
        // test loadHomeplacementref()
        //

        println("Test loadHomeplacementref()");
        // Get all <homeplacementref> elements of "test_loadHomeplacementref*" <connectinterface>.
        // First should be OK, second should not correspond to a home in cad
        try {
            NodeDescription[] nodes = rootElt.select_nodes(XMLHelper.toShortArray(
                "//connectinterface[starts-with(@id,\"test_loadHomeplacementref\")]//homeplacementref"));

            if (nodes.length < 2) {
                fail("XML test file " + XML_FILENAME +
                     " is not appropriate for test of loadHomeplacementref()");
                return;
            }

            try {
                Element elt = ElementHelper.narrow(nodes[0].reference());
                HomePlacementRef ref =
                    ObjectRefLoader.loadHomeplacementref(elt, assembly);

                if (ref.getHome() == h1) {
                    succeed();
                } else {
                    fail("homeplacementref doesn't correspond to component " +
                         HOME_1_ID);
                }
            } catch (CreateFailure e) {
                fail(e.getMessage());
            }

            try {
                Element elt = ElementHelper.narrow(nodes[1].reference());
                HomePlacementRef ref =
                    ObjectRefLoader.loadHomeplacementref(elt, assembly);

                fail("homeplacementref found : " + ref.getHome().getId());
            } catch (CreateFailure e) {
                succeed();
            }

        } catch (com.thalesgroup.CdmwXML.XPathFormatException e) {
            fail(e.reason);
            return;
        }

        //
        // test loadFindby()
        //
        println("Test loadFindby()");

        // Get all <findby> elementsof "test_loadFindby*" <connectinterface>.
        // First should be a <findby><nameingservice>,
        // Second should be a <findby><stringifiedobject>,
        // Third should be a <findby><homefinder>
        try {
            NodeDescription[] nodes = rootElt.select_nodes(XMLHelper.toShortArray(
                "//connectinterface[starts-with(@id,\"test_loadFindby\")]//findby"));

            if (nodes.length < 3) {
                fail("XML test file " + XML_FILENAME +
                     " is not appropriate for test of loadFindby()");
                return;
            }

            try {
                Element elt = ElementHelper.narrow(nodes[0].reference());
                FindbyNamingService ref =
                    (FindbyNamingService) ObjectRefLoader.loadFindby(elt);

                if (NAMING_SERVICE_REGISTRATION_NAME.equals(ref.getName())) {
                    succeed();
                } else {
                    fail("<findby><namingservice> has wrong name: " +
                         ref.getName() + " instead of " +
                         NAMING_SERVICE_REGISTRATION_NAME);
                }
            } catch (CreateFailure e) {
                fail(e.getMessage());
            } catch (ClassCastException e) {
                fail("Result of first loadFindby is not a FindbyNamingService !");
            }

            try {
                Element elt = ElementHelper.narrow(nodes[1].reference());
                FindbyStringifiedObject ref =
                    (FindbyStringifiedObject) ObjectRefLoader.loadFindby(elt);

                if (STRINGIFIED_OBJECT_REF.equals(ref.getStringifiedObject())) {
                    succeed();
                } else {
                    fail("<findby><stringifiedobject> has wrong ref: " +
                         ref.getStringifiedObject() + " instead of " +
                         STRINGIFIED_OBJECT_REF);
                }
            } catch (CreateFailure e) {
                fail(e.getMessage());
            } catch (ClassCastException e) {
                fail("Result of second loadFindby is not a FindbyStringifiedObject !");
            }

            try {
                Element elt = ElementHelper.narrow(nodes[2].reference());
                FindbyHomeFinder ref =
                    (FindbyHomeFinder) ObjectRefLoader.loadFindby(elt);

                if (HOME_FINDER_REGISTRATION_NAME.equals(ref.getHomeName())) {
                    succeed();
                } else {
                    fail("<findby><homefinder> has wrong name: " +
                         ref.getHomeName() + " instead of " +
                         HOME_FINDER_REGISTRATION_NAME);
                }
            } catch (CreateFailure e) {
                fail(e.getMessage());
            } catch (ClassCastException e) {
                fail("Result of third loadFindby is not a FindbyHomeFinder !");
            }

        } catch (com.thalesgroup.CdmwXML.XPathFormatException e) {
            fail(e.reason);
            return;
        }


        //
        // test loadFindby()
        //
        println("Test loadProvidesport()");

        // Get all <providesport> elements of "test_loadProvidesport*" <connectinterface>.
        // Should be only one and have a <componentinstantiationref> child referencing Component2
        try {
            NodeDescription[] nodes = rootElt.select_nodes(XMLHelper.toShortArray(
                "//connectinterface[starts-with(@id,\"test_loadProvidesport\")]//providesport"));

            if (nodes.length < 1) {
                fail("XML test file " + XML_FILENAME +
                     " is not appropriate for test of loadProvidesport()");
                return;
            }

            try {
                Element elt = ElementHelper.narrow(nodes[0].reference());
                FacetRef ref =
                    ObjectRefLoader.loadProvidesport(elt, assembly);

                if (PROVIDES_PORT_NAME.equals(ref.getProvidesId())) {
                    succeed();
                } else {
                    fail("ProvidesId found is not expected : " +
                         ref.getProvidesId() + " instead of " + PROVIDES_PORT_NAME);
                }

                ComponentInstantiationRef compRef =
                    (ComponentInstantiationRef) ref.getComponentRef();
                if (compRef.getComponent() == c2) {
                    succeed();
                } else {
                    fail("The component referenced by <providesport> is not " +
                         c2.getId() + ". It's " + compRef.getComponent().getId());
                }

            } catch (CreateFailure e) {
                fail(e.getMessage());
            } catch (ClassCastException e) {
                // cast of ref.getObjectRef() into ComponentInstantiationRef failed
                fail("The component referenced by <providesport> is not " +
                     "a <componentinstantiationref>");
            }

        } catch (com.thalesgroup.CdmwXML.XPathFormatException e) {
            fail(e.reason);
            return;
        }
    }


    public static void registerNamingServiceStub(org.omg.CORBA.ORB orb) {
        NamingContextExtStub stub = new NamingContextExtStub();
        org.omg.CosNaming.NamingContextExt naming =
            stub._this(orb);
        ORBSupport.registerInitialReference(orb, "NameService", naming);
    }

    public static void registerHomeFinderStub(org.omg.CORBA.ORB orb) {
        HomeFinderStub stub = new HomeFinderStub();
        org.omg.Components.HomeFinder finder =
            stub._this(orb);
        ORBSupport.registerInitialReference(orb, "HomeFinder", finder);
    }




    public static void main(String[] args) {

        org.omg.CORBA.ORB orb = null;
        try {
            // init ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);

            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // register XML value factories
            XMLHelper.registerValueFactories(orb);

            // create XML Parser object
            String[] parserArgs = {"--validate",
                                   "--doctypedir=" + DTD_DIR};
            parser = XMLParserImpl.init(orb, rootPOA, parserArgs);

            // register NamingService stub
            registerNamingServiceStub(orb);

            // register HomeFinder stub
            registerHomeFinderStub(orb);

            // init AssemblyFactoryImpl
            AssemblyFactoryImpl.init(orb, rootPOA, rootPOA, null, parser, null);

            // run test
            ObjectRefLoaderTest test = new ObjectRefLoaderTest();
            test.start();

        } catch (Exception e) {
            e.printStackTrace();
        }

        // destroy ORB
        if (orb != null) {
            orb.destroy();
        }
    }

}


