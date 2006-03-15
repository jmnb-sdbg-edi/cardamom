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


import org.omg.Components.CreateFailure;
import org.omg.Components.RemoveFailure;
import org.omg.Components.Deployment.InvalidLocation;
import org.omg.Components.Deployment.InvalidAssembly;
import org.omg.Components.Deployment.Assembly;
import org.omg.Components.Deployment.AssemblyHelper;
import org.omg.Components.Deployment.AssemblyState;

import org.omg.CORBA.Repository;
import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;
import com.thalesgroup.CdmwDeployment.PackageManager;
import com.thalesgroup.CdmwDeployment.StandardConfiguratorFactory;

import cdmw.ccm.deployment.FileUtilities;
import cdmw.ccm.deployment.CadReader;


/**
 * Implementation of the AssemblyFactory CORBA interface.
 * This class follows the singleton pattern.
 */
class AssemblyFactoryImpl
    extends com.thalesgroup.CdmwDeployment.AssemblyFactoryPOA
{

    private static final String TMP_ASSEMBLY_DIR_PREFIX = "assemblyPack";
    private static final String TMP_ASSEMBLY_PACK_NAME  = "assembly.pack";

    /**
     * Singleton instance.
     */
    private static AssemblyFactoryImpl instance = null;

    /**
     * ORB object.
     */
    private org.omg.CORBA.ORB orb;

    /**
     * POA for AssemblyFactory object.
     */
    private org.omg.PortableServer.POA factoryPOA;

    /**
     * POA for Assembly objects.
     */
    private org.omg.PortableServer.POA assembliesPOA;

    /**
     * ConfiguratorFactory CORBA object.
     */
    private StandardConfiguratorFactory configuratorFactory;

    /**
     * XMLParser CORBA object.
     */
    private XMLParser parser;

    /**
     * PackageManager CORBA object
     */
    private PackageManager packageManager;

    /**
     * AssemblyLoader: 
     *   fill AssemblyImpl objects according to .cad files
     */
    private AssemblyLoader assemblyLoader;

    /**
     * Map of Assemblies created by the AssemblyFactory.
     * They are indexed by there identifier.
     */
    private java.util.Map assemblies;

    /**
     * Counter used to identify anonymous assemblies.
     */
    private int assemblyCounter;


    /**
     * Constructor.
     *
     * @param orb the ORB object.
     * @param factoryPOA POA for AssemblyFactory object.
     * @param assembliesPOA POA for Assembly objects.
     * @param configuratorFactory ConfiguratorFactory CORBA object.
     * @param parser XMLParser CORBA object.
     * @param packageManager PackageManager CORBA object.
     */
    protected AssemblyFactoryImpl(org.omg.CORBA.ORB orb,
                                  org.omg.PortableServer.POA factoryPOA,
                                  org.omg.PortableServer.POA assembliesPOA,
                                  StandardConfiguratorFactory configuratorFactory,
                                  XMLParser parser,
                                  PackageManager packageManager)
    {
        this.orb = orb;
        this.factoryPOA = factoryPOA;
        this.assembliesPOA = assembliesPOA;
        this.configuratorFactory = configuratorFactory;
        this.parser = parser;
        this.packageManager = packageManager;
        this.assemblyLoader = new AssemblyLoader(this.assembliesPOA);
        this.assemblies = new java.util.HashMap();

        // register CookieFactory
        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.CookieHelper.id(),
                new cdmw.ccm.common.CookieFactoryImpl());

        // init assemblyCounter
        assemblyCounter = 0;
    }

    /**
     * Initialises the AssemblyFactoryImpl singleton instance.
     *
     * @param orb the ORB object.
     * @param factoryPOA POA for AssemblyFactory object.
     * @param assembliesPOA POA for Assembly objects.
     * @param configuratorFactory ConfiguratorFactory CORBA object.
     * @param parser XMLParser CORBA object.
     * @param packageManager PackageManager CORBA object.
     */
    public static AssemblyFactoryImpl 
    init(org.omg.CORBA.ORB orb,
         org.omg.PortableServer.POA factoryPOA,
         org.omg.PortableServer.POA assembliesPOA,
         StandardConfiguratorFactory configuratorFactory,
         XMLParser parser,
         PackageManager packageManager)
    {
        cdmw.common.Assert.check(instance == null);
        
        instance = new AssemblyFactoryImpl(orb,
                                           factoryPOA,
                                           assembliesPOA,
                                           configuratorFactory,
                                           parser,
                                           packageManager);
        return instance;
    }


    /**
     * Returns the AssemblyFactoryImpl singleton instance.
     * The init() operation must have been called first. 
     *
     * @return the AssemblyFactoryImpl singleton instance.
     */
    public static AssemblyFactoryImpl getInstance() {
        cdmw.common.Assert.check(instance != null);
        return instance;
    }

    
    /**
     * Returns the ORB object.
     *
     * @return the ORB object.
     */
    public org.omg.CORBA.ORB getOrb() {
        return orb;
    }

    
    /**
     * Returns the ConfiguratorFactory CORBA object.
     *
     * @return the ConfiguratorFactory CORBA object.
     */
    public StandardConfiguratorFactory getConfiguratorFactory() {
        return configuratorFactory;
    }

    
    /**
     * Returns the XMLParser CORBA object.
     *
     * @return the XMLParser CORBA object.
     */
    public XMLParser getXMLParser() {
        return parser;
    }

    
    /**
     * Returns the PackageManager CORBA object.
     *
     * @return the PackageManager CORBA object.
     */
    public PackageManager getPackageManager() {
        return packageManager;
    }


    /**
     * Retrieves the InterfaceRepository CORBA object
     * using orb.resolve_initial_reference() operation.
     *
     * @return the InterfaceRepository CORBA object.
     */
    public org.omg.CORBA.Repository getInterfaceRepository()
        throws Exception
    {
        org.omg.CORBA.Object obj;
        try {
            obj = orb.resolve_initial_references("InterfaceRepository");
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("InterfaceRepository is not specified");
        }

        if (obj == null) {
            throw new Exception("The specified InterfaceRepository reference is invalid.");
        }

        org.omg.CORBA.Repository repository = null;
        try {
            repository = org.omg.CORBA.RepositoryHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for InterfaceRepository " +
                                "does not implement a Repository.");
        }

        return repository;
    }


    /**
     * Retrieves the Naming Service's root NamingContext CORBA object
     * using orb.resolve_initial_reference() operation.
     *
     * @return the Naming Service's root NamingContext CORBA object.
     */
    public org.omg.CosNaming.NamingContext getNamingService()
        throws Exception
    {
        org.omg.CORBA.Object obj;
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("NameService is not specified");
        }

        if (obj == null) {
            throw new Exception("The specified NameService reference is invalid.");
        }

        org.omg.CosNaming.NamingContext naming = null;
        try {
            naming = org.omg.CosNaming.NamingContextHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for NameService " +
                                "does not implement a NamingContext.");
        }

        return naming;
    }


    /**
     * Retrieves the HomeFinder CORBA object
     * using orb.resolve_initial_reference() operation.
     *
     * @return the HomeFinder CORBA object.
     */
    public org.omg.Components.HomeFinder getHomeFinder()
        throws Exception
    {
        org.omg.CORBA.Object obj;
        try {
            obj = orb.resolve_initial_references("HomeFinder");
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("HomeFinder is not specified");
        }

        if (obj == null) {
            throw new Exception("The specified HomeFinder reference is invalid.");
        }

        org.omg.Components.HomeFinder homeFinder = null;
        try {
            homeFinder = org.omg.Components.HomeFinderHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for HomeFinder " +
                                "does not implement a HomeFinder.");
        }

        return homeFinder;
    }




    /**
     * Returns the parent POA of the AssemblyFactory object.
     *
     * @return the parent POA of the AssemblyFactory object.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return factoryPOA;
    }


    /**
     * Creates a new Cookie using the name parameter.
     *
     * @param name the name to be used as Cookie data.
     * @return the new Cookie.
     */
    private synchronized org.omg.Components.Cookie createCookie(String name) {
        byte[] b = name.getBytes();
        return new cdmw.ccm.common.CookieImpl(b);
    }

    
    /**
     * Implementation of the IDL operation:
     *  IDL:omg.org/Components/Deployment/AssemblyFactory/create:1.0.
     * Creates a new Assembly object according to an assembly package location,
     * and returns a new Cookie identifying this Assembly.
     * The Assembly will have a generated unique name into its Cookie.
     *
     * @param assembly_loc the location of the assembly package.
     * @return a Cookie identifying the new Assembly.
     *
     * @exception InvalidLocation if assembly_loc is an invalid location.
     * @exception CreateFailure if the assembly creation fails.
     */
    public org.omg.Components.Cookie create(String assembly_loc)
        throws InvalidLocation,
               CreateFailure
    {                
        // call create_with_name() with null as name
        // (for synchronization of name reservations)
        return create_with_name(assembly_loc, null);
    }

    /**
     * Implementation of the IDL operation:
     *  IDL:thalesgroup.com/CdmwDeployment/AssemblyFactory/create_with_name:1.0.
     * Creates a new Assembly object according to an assembly package location,
     * and returns a new Cookie identifying this Assembly.
     * The Assembly will have the specified name into its Cookie.
     *
     * @param assembly_loc the location of the assembly package.
     * @param name the name of the Assembly to be created.
     * @return a Cookie identifying the new Assembly.
     *
     * @exception InvalidLocation if assembly_loc is an invalid location.
     * @exception CreateFailure if the assembly creation fails.
     */
    public org.omg.Components.Cookie create_with_name(String assembly_loc, String name)
        throws InvalidLocation,
               CreateFailure
    {        
        
        // name of the assembly (could be generated)
        String assemblyName;
        org.omg.Components.Cookie c;
        
        // synchronize the reservation of names
        synchronized (this) {
            if (name != null) {
                c = createCookie(name);
                if (assemblies.containsKey(c)) {
                    throw new org.omg.Components.CreateFailure(
                        "The name \"" + name + "\" already correspond to another Assembly.",
                        com.thalesgroup.CdmwDeployment.ASSEMBLY_NAME_ALREADY_USED.value);
                } else {
                    assemblyName = name;
                }                    
            
            } else {
                // generate a name which doesn't already exists
                do {
                    assemblyName = "Assembly_" + (assemblyCounter++);
                    c = createCookie(assemblyName);
                } while (assemblies.containsKey(c));
            }
            
            // Reserve the name
            assemblies.put(c, null);
        }
        
        System.out.println("---- Creation of assembly " + assemblyName + 
                           " according to " + assembly_loc);
        
        try {
            if (assembly_loc.equals("")) {
                throw new InvalidLocation(
                    "The assembly location couldn't be an empty string.");
            }

            CadReader cadReader = null;
            String cadLocation = assembly_loc;
            
            // check if assembly_loc is a local file
            java.io.File loc = new java.io.File(assembly_loc);
            if (loc.isFile()) {
                // get absolute path of cad file
                cadLocation = loc.getAbsolutePath();
            }
            
            // try to parse cad file
            try {
                cadReader = new CadReader(parser, cadLocation);
            } catch (ParsingException e) {
                throw new InvalidLocation(e.reason);
            }
            
            // load assembly
            AssemblyImpl assemblyImpl = assemblyLoader.load(cadReader);
            
            // activate assembly
            Assembly assembly = null;
            try {
                byte[] oid = assembliesPOA.activate_object(assemblyImpl);
                assembly = AssemblyHelper.narrow(assembliesPOA.id_to_reference(oid));
            } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            
            // put assemblyImpl into assemblies map
            assemblies.put(c, assemblyImpl);
            
            // return cookie
            return c;
            
        } catch (InvalidLocation e) {
            assemblies.remove(c);
            System.err.println(e.getMessage());
            throw e;
        } catch (CreateFailure e) {
            assemblies.remove(c);
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            assemblies.remove(c);
            System.err.println(e.toString());
            throw e;
        } finally {
            System.out.println("---- End of " + assemblyName + " creation");
            System.out.println();
        }
    }


    /**
     * Implementation of the IDL operation:
     *  IDL:omg.org/Components/Deployment/AssemblyFactory/lookup:1.0.
     * Returns the Assembly CORBA object corresponding to the specified Cookie.
     *
     * @param c the Cookie identifying the Assembly.
     * @return the Assembly corresponding to the Cookie.
     *
     * @exception InvalidAssembly if the specified Cookie doesn't correspond 
     *            to any Assembly created by this AssemblyFactory.
     */
    public Assembly lookup(org.omg.Components.Cookie c)
        throws InvalidAssembly
    {
        try {
            if (c == null) {
                throw new org.omg.CORBA.BAD_PARAM(
                    cdmw.orbsupport.ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            
            // get corresponding AssemblyImpl
            AssemblyImpl assemblyImpl = (AssemblyImpl) assemblies.get(c);
    
            if (assemblyImpl == null) {
                throw new InvalidAssembly();
            }
    
            // retrieve corresponding object reference
            Assembly assembly = null;
            try {
                 assembly = AssemblyHelper.narrow(
                    assembliesPOA.servant_to_reference(assemblyImpl));
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ServantNotActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
    
           return assembly;
        
        } catch (InvalidAssembly e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }        
    }

    /**
     * Implementation of the IDL operation:
     *  IDL:thalesgroup.com/CdmwDeployment/AssemblyFactory/lookup_with_name:1.0.
     * Returns the Assembly CORBA object corresponding to the specified name.
     *
     * @param name the name of the Assembly.
     * @return the Assembly with the specified name.
     *
     * @exception InvalidAssembly if the specified name doesn't correspond 
     *            to any Assembly created by this AssemblyFactory.
     */
    public Assembly lookup_with_name(String name)
        throws InvalidAssembly
    {
        // create cookie corresponding to name
        org.omg.Components.Cookie c = createCookie(name);
    
        return lookup(c);
    }


    /**
     * Implementation of the IDL operation:
     *  IDL:omg.org/Components/Deployment/AssemblyFactory/destroy:1.0.
     * Destroys an Assembly created by this AssemblyFactory.
     *
     * @param c the Cookie identifying the Assembly.
     *
     * @exception InvalidAssembly if the specified Cookie doesn't correspond 
     *            to any Assembly created by this AssemblyFactory.
     * @exception RemoveFailure if destruction fails.
     */
    public void destroy(org.omg.Components.Cookie c)
        throws InvalidAssembly,
               org.omg.Components.RemoveFailure
    {
        try {
            if (c == null) {
                throw new org.omg.CORBA.BAD_PARAM(
                    cdmw.orbsupport.ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            // get corresponding AssemblyImpl
            AssemblyImpl assemblyImpl = (AssemblyImpl) assemblies.get(c);
            
            if (assemblyImpl == null) {
                throw new InvalidAssembly();
            }
            
            // remove assemblyImpl from assemblies map
            assemblies.remove(c);
            
            // if assembly is in service, tear it down
            if (assemblyImpl.get_state().value() == AssemblyState._INSERVICE) {
                assemblyImpl.tear_down();
            }
            
            // deactivate assemblyImpl
            try {
                byte[] oid = assembliesPOA.servant_to_id(assemblyImpl);
                assembliesPOA.deactivate_object(oid);
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ServantNotActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }

            // destroy assemblyImpl
            assemblyImpl.destroy();
                        
        } catch (InvalidAssembly e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.Components.RemoveFailure e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }
    }

    /**
     * Implementation of the IDL operation:
     *  IDL:thalesgroup.com/CdmwDeployment/AssemblyFactory/destroy_with_name:1.0.
     * Destroys an Assembly created by this AssemblyFactory.
     *
     * @param name the name of the Assembly.
     *
     * @exception InvalidAssembly if the specified name doesn't correspond 
     *            to any Assembly created by this AssemblyFactory.
     * @exception RemoveFailure if destruction fails.
     */
    public void destroy_with_name(String name)
        throws InvalidAssembly,
               org.omg.Components.RemoveFailure
    {
        // create cookie corresponding to name
        org.omg.Components.Cookie c = createCookie(name);
    
        destroy(c);
    }

}


