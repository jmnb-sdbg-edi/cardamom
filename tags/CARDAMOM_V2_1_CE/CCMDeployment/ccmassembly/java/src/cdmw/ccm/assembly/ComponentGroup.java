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

import org.omg.Components.Configurator;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.CREATE_FAILURE;

/**
 * Class representing a component group.
 * A ComponentGroup as a list of SingleComponent members.
 */
abstract class ComponentGroup extends Component 
{
    
    /**
     * ComponentGroup's parent assembly.
     */
    private AssemblyImpl parentAssembly;
    
    /**
     * Map of SingleComponent members, indexed by their identifier.
     */
    protected java.util.Map components;
    
    
    /**
     * Constructor.
     *
     * @param id the component group identifier as specified 
     *        into the .cad file.
     * @param parentAssembly the parent assembly of this
     *        ComponentGroup.
     */
    public ComponentGroup (String id,
                           AssemblyImpl parentAssembly)
    {
        super(id);
        
        if (parentAssembly == null) {
            throw new NullPointerException("parentAssembly is null");
        }
        this.parentAssembly = parentAssembly;
        parentAssembly.addComponentGroup(this);
        
        components = new cdmw.ccm.deployment.OrderedMap();
    }

    
    /**
     * Returns the ComponentGroup's parent assembly.
     *
     * @return the ComponentGroup's parent assembly.
     */
    public AssemblyImpl getParentAssembly() {
        return parentAssembly;
    }
    

    /**
     * Returns the ComponentGroup's RepositoryId.
     * (equals the first member's RepositoryId) 
     *
     * @return the ComponentGroup's RepositoryId
     *         or null if the group is empty.
     */
    public String getRepositoryId() {
        if (components.size() == 0) {
            return null;
        }
        
        SingleComponent firstComponent = 
            (SingleComponent) components.values().iterator().next();
        return firstComponent.getRepositoryId();
    }
    
    /**
     * Adds a SingleComponent as member of this group.
     * If the group is not empty, this component should have 
     * the same RepositoryId than the other members.
     *
     * @param component the member to be added to his group.
     * @return false if a component with the same identifier
     *         is already member of this group.
     * 
     * @exception CreateFailure if the component's RepositoryId
     *            is not specified or if it is not the same than
     *            other group members.
     */
    public boolean addComponent(SingleComponent component) 
        throws CreateFailure
    {
        System.out.println("      add " + component.getId() +
                           " to group " + getId());

        if (components.containsKey(component.getId())) {
            return false;
        }
        
        // check that component has its RepositoryId specified
        // (should have been loaded from .ccd file before)
        if (component.getRepositoryId() == null) {
            throw new CreateFailure(
                "Internal Error while adding component " + component.getId() +
                " to component group " + getId() + ": " +
                "Component has no RepositoryId specified !",
                CREATE_FAILURE.value);
        }
        
        // check that component's RepositoryId is the same that the group
        // If the group is empty, the component's RepositoryId becomes
        // also the group's RepositoryId.
        if (components.size() != 0) {
            if (!component.getRepositoryId().equals(getRepositoryId())) {
                throw new CreateFailure(
                    "Cannot add component " + component.getId() + 
                    " with RepositoryId " + component.getRepositoryId() +
                    " to component group " + getId() +
                    " with RepositoryId " + getRepositoryId(),
                    ERROR_IN_CAD.value);
            }
        }
        
        components.put(component.getId(), component);
        return true;
    }
    
    /**
     * Returns the SingleComponent member of this ComponentGroup 
     * with the specified identifier.
     *
     * @param componentId the SingleComponent's identifier
     * @return the SingleComponent with specified identifier or
     *         null if the group as no member with such identifier.
     */
    public SingleComponent getComponent(String componentId) {
        return (SingleComponent) components.get(componentId);
    }
    
    
    /**
     * Returns a java.util.Collection of all SingleComponent objects
     * which have been added to this ComponentGroup.
     *
     * @return a Collection of all SingleComponent of this ComponentGroup.
     */
    public java.util.Collection getComponents() {
        return components.values();
    }
    
    /**
     * Set a Configurator to this ComponentGroup.
     * The Configurator is set to all members of this group,
     * except if the member already has a Configurator.
     * (the configuration specified for a componentinstance as
     * priority over the group configuration)
     * 
     * @param configurator the Configurator to be applied
     *          to each member of this group.
     */
    public void setComponentConfigurator(Configurator configurator)
    {
        this.componentConfigurator = configurator;
        java.util.Iterator it = components.values().iterator();
        while (it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            
            // if component already has a configurator, print warning
            // but don't set configurator.
            if (comp.getComponentConfigurator() != null) {
                System.out.println(
                    "WARNING: component instance " + comp.getId() +
                    " has two configurations: one with component group " + getId() +
                    " and one specific to this instance." +
                    "For this component instance, the group configuration will be ignored.");
            } else {
                comp.setComponentConfigurator(configurator);
            }
        }
    }
    
    
    
    /**
     * Operation called by the assembly before any activation.
     *
     * @exception CreateFailure if activation fails.
     */
    public abstract void preactivate()
        throws CreateFailure;
    


    /**
     * Complete configuration of the group.
     *
     * @exception CreateFailure if a configuration_complete() fails
     */
    public abstract void completeConfiguration() 
        throws CreateFailure;
    
}

