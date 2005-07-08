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
 * Base class for all servant providers
 *
 */
public abstract class ServantProviderBase 
    implements ServantProviderBaseInterface {

    private CleanupNotifier cleanupNotifier = null;

    /**
     * This method indicates whether the ServantProvider supports objects
     * with the specified Repository Id.
     *
     * @param repositoryId RepositoryID of an object
     *
     * @return <B>true</B> if objects with the specified repository ID are
     * supported by the ServantProvider, <B>false</B> otherwise.
     */
    public boolean supports(String repositoryId) {

        boolean result = false;
        String[] ids = getIds();
        for(int i = 0 ; i < ids.length ; ++i) {
          if( repositoryId.equals(ids[i]) ) {
              result = true;
              break;
          }
        }
        return result;

    }

    /**
     * This returns a sequence of all RepositoryIDs of objects supported by
     * the current ServantProvider.
     *
     * @return A sequence of RepositoryIds as a String table
     * supported by the ServantProvider.
     */
    public String[] getSupportedObjects() {
        return getIds();
    }

    // NOTA: notifier should not be destroyed!
    /**
     * Set the current cleanup notifier with the argument
     *
     * @param notifier A CleanupNotifier object
     */
    public void setCleanupNotifier(CleanupNotifier notifier) {
        this.cleanupNotifier = notifier;
    }

    /**
     * This operation be called by the user-defined servant provider whenever
     * an object is destroyed. It enables the Life Cycle Framework to perform
     * further cleanup operations following the destruction of a CDMW object.
     *
     * @param The ObjectId of the destroyed object
     */
    protected void cleanupObject(byte[] oid) {
        if (cleanupNotifier!=null) {
            this.cleanupNotifier.unregisterObject(oid);
        }
    }

    /**
     * This returns an array containing repository IDs of all objects
     * supported by the current ServantProvider.
     *
     * @return A String table containing all the IDs
     */
    protected abstract String[] getIds();

    /**
     * This returns the number of repository Ids supported
     * by the current ServantProvider.
     *
     * @return Number of repository Ids supported
     */
    protected abstract int getNumberOfIds();

}

