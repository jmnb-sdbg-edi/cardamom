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


package cdmw.tools;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.commonsvcs.naming.InvalidNameException;

import com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT;
import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.NoNameDomain;
import com.thalesgroup.CdmwNamingAndRepository.InvalidName;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext;

import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;

/**
 * Provides an implementation of the Repository interface.
 *
 * Features:
 * Thread safe
 *
 */
public class SimRepositoryImpl 
    extends com.thalesgroup.CdmwNamingAndRepository.RepositoryPOA {

    public static final String FACTORIES_CONTEXT = "Factories";

    private java.util.HashMap domains = new java.util.HashMap();
    private java.util.HashMap rootContexts = new java.util.HashMap();

    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA poa;

    private boolean initialised = false;

    public SimRepositoryImpl(org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        NamingContext nc,String[] args) {

        this.orb = orb;
        this.poa = parent;

        initialised = init(nc,args);
        if (!initialised) {
            System.out.println("REPOSITORY: Initialisation failed.");
        }

    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }


    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_root_context:1.0
    //
    public NamingContext resolve_root_context(String rootId)
        throws NoRootContext,org.omg.CORBA.SystemException {

        if (!initialised) {
            throw new org.omg.CORBA.INTERNAL();
        }

        NamingContext context = (NamingContext) rootContexts.get(rootId);
        if (context  == null) {
            throw new NoRootContext();
        } else {
            return context;
        }

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_name_domain:1.0
    //
    public NameDomain resolve_name_domain(String domainName)
        throws NoNameDomain,InvalidName,org.omg.CORBA.SystemException {

        if (!initialised) {
            throw new org.omg.CORBA.INTERNAL();
        }

        // Is the name valid
        try {
            org.omg.CosNaming.NameComponent[] name 
                = NamingInterface.toName(domainName);
        } catch (InvalidNameException ine) {
            throw new InvalidName();
        }
        SimNameDomainImpl domain =
            (SimNameDomainImpl) domains.get(domainName);
        if (domain  == null) {
            throw new NoNameDomain();
        } else {
            return domain._this();
        }

    }

    private boolean init(NamingContext nc, String[] args) {

        String name = DEFAULT_ROOT_CONTEXT.value;

        // get default root context
        NamingContext defaultRootNc = bind_new_context(nc,name);
        rootContexts.put(name,defaultRootNc);
        NamingInterface defaultNi = new NamingInterface(defaultRootNc);

        //
        // Create persistent POA
        //
        org.omg.PortableServer.POAManager poaManager = poa.the_POAManager();
        cdmw.orbsupport.PolicyList policies 
            = new cdmw.orbsupport.PolicyList();
        policies.insert(
            poa.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT));
        org.omg.PortableServer.POA domPOA = null;
        try {
            domPOA =
                poa.create_POA("NameDomains", poaManager, policies.getList());
        } catch(org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            return false;
        } catch(org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            return false;
        }

        // This installs initial bindings for NameDomains
        int i = 0;
        while(i < args.length) {
            String option = args[i];

            if( option.equals("-DOM") ) {
                if(i + 1 >= args.length) {
                    System.out.println("Argument expected for -DOM");
                    return false;
                }
                String dom = args[i+1];
                SimNameDomainImpl domainPos =
                    (SimNameDomainImpl ) domains.get(dom);

                if (domainPos == null) {
                    // add a name domain
                    SimNameDomainImpl nd = new SimNameDomainImpl (orb,
                                            domPOA,defaultRootNc,dom,this);
                    domains.put(dom,nd);
                    // activate object
                    try {
                        domPOA.activate_object(nd);
                    } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive saa) {
                        System.out.println("ServantAlreadyActive: "
                            + saa.getMessage());
                        return false;
                    } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
                        System.out.println("WrongPolicy: "+wp.getMessage());
                        return false;
                    } catch (Exception e) {
                        System.out.println("Exception: "+e.getMessage());
                        return false;
                    }
                    // create associated naming context
                    try {
                        defaultNi.bindNewContext(dom,true);
                    } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
                        System.out.println("Invalid name "+in.getMessage());
                         return false;
                    } catch (Exception e) {
                        System.out.println("Unable to bind <" + dom
                            + "> naming context.");
                        return false;
                    }
                } else {
                    System.out.println("Name Domain <"
                        + dom + "> already present.");
                    return false;
                }

                i += 2;
                
            } else if( option.equals("-ROOT") ) {
                if(i + 1 >= args.length) {
                    System.out.println("Argument expected for -ROOT");
                    return false;
                }

                String root = args[i+1];
                NamingContext context = (NamingContext) rootContexts.get(root);

                if (context == null) {
                     // add a root context
                     NamingContext rootNc = bind_new_context(nc,root);
                     rootContexts.put(root,rootNc);
                } else {
                    System.out.println("Root Naming Context <"
                                            + root + "> already present.");
                    return false;
                }

                i += 2;
            
            } else {
                i++;
            }
        }
        return true;

    }

    private NamingContext bind_new_context(NamingContext nc,String name) {

        NamingInterface ni = new NamingInterface(nc);

        try {
            ni.bindNewContext(name,false);
        } catch (AlreadyBound ab) {
            // Good...
        } catch (Exception e) {
            System.out.println("Cannot bind <" + name + "> context");
            return null;
        }

        try {
            org.omg.CORBA.Object o = ni.resolve(name);
            return org.omg.CosNaming.NamingContextHelper.narrow(o);
        } catch(NotFound nf) {
            System.out.println(nf.getMessage());
            return null;
        } catch(CannotProceed cp) {
            System.out.println(cp.getMessage());
            return null;
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            System.out.println(in.getMessage());
            return null;
        }

    }

}

