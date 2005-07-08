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

/**
 * Base class for all Servant Provider allocators
 *
 */
public class ServantProviderAllocatorBase {

    private static java.util.Hashtable table = new java.util.Hashtable();

    /**
     * This operation returns a Servant Provider Allocator
     * registered with a given name.
     *
     * @param className Name of a registered Servant Provider Allocator
     *
     * @return A ServantProviderAllocatorBase instance
     */
    public static final ServantProviderAllocatorBase
        getAllocator(String className) {

        return (ServantProviderAllocatorBase) table.get(className);
                
    }


    /**
     * This operation returns a list containing names
     * of all Servant Provider Allocators within the current process.
     *
     * @return A string table containing all the names
     */
    public static String[] getAllNames() {

        String[] result = new String[table.size()];
        java.util.Iterator iterator = table.entrySet().iterator();
        int i = 0;
        for(; iterator.hasNext() ;) {
            java.util.Map.Entry entry = (java.util.Map.Entry) iterator.next();
            result[i] = (String) entry.getKey();
            i++;
        }
        return result;

    }

    /**
     * Constructor.
     *
     * @param className Name of the class of the allocator.
     */
    protected ServantProviderAllocatorBase(String className) {
        table.put(className,this);
    }

}

