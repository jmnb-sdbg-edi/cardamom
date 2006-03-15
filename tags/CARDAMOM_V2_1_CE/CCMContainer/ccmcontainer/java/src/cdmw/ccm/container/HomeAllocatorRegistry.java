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


package cdmw.ccm.container;


/**
 * A Registry/Repository for home allocators
 *
 */
public class HomeAllocatorRegistry {

    private static java.util.Hashtable registry =
        new java.util.Hashtable();

    /**
     * Registers a home allocator using id identifier.
     *
     * @param id Identifier for the registration.
     * @param allocator A pointer to a home allocator object.
     * @return <B>true</B> if the registration was successful;
     * <B>false</B> otherwise
     * (if there already was a home allocator registered with the same id).
     */
    public static boolean register(String id, HomeAllocatorBase allocator) {
        try {
            registry.put(id, allocator);
            return true;
        } catch(NullPointerException npe) {
            return false;
        }
    }


    /**
     * Unregisters the home allocator for the given identifier.
     *
     * @param id Identifier to a previously registered home allocator.
     * @return <B>true</B> if id was previously registered;
     * <B>false</B> otherwise.
     */
    public static boolean unregister(String id) {
        return registry.remove(id) == null ? false : true;
    }

    /**
     * Returns a home allocator with the given identifier.
     *
     * @param id Identifier of the home allocator
     * @return A HomeAllocatorBase instance or null if not found.
     */
    public static HomeAllocatorBase getAllocator(String id) {
        try {
            return (HomeAllocatorBase) registry.get(id);
        } catch(ClassCastException cce) {
            return null;
        }
    }

    /**
     * Returns all registered identifiers in an array.
     *
     * @return An array containing all the registered identifiers.
     */
    public static String[] getAllIds() {
        java.util.Enumeration e = registry.keys();
        String[] ids = new String[registry.size()];
        int i = 0;
        for ( ; e.hasMoreElements(); ) {
            ids[i++] = (String) e.nextElement();
        }
        return ids;
    }

}

