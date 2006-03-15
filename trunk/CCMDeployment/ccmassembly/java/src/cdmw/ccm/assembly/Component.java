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

import org.omg.Components.Configurator;

import com.thalesgroup.CdmwDeployment.WRONG_HOME_KIND;
import com.thalesgroup.CdmwDeployment.PROPERTIES_CONFIGURATION_ERROR;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;
import com.thalesgroup.CdmwDeployment.NAMING_SERVICE_ERROR;
import com.thalesgroup.CdmwDeployment.PORT_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.CREATE_FAILURE;


/**
 * Abstract class representing a component.
 * It could be a single component or a component group.
 * In both case, it has a reference to a CCMObject which
 * could be configured and registered into a Naming Service.
 */
abstract class Component {

    /**
     * Identifier of the component as specified 
     * into the .cad file.
     */
    private String id;
    
    /**
     * The CCMObject this class represent.
     */
    protected org.omg.Components.CCMObject component;
    
    /**
     * Configurator for the component.
     */
    protected Configurator componentConfigurator;
    
    /**
     * Boolean set to true if component has been activated.
     */
    protected boolean isActivated;
    
    /**
     * List of names the component has to be registered with.
     */
    protected java.util.Collection componentNamingRegistrations;
    
    /**
     * Map indexed by provides identifier.
     * For each, there is a list of name 
     * the facet has to be registered with.
     */
    protected java.util.Map providesNamingRegistrations;

    /**
     * Map indexed by consumes identifier.
     * For each, there is a list of name 
     * the consumer has to be registered with.
     */
    protected java.util.Map consumerNamingRegistrations;


    /**
     * Protected base constructor to be used by inheriting classes.
     *
     * @param id the component identifier as specified into the .cad file.
     */
    protected Component(String id)
    {
        if (id == null) {
            throw new NullPointerException("id is null");
        }

        this.id = id;
        component = null;
        componentConfigurator = null;
        isActivated = false;
        componentNamingRegistrations = new java.util.HashSet();
        providesNamingRegistrations = new java.util.Hashtable();        
        consumerNamingRegistrations = new java.util.Hashtable();        
    }



    /**
     * Returns the component's identifier.
     *
     * @return the component's identifier.
     */
    public String getId() {
        return id;
    }

    /**
     * Returns the component's parent assembly.
     *
     * @return the component's parent assembly.
     */
    public abstract AssemblyImpl getParentAssembly();
    
    /**
     * Returns true is the component has been activated.
     *
     * @return true is the component has been activated.
     */
    public boolean isActivated() {
        return isActivated;
    }

    /**
     * Returns the CCMObject correcsponding to the component.
     *
     * @return the CCMObject correcsponding to the component.
     *         (null is the component is not active)
     */
    public org.omg.Components.CCMObject getComponent() {
        return component;
    }

    /**
     * Returns the component's RepositoryId.
     *
     * @return the component's RepositoryId.
     */
    public abstract String getRepositoryId();

    
    /**
     * Set a Configurator to configure this component.
     * Replace any previously set Configurator.
     *
     * @param componentConfigurator Configurator to configure this component 
     */
    public void setComponentConfigurator(Configurator componentConfigurator) {
        this.componentConfigurator = componentConfigurator;
    }

    /**
     * Returns the Configurator set to this component.
     *
     * @return the Configurator set to this component or
     *         null if no Configurator was set.
     */
    public Configurator getComponentConfigurator() {
        return componentConfigurator;
    }

    /**
     * Adds a name for the component to be registered
     * into the naming service.
     *
     * @param name the registration name.
     */
    public void addComponentNamingRegistration(String name) {
        componentNamingRegistrations.add(name);
    }

    /**
     * Adds a name for a facet of the component to be 
     * registered into the naming service.
     *
     * @param providesId the identifier of the component's 
     *        facet to be registered.
     * @param name the registration name.
     */
    public void addProvidesNamingRegistration(String providesId, String name) {
        // get list of names for this facet
        java.util.Collection names = 
            (java.util.Collection) providesNamingRegistrations.get(providesId);
        
        // if not exist, create it
        if (names == null) {
            names = new java.util.HashSet();
            providesNamingRegistrations.put(providesId, names);
        }
        
        names.add(name);
    }

    /**
     * Adds a name for a consumer of the component to be 
     * registered into the naming service.
     *
     * @param consumerId the identifier of the component's 
     *        consumer to be registered.
     * @param name the registration name.
     */
    public void addConsumerNamingRegistration(String consumerId, String name) {
        // get list of names for this consumer
        java.util.Collection names = 
            (java.util.Collection) consumerNamingRegistrations.get(consumerId);
        
        // if not exist, create it
        if (names == null) {
            names = new java.util.HashSet();
            consumerNamingRegistrations.put(consumerId, names);
        }
        
        names.add(name);
    }
    
    /**
     * Returns a facet object of the component.
     *
     * @param providesId the identifier of the component's facet.
     *
     * @exception CreateFailure if the facet cannot be found.
     */
    private org.omg.CORBA.Object getProvides(String providesId)
        throws CreateFailure
    {
        // component must have been created
        cdmw.common.Assert.check(component != null);
        
        // get Navigation interface from component
        org.omg.Components.Navigation navig; 
        try {
            navig = org.omg.Components.NavigationHelper.narrow(component);
        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new CreateFailure(
                "Failed to get Navigation interface on component " + getId(),
                NOT_A_COMPONENT.value);
        }

        // get facet object from Navigation interface
        org.omg.CORBA.Object facetObj;
        try {
            facetObj = navig.provide_facet(providesId);
        } catch (org.omg.Components.InvalidName e) {
            throw new CreateFailure(
                "providesidentifier " + providesId +
                " not found on component " + id,
                PORT_NOT_FOUND.value);
        }

        return facetObj;    
    }


    /**
     * Returns a consumer object of the component.
     *
     * @param consumerId the identifier of the component's consumer.
     *
     * @exception CreateFailure if the consumer cannot be found.
     */
    private org.omg.CORBA.Object getConsumer(String consumerId)
        throws CreateFailure
    {
        // component must have been created
        cdmw.common.Assert.check(component != null);
        
        // get Events interface of consumesComponent
        org.omg.Components.Events events;
        try {
            events = org.omg.Components.EventsHelper.narrow(component);

        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new CreateFailure(
                "Failed to get Events interface on component " + getId(),
                NOT_A_COMPONENT.value);
        }
        
        // retrieve consumer from Events interface
        org.omg.Components.EventConsumerBase consumer;
        try {
            consumer = events.get_consumer(consumerId);
            
        } catch (org.omg.Components.InvalidName e) {
            throw new CreateFailure(
                "consumeridentifier " + consumerId +
                " not found on component " + getId(),
                PORT_NOT_FOUND.value);
        }
        
        return consumer;
    }


    /**
     * Activates the component.
     *
     * @exception CreateFailure if activation fails.
     */
    public abstract void activate()
            throws CreateFailure;

    /**
     * Deactivates the component.
     *
     * @exception CreateFailure if deactivation fails.
     */
    public abstract void deactivate()
            throws RemoveFailure;

    /**
     * Registers the component, its facets and its consumers into 
     * the Naming Service with any previously added registration name.
     *
     * @exception CreateFailure if registration fails.
     */
    protected void registerWithNaming() 
        throws CreateFailure
    {
        if (componentNamingRegistrations.size() > 0 ||
            providesNamingRegistrations.size() > 0  ||
            consumerNamingRegistrations.size() > 0  )
        {
            // get NameService
            org.omg.CosNaming.NamingContext nameServ;
            try {
                nameServ = AssemblyFactoryImpl.getInstance().getNamingService();
            } catch (Exception e) {
                throw new CreateFailure(
                    "Cannot contact NamingService: " + e.getMessage(),
                    NAMING_SERVICE_ERROR.value);
            }
            
            // create NamingInterface
            cdmw.commonsvcs.naming.NamingInterface naming = 
                new cdmw.commonsvcs.naming.NamingInterface(nameServ);
        
            // register component
            java.util.Iterator it = componentNamingRegistrations.iterator();
            while (it.hasNext()) {
                String name = (String) it.next();
                
                try {
                    naming.bind(name, component, true);
                } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                    throw new CreateFailure(
                        "NotFound exception during registration of component " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                    throw new CreateFailure(
                        "CannotProceed exception during registration of component " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                    throw new CreateFailure(
                        "InvalidName exception during registration of component " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
                    throw new CreateFailure(
                        "AlreadyBound exception during registration of component " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                }                
            }
            
            // register facets
            it = providesNamingRegistrations.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                
                // get providesId
                String providesId = (String) entry.getKey();
                // get list of names
                java.util.Collection names = 
                    (java.util.Collection) entry.getValue();
                
                // get facet object
                org.omg.CORBA.Object facet = getProvides(providesId);
                
                // register facet
                java.util.Iterator it2 = names.iterator();
                while(it2.hasNext()) {
                    String name = (String) it2.next();
                    
                    try {
                        naming.bind(name, facet, true);
                    } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                        throw new CreateFailure(
                            "NotFound exception during registration of provides " +
                            providesId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                        throw new CreateFailure(
                            "CannotProceed exception during registration of provides " +
                            providesId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                        throw new CreateFailure(
                            "InvalidName exception during registration of provides " +
                            providesId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
                        throw new CreateFailure(
                            "AlreadyBound exception during registration of provides " +
                            providesId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    }                
                } // end of while(it2.hasNext())  (register facet)
                
            } // end of while(it.hasNext())  (register facets)
            

            // register consumers
            it = consumerNamingRegistrations.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                
                // get consumerId
                String consumerId = (String) entry.getKey();
                // get list of names
                java.util.Collection names = 
                    (java.util.Collection) entry.getValue();
                
                // get consumer object
                org.omg.CORBA.Object consumer = getConsumer(consumerId);
                
                // register consumer
                java.util.Iterator it2 = names.iterator();
                while(it2.hasNext()) {
                    String name = (String) it2.next();
                    
                    try {
                        naming.bind(name, consumer, true);
                    } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                        throw new CreateFailure(
                            "NotFound exception during registration of consumer " +
                            consumerId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                        throw new CreateFailure(
                            "CannotProceed exception during registration of consumer " +
                            consumerId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                        throw new CreateFailure(
                            "InvalidName exception during registration of consumer " +
                            consumerId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
                        throw new CreateFailure(
                            "AlreadyBound exception during registration of consumer " +
                            consumerId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    }                
                } // end of while(it2.hasNext())  (register consumer)
                
            } // end of while(it.hasNext())  (register consumers)

        } // end of if (namingRegistrations.size() > 0)
    }


    /**
     * Unregisters the component, its facets and its consumers from 
     * the Naming Service.
     *
     * @exception CreateFailure if registration fails.
     */
    protected void unregisterFromNaming() 
        throws RemoveFailure
    {
        if (componentNamingRegistrations.size() > 0 ||
            providesNamingRegistrations.size() > 0  ||
            consumerNamingRegistrations.size() > 0  )
        {
            // get NameService
            org.omg.CosNaming.NamingContext nameServ;
            try {
                nameServ = AssemblyFactoryImpl.getInstance().getNamingService();
            } catch (Exception e) {
                throw new RemoveFailure(
                    "Cannot contact NamingService: " + e.getMessage(),
                    NAMING_SERVICE_ERROR.value);
            }
            
            // create NamingInterface
            cdmw.commonsvcs.naming.NamingInterface naming = 
                new cdmw.commonsvcs.naming.NamingInterface(nameServ);
        
            // unregister component
            java.util.Iterator it = componentNamingRegistrations.iterator();
            while (it.hasNext()) {
                String name = (String) it.next();
                
                try {
                    naming.unbind(name);
                } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                    // Component has already been unregistered by someone else ??
                    // Ignore.
                } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                    throw new RemoveFailure(
                        "CannotProceed exception during unregistration of component " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                    throw new RemoveFailure(
                        "InvalidName exception during unregistration of component " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                }                
            }
            
            // unregister facets
            it = providesNamingRegistrations.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                
                // get providesId
                String providesId = (String) entry.getKey();
                // get list of names
                java.util.Collection names = 
                    (java.util.Collection) entry.getValue();
                
                // unregister facet
                java.util.Iterator it2 = names.iterator();
                while(it2.hasNext()) {
                    String name = (String) it2.next();
                    
                    try {
                        naming.unbind(name);
                    } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                        // Facet has already been unregistered by someone else ??
                        // Ignore.
                    } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                        throw new RemoveFailure(
                            "CannotProceed exception during unregistration of provides " +
                            providesId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                        throw new RemoveFailure(
                            "InvalidName exception during unregistration of provides " +
                            providesId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    }                
                } // end of while(it2.hasNext())
                
            } // end of while(it.hasNext())
            
            // unregister consumers
            it = consumerNamingRegistrations.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                
                // get consumerId
                String consumerId = (String) entry.getKey();
                // get list of names
                java.util.Collection names = 
                    (java.util.Collection) entry.getValue();
                
                // unregister consumer
                java.util.Iterator it2 = names.iterator();
                while(it2.hasNext()) {
                    String name = (String) it2.next();
                    
                    try {
                        naming.unbind(name);
                    } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                        // Facet has already been unregistered by someone else ??
                        // Ignore.
                    } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                        throw new RemoveFailure(
                            "CannotProceed exception during unregistration of provides " +
                            consumerId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                        throw new RemoveFailure(
                            "InvalidName exception during unregistration of provides " +
                            consumerId + " from component " + id +
                            " with name " + name,
                            NAMING_SERVICE_ERROR.value); 
                    }                
                } // end of while(it2.hasNext())
                
            } // end of while(it.hasNext())

        } // end of if (namingRegistrations.size() > 0)
    }

    
}


