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

import com.thalesgroup.CdmwNamingAndRepository.InvalidName;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.InvalidRegistration;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NameDomainDescription;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered;

import com.thalesgroup.CdmwLifeCycle.FactoryBase;

import org.omg.CosNaming.NameComponent;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;

import org.omg.CORBA.Any;
import org.omg.CORBA.BAD_PARAM;
import org.omg.CORBA.NO_IMPLEMENT;
import org.omg.CORBA.SystemException;

/**
 * Provides an implementation of the Name Domain interface.
 *
 * Features:
 * Thread safe
 *
 */
public class SimNameDomainImpl 
    extends com.thalesgroup.CdmwNamingAndRepository.NameDomainPOA {

    public static final String FACTORIES_CONTEXT = "Factories";

    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA poa;
    private NamingInterface root;
    private String name;
    private SimRepositoryImpl repository;
    private java.util.Vector registry = new java.util.Vector();

    public SimNameDomainImpl(org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        org.omg.CosNaming.NamingContext root, String name,
        SimRepositoryImpl rep) throws SystemException {

        this.orb = orb;
        this.poa = parent;
        this.root = new NamingInterface(root);
        this.name = name;
        this.repository = rep;

    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    private boolean isValid(String s) {

        boolean valid = false;
        try {
            NameComponent[] name = root.toName(s);
            if (name.length == 1) {
                valid = true;
            }
        } catch (InvalidNameException ine) {}
        return valid;

    }

    private boolean isInRegistry(String s) {
        return registry.contains(s);
    }

    private void insertInRegistry(String s) {
        registry.add(s);
    }

    private void removeFromRegistry(String s) {
        registry.remove(s);
    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/new_name:1.0
    //
    public Any new_name(String name)
        throws AlreadyExists,InvalidName,SystemException {

        if (!isValid(name)) {
            throw new InvalidName();
        }

        if (isInRegistry(name)) {
            throw new AlreadyExists();
        }

        try {
            String sname = this.name + "/" + name;
            org.omg.CORBA.Object obj = root.resolve(sname);
            throw new AlreadyExists();
        } catch (NotFound nf) {
            insertInRegistry(name);
            Any id = orb.create_any();
            id.insert_string(name);
            return id;
        } catch(CannotProceed cp) {
            throw new org.omg.CORBA.INTERNAL();
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new BAD_PARAM();
        }

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_object:1.0
    //
    public void register_object(Any regId,org.omg.CORBA.Object theObject)
        throws InvalidRegistration,AlreadyExists,SystemException {

        String n = regId.extract_string();
        if (!isInRegistry(n)) {
            throw new InvalidRegistration();
        }

        try {
            String name = this.name + "/" + n;
            root.bind(name,theObject,false);
            // Invalidate Registration ID
            removeFromRegistry(n);
        } catch (AlreadyBound ab) {
            throw new AlreadyExists();
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new InvalidRegistration();
        } catch (NotFound nf) {
            throw new BAD_PARAM();
        } catch (CannotProceed cp) {
            throw new BAD_PARAM();
        }

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_object:1.0
    //
    public void register_new_object(
        String objectName,org.omg.CORBA.Object theObject)
        throws AlreadyExists,InvalidName,SystemException {

         // Check validity of the name
        if (!isValid(objectName)) {
            throw new InvalidName();
        }

        if (isInRegistry(objectName)) {
            throw new AlreadyExists();
        }

        try {
            String name = this.name + "/" + objectName;
            root.bind(name,theObject,false);
        } catch (AlreadyBound ab) {
            throw new AlreadyExists();
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new InvalidName();
        } catch (NotFound nf) {
            throw new BAD_PARAM();
        } catch (CannotProceed cp) {
            throw new BAD_PARAM();
        }

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_factory:1.0
    //
    public void register_factory(Any regId,FactoryBase theFactory)
        throws InvalidRegistration,AlreadyExists,SystemException {

        register_object(regId,theFactory);
        // Bind factory in "Factories"

        String ctx = this.name + "/" + FACTORIES_CONTEXT;
        String s = regId.extract_string();
        try {
            String name = ctx + "/" + s;
            root.bind(name,theFactory,false);
        } catch(NotFound nf) {
            try {
                NamingInterface ni = root.bindNewContext(ctx,false);
                ni.bind(s,theFactory,false);
            } catch (Exception e) {
                // bad news!
            }
        } catch (CannotProceed cp) {
            throw new BAD_PARAM();
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new InvalidRegistration();
        } catch (AlreadyBound ab) {
            throw new AlreadyExists();
        }

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_factory:1.0
    //
    public void register_new_factory(String factoryName,FactoryBase theFactory)
        throws AlreadyExists,InvalidName,SystemException {

        register_new_object(factoryName,theFactory);

        String ctx = this.name + "/" + FACTORIES_CONTEXT;
        // Bind factory in "Factories"
        try {
            String name = ctx + "/" + factoryName;
            root.bind(name,theFactory,false);
        } catch(NotFound nf) {
            try {
                NamingInterface ni = root.bindNewContext(ctx,false);
                ni.bind(factoryName,theFactory,false);
            } catch (Exception e) {
                // bad news!
            }
        } catch (CannotProceed cp) {
            throw new BAD_PARAM();
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new InvalidName();
        } catch (AlreadyBound ab) {
            throw new AlreadyExists();
        }

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/resolve_sub_domain:1.0
    //
    public com.thalesgroup.CdmwNamingAndRepository.NameDomain
        resolve_sub_domain(String domainName)
        throws com.thalesgroup.CdmwNamingAndRepository.NoNameDomain,
        InvalidName,SystemException {

         // Check validity of domain_name
        try {
            NameComponent[] name = root.toName(domainName);
        } catch (InvalidNameException ine) {
            throw new InvalidName();
        }

        String name = this.name + "/" + domainName;
        return repository.resolve_name_domain(name);

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_name_domain:1.0
    //
    public void
        register_name_domain(Any regId,NameDomainDescription theNameDomain)
        throws InvalidRegistration,AlreadyExists,SystemException {

        throw new NO_IMPLEMENT();

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_name_domain:1.0
    //
    public void register_new_name_domain(String domainName,
        NameDomainDescription theNameDomain)
        throws AlreadyExists,InvalidName,SystemException {

        throw new NO_IMPLEMENT();

    }

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/release_name:1.0
    //
    public void release_name(String name)
        throws NotRegistered,InvalidName,SystemException {

         // Check validity of the name
        if (!isValid(name)) {
            throw new InvalidName();
        }

        if (isInRegistry(name)) {
           removeFromRegistry(name);
        } else {
           try {
                String n = this.name + "/" + name;
                root.unbind(n);
                // If its a factory, then remove it from "Factories"
                try {
                  String f = name + "/" + FACTORIES_CONTEXT + "/" + name;
                  root.unbind(f);
                } catch (Exception e) {
                  // continue anyway ...
                }
            } catch (NotFound nf) {
                throw new NotRegistered();
            } catch (CannotProceed cp) {
                throw new BAD_PARAM();
            } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
                throw new InvalidName();
            }
        }

    }

}

