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


package cdmw.lifecycle;

/**
 * This class provides a default implementation for a Cleanup
 * Notifier that will be called by Servant Providers whenever an object
 * is destroyed. The notifier informs/notifies the LifeCycle Framework
 * of the deletion of the object and performs necessary actions to cleanup
 * the Naming and Repository service.
 * The Life Cycle Framework uses one instance of a CleanupNotifier per
 * CDMW factory (non user defined).
 * This default implementation does nothing.
 *
 * @see EntityCleanupNotifier
 *
 */
public class CleanupNotifier {

    /**
     * The purpose of this operation is to unregister the Object of the
     * specified object Id.
     *
     * @param oid The Object Id of the object.
     */
    public void unregisterObject(byte[] oid) {
        // Default does nothing
    }

}

