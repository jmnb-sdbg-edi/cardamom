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


package cdmw.ccm.componentserver;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

/**
 * The HomeRegistration acts as a singleton factory for the creation of Container
 * objects and is used by an Assembly object during the deployment process.
 */
public class StandardHomeRegistration extends HomeRegistrationBase {

    private cdmw.namingandrepository.NamingInterface root;

    public StandardHomeRegistration() {
        // Check that RepositoryInterface has been initialised
        RepositoryInterface.getRepository();
    }

    protected boolean
        bind(org.omg.Components.CCMHome homeRef, String homeName) {

        boolean result = false;
        com.thalesgroup.CdmwNamingAndRepository.NameDomain dom = null;
        com.thalesgroup.CdmwLifeCycle.FactoryBase factory = null;
        String basename = "";
        try {
            basename = NamingInterface.basename(homeName);
            String dirname = NamingInterface.dirname(homeName);

            // Get the NameDomain
            dom = RepositoryInterface.getDomain(dirname);
            factory =
                com.thalesgroup.CdmwLifeCycle.FactoryBaseHelper.narrow(homeRef);

            if (factory != null) {
                dom.register_new_factory(basename, factory);
                result = true;
            }
        } catch (cdmw.namingandrepository.InvalidNameException ine) {
            // ignore...
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            // ignore...
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            // ignore...
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
            try {
                // Re-register it again (force registration!).
                dom.release_name(basename);
                dom.register_new_factory(basename, factory);
                result = true;
            } catch (Exception e) {
                // ignore...
            }
        }
        return result;

    }

    protected boolean unbind(String homeName) {

        boolean result = false;
        try {
            String basename = NamingInterface.basename(homeName);
            String dirname = NamingInterface.dirname(homeName);

            // Get the NameDomain
            com.thalesgroup.CdmwNamingAndRepository.NameDomain dom =
                RepositoryInterface.getDomain(dirname);
            dom.release_name(basename);
            result = true;
        } catch (cdmw.namingandrepository.InvalidNameException ine) {
            // ignore...
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            // ignore...
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            // ignore...
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
            // ignore...
        }
        return result;

    }

}

