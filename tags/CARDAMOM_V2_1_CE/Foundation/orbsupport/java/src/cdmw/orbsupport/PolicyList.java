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


package cdmw.orbsupport;

import org.omg.CORBA.Policy;

/**
 * This class defines a list of policy.
 *
 * Features:
 * Thread safe, exception safe
 *
 * @see  ORBSupport
 *
 */
public class PolicyList {

    /**
     * List of the policies.
     */
    private java.util.Vector list = new java.util.Vector();

    /**
     * Constructor
     */
    public PolicyList() {
        // nothing to do
    }
    
    /**
     * Constructor
     *
     * @param policies Array of policies
     */
    public PolicyList(Policy[] policies) {

        for(int i = 0; i<policies.length; i++) {
            list.add(policies[i]);
        }

    }

    /**
     * Copy constructor
     *
     * @param v List of the policies to be copied
     */
    public PolicyList(java.util.Vector v) {

         this.list = v;

    }

    /**
     * Return the list of the policies.
     *
     * @return the policy list as a table.
     */
    public synchronized Policy[] getList() {

        Policy[] result = new Policy[list.size()];
        int ind = 0;
        for (java.util.Enumeration e = list.elements() ;e.hasMoreElements();) {
            result[ind] = (Policy) e.nextElement();
            ind++;
        }
        return result;

    }

    /**
     * Add a policy to the list.
     *
     * @param p Policy to be added.
     */
    public void insert(Policy p) {
        list.add(p);
    }

    /**
     * Remove a policy from the list.
     *
     * @param p Policy to be removed.
     *
     * @return true if the list contained the specified element
     */
    public boolean remove(Policy p) {
        return list.remove(p);
    }

    /**
     * Make a copy of the list.
     *
     * @return a new PolicyList object which is a copy of the argument.
     */
    public PolicyList copy() {
        return new PolicyList((java.util.Vector) this.list.clone());
    }

}

