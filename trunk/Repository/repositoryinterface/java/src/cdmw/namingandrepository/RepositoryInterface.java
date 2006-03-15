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


package cdmw.namingandrepository;

import cdmw.common.TypeMismatchException;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.commonsvcs.naming.NamingInterface;

import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext;

import org.omg.CosNaming.NamingContext;

/**
 * This class provides access to the naming context associated
 * to a name domain.
 * If no name domain is specified, it will use the default name domain.
 *
 */
public class RepositoryInterface {

    /**
     * Default domain name
     */
    private static String defaultDomain = "";

    /**
     * Reference to the CDMW repository
     */
    private static Repository repository = null;
    
    /**
     * Default name domain
     */
    private static NameDomain defaultNameDomain = null;

    /**
     * Default root context
     */
    private static NamingContext defaultRootContext = null;

    /**
     * Default domain context
     */
    private static NamingContext defaultDomainContext = null;

    /**
     * Initialise the RepositoryInterface for use by getDomainContext
     * and getDomainNamingInterface operations.
     *
     * @param dfDomain Name of the default name domain.
     * @param rep Reference of the Cdmw NamingAndRepository.
     *
     * @exception NoNameDomain  <I>defaultDomain</I> does
     * not refer to a Name Domain.
     * @exception com.thalesgroup.CdmwNamingAndRepository.InvalidName
     * <I>defaultDomain</I> is ill-formed.
     * @exception SystemException Any CORBA system exception that may be raised
     * by calls to repository object.
     */
    public static void init(String dfDomain, Repository rep)
        throws
            com.thalesgroup.CdmwNamingAndRepository.NoNameDomain,
            com.thalesgroup.CdmwNamingAndRepository.InvalidName,
            org.omg.CORBA.SystemException {

        if (rep == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        defaultDomain = dfDomain;
        repository = rep;
        // Get the default root context
        try {
            defaultRootContext =
                repository.resolve_root_context(DEFAULT_ROOT_CONTEXT.value);
        } catch (NoRootContext nrc) {
            defaultDomain = "";
            repository = null;
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        // Check that default domain is really a Name Domain
        NameDomain dom = repository.resolve_name_domain(defaultDomain);
        // default_domain is a name domain, so we should be able
        // to get its naming context.
        NamingInterface ni = new NamingInterface(defaultRootContext);

        try {
            defaultDomainContext = resolveName(ni, defaultDomain);
        } catch (TypeMismatchException tme) {
            defaultDomain = "";
            repository = null;
            defaultRootContext = null;
            defaultDomainContext = null;
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CORBA.UserException ue) {
            defaultDomain = "";
            repository = null;
            defaultRootContext = null;
            defaultDomainContext = null;
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        defaultNameDomain = dom;

    }

    /**
     * This operation releases all static object references initialised
     * by previous call to <I>init()</I>. This should be the last operation on
     * this class.
     */
    public static void finish() {
        // Release all static object references
        repository = null;
        defaultRootContext = null;
        defaultDomainContext = null;
    }

    /**
     * Returns the object reference of the CDMW naming
     * and repository service.
     * RepositoryInterface should be initialed before any call to getRepository
     *
     * @return Object reference of the CDMW naming and repository.
     */
    public static Repository getRepository() {
        assertInitialised();
        return repository;
    }

    /**
     * Returns a NamingInterface object embedding a naming context
     * associated to a name domain. If no name is specified for the domain,
     * the default name domain is used.
     * RepositoryInterface should be initialed before any calls
     * to getDomainContext and getDomainNamingInterface operations.
     *
     * @param domainName Name of the name domain.
     *
     * @return NamingInterface object to access the naming context associated to
     * the name domain.
     *
     * @exception NoNameDomain  <I>domainName</I> does
     * not refer to a Name Domain.
     * @exception com.thalesgroup.CdmwNamingAndRepository.InvalidName
     * <I>domainName</I> is ill-formed.
     */
    public static NamingInterface getDomainNamingInterface(String domainName)
        throws 
            com.thalesgroup.CdmwNamingAndRepository.NoNameDomain, 
            com.thalesgroup.CdmwNamingAndRepository.InvalidName {

        NamingContext theContext = getDomainContext(domainName);
        return new NamingInterface(theContext);

    }

    /**
     * Returns the naming context associated to a name domain. If no name
     * is specified for the domain, the default name domain is used.
     * RepositoryInterface should be initialed before any calls to
     * getDomainContext and getDomainNamingInterface operations.
     *
     * @param domainName Name of the name domain.
     *
     * @return Naming context associated to the name domain.
     *
     * @exception NoNameDomain <I>domainName</I> does not refer 
     * to a Name Domain.
     * @exception com.thalesgroup.CdmwNamingAndRepository.InvalidName
     * <I>domainName</I> is ill-formed.
     * @exception INTERNAL Unable to resolve domain name
     */
    public static NamingContext getDomainContext(String domainName)
        throws
            com.thalesgroup.CdmwNamingAndRepository.NoNameDomain,
            com.thalesgroup.CdmwNamingAndRepository.InvalidName,
            org.omg.CORBA.INTERNAL {

        assertInitialised();

        NamingContext theContext;
        if (domainName.length() == 0) {
            theContext = defaultDomainContext;
        } else {
            // Check that default name domain is really a Name Domain
            NameDomain dom = repository.resolve_name_domain(domainName);
            // defaultDomain is a name domain, so we should be able to get its naming
            // context.
            NamingInterface ni = new NamingInterface(defaultRootContext);

            try {
                theContext = resolveName(ni, domainName);
            } catch (TypeMismatchException tme) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (org.omg.CORBA.UserException ue) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        }
        return theContext;

    }
    
    /**
     * Returns the object reference to a name domain. 
     * If no name is specified for the domain, the default name domain is used.
     * RepositoryInterface should be initialed before any calls to this method.
     *
     * @param domainName Name of the name domain.
     *
     * @return A Name Domain object.
     *
     * @exception com.thalesgroup.CdmwNamingAndRepository.NoNameDomain 
     * <I>domainName</I> does not refer to a Name Domain.
     * @exception com.thalesgroup.CdmwNamingAndRepository.InvalidName 
     * <I>domainName</I> is ill-formed.
     * @exception org.omg.CORBA.BAD_INV_ORDER 
     * RepositoryInterface is not initialed.
     * @exception org.omg.CORBA.SystemException Any CORBA system exception 
     * that may be raised by calls to CDMW repository.
     */ 
    public static NameDomain getDomain(String domainName)
        throws
            com.thalesgroup.CdmwNamingAndRepository.NoNameDomain,
            com.thalesgroup.CdmwNamingAndRepository.InvalidName {

        assertInitialised();
    
        NameDomain theNameDomain = null;
        if (domainName.length() == 0)
            theNameDomain = defaultNameDomain;
        else {  
            theNameDomain = repository.resolve_name_domain(domainName);
        }
        return theNameDomain;
        
    }

    /**
     * Checks if internal object references are non-nil.
     *
     * @return <B>true</B> if Repository interface is initiased, <B>false</B>
     * otherwise.
     */
    private static boolean isInitialised() {
        // Check that init have been done properly
        boolean nilRef =
            (repository == null
                || defaultRootContext == null
                || defaultDomainContext == null);
        return !nilRef;
    }

    /**
     * Asserts that internal object references are non-nil.
     *
     * @exception BAD_INV_ORDER If one of these internal references is nil
     * - init() is not called.
     */
    private static void assertInitialised()
        throws org.omg.CORBA.BAD_INV_ORDER {
        if (!isInitialised()) {
            System.out.println("not initialised !");
            throw new org.omg.CORBA.BAD_INV_ORDER(
                ExceptionMinorCodes.BAD_INV_ORDERInitNotCalled,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }

    /**
     * Retrieves the naming context bound to the given name
     *
     * @param The NamingInterface object used to retrieve the context
     * @param The name of the naming context to be retrieved
     *
     * @exception NotFound
     * @exception CannotProceed
     * @exception TypeMismatchException
     * @exception org.omg.CosNaming.NamingContextPackage.InvalidName
     *
     * @return The naming context corresponding to the name given
     */
    private static NamingContext resolveName(NamingInterface ni, String s)
        throws
            TypeMismatchException,
            org.omg.CosNaming.NamingContextPackage.NotFound,
            org.omg.CosNaming.NamingContextPackage.CannotProceed,
            org.omg.CosNaming.NamingContextPackage.InvalidName {

        org.omg.CORBA.Object obj = ni.resolve(s);
        NamingContext nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);
        if (obj != null && nc == null) {
            throw new TypeMismatchException();
        }
        return nc;

    }

}

