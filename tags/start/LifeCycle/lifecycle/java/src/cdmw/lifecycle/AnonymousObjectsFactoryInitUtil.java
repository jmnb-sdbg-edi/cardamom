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


package cdmw.lifecycle;

import cdmw.common.Assert;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.InvalidNameException;

import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.NoNameDomain;
import com.thalesgroup.CdmwNamingAndRepository.InvalidName;

import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.InvalidRegistration;

import com.thalesgroup.CdmwLifeCycle.FactoryBase;
import com.thalesgroup.CdmwLifeCycle.FactoryBaseHelper;

import org.omg.CORBA.ORB;
import org.omg.CORBA.Any;
import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.TRANSIENT;
import org.omg.CORBA.OBJECT_NOT_EXIST;
import org.omg.CORBA.SystemException;

import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;
import org.omg.PortableServer.POAPackage.ObjectAlreadyActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;
import org.omg.PortableServer.POAPackage.ObjectNotActive;
import org.omg.PortableServer.POAPackage.ServantAlreadyActive;

/**
 * This class provides functions needed during CDMW process initialisation
 * to create CDMW anonymous objects.
 *
 */
public class AnonymousObjectsFactoryInitUtil {

    /**
     * This function format a string to a string suitable 
     * for use as an ObjectId.
     * @param s Input string
     *
     * @return A properly formatted string
     */
    public static String formatNameForObjectId(String s) {
        return s.replace('/','_');
    }

    /**
     * This function creates/activates an anonymous objects CDMW factory.
     *
     * @param allocator The factory allocator to be used for factory creation
     * @param orb A reference to the ORB pseudo-object
     * @param parent Parent POA for the factory
     * @param repository Reference of the CDMW Repository
     * @param rootContext Default Root Context of the CDMW Repository
     * @param fullyQualifiedFactoyName Fully qualified name of the CDMW factory
     * @param factoryRepositoryId Repository ID of the factory
     * @param threadingPolicy Threading issues
     * @param args Program arguments
     *
     * @exception BadParameterException Wrong parameter value
     * @exception InternalErrorException Internal Error
     * @exception SystemException Unreachable CDMW Repository
     */
    public static void setupAnonymousObjectFactory(
            AnonymousObjectsFactoryAllocatorBase allocator,
            ORB orb,
            POA parent,
            Repository repository,
            NamingInterface rootContext,
            String fullyQualifiedFactoryName,
            String factoryRepositoryId,
            POAThreadingPolicy threadingPolicy,
            String[] args)
        throws  BadParameterException,
                InternalErrorException,
                SystemException {

        Assert.check(parent!=null);
        Assert.check(repository!=null);

        // What do we need?
        // 1) NameDomain where the factory is/will be registered
        // 2) Name of the factory
        // 3) Parent POA for the factory
        // 4) A reference to the ORB pseudo-object
        // 5) args
        // 6) Add thrading issues : POA threading policy + thread pool size ... etc.

        // typedef Dictionary<FactoryAllocatorBase*> FDictionary;
        // typedef NamedObjectsFactoryAllocator<T> Allocator;

        // Register the name of the factory within its NameDomain. This
        // will check validity of the name and whether it is already in use.
        String factoryName = "";
        String factoryNameDomainName = "";

        try {
            factoryName
                = NamingInterface.basename(fullyQualifiedFactoryName);
            factoryNameDomainName
                = NamingInterface.dirname(fullyQualifiedFactoryName);
        } catch (InvalidNameException ine) {
            throw new BadParameterException(ErrorMessages.InvalidNameMessage);
        }

        // CDMW names should be within a name domain
        Assert.check(factoryNameDomainName.length()!=0);

        NameDomain factoryNameDomain = null;
        try {
            factoryNameDomain
                = repository.resolve_name_domain(factoryNameDomainName);
        } catch (NoNameDomain nnd) {
            throw new BadParameterException(ErrorMessages.NoNameDomainMessage);
        } catch (InvalidName in) {
            throw new BadParameterException(ErrorMessages.InvalidNameMessage);
        }

        Any regId = null;
        boolean doCreateFactoryReference = true;
        try {
            regId = factoryNameDomain.new_name(factoryName);
        } catch (AlreadyExists ae) {
            doCreateFactoryReference = false;
        } catch (InvalidName in) {
            // The name should have been valid
            Assert.check(false);
        } catch (OBJECT_NOT_EXIST one) {
            // Oops! CDMW Repository has been destroyed
            throw new InternalErrorException();
        } catch (SystemException se) {
            // Cannot reach the CDMW Repository at the moment, retry later!
            throw new TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        // TODO: If factory's name has been registered successfuly.
        // Do not forget to unregister it if an exception occurs
        // ObjectRegistrationGuard unreg(factory_name_domain),factory_name);
        Servant fac = allocator.allocate(
                // ORB
                orb,
                // PARENT POA
                parent,
                // Fully qualified factory name
                fullyQualifiedFactoryName,
                // Threading Policy
                threadingPolicy,
                args);

        // We add a guard object to deallocate the factory if an exception is raised.
        FactoryAllocationGuard allocGuard
            = new FactoryAllocationGuard(allocator,fac);

        // Obtain object Id from fully_qualified_factory_name
        String oidstring = formatNameForObjectId(fullyQualifiedFactoryName);
        byte[] oid = oidstring.getBytes();

        try {
            parent.activate_object_with_id(oid,fac);
        } catch (ObjectAlreadyActive oaa) {
            allocGuard.destroy();
            throw new
                BadParameterException(ErrorMessages.FactoryExistsMessage);
        } catch (ServantAlreadyActive saa) {
            allocGuard.destroy();
            throw new
                BadParameterException(ErrorMessages.FactoryInUseMessage);
        } catch (WrongPolicy wp) {
            allocGuard.destroy();
            throw new InternalErrorException();
        }

        // Register the factory instance within the local dictionary
        //FDictionary::Get()[oidstring]= &allocator;

        if (doCreateFactoryReference) {
            // Register the object reference within the CDMW repository
            try {
                org.omg.CORBA.Object obj = parent.id_to_reference(oid);

                // Register Factory's object reference
                FactoryBase facRef = FactoryBaseHelper.narrow(obj);
                Assert.check(facRef!=null);

                factoryNameDomain.register_factory(regId,facRef);
            } catch (ObjectNotActive nta) {
                allocGuard.destroy();
                // From id_to_reference
                throw new InternalErrorException();
            } catch (WrongPolicy wp) {
                allocGuard.destroy();
                // From id_to_reference
                throw new InternalErrorException();
            } catch (InvalidRegistration ir) {
                allocGuard.destroy();
                // From register_factory
                throw new InternalErrorException();
            } catch (AlreadyExists ae) {
                allocGuard.destroy();
                // From register_factory
                throw new InternalErrorException();
            }
        }
        // Do not deallocate!
        allocGuard.cancel();
    }

}

