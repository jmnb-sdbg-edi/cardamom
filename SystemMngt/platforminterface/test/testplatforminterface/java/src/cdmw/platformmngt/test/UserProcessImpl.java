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


package cdmw.platformmngt.test;

import cdmw.platformmngt.ProcessImpl;
import cdmw.platformmngt.ProcessBehaviour;
import com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo;
import com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointMonitoringModel;

/**
 * User specific Process implementation.
 *
 */
public class UserProcessImpl extends ProcessImpl {

    private ActivityPointInfo[] pointInfos;

    public UserProcessImpl(ProcessBehaviour behaviour) {
        super(behaviour);
    }
    
    public UserProcessImpl() {
        super(new DefaultBehaviour());
    }
    
    public int nb_steps() {
        return 1;
    }

    public int nb_activity_points() {
        return pointInfos.length;
    }

    public com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo
        get_activity_point(int pointIndex)
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.OutOfRange {
        if (pointIndex >= 0 && pointIndex < pointInfos.length) {
            return pointInfos[pointIndex];
        } else {
            throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.OutOfRange();
        }
    }

    public com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo[]
        get_all_activity_points() {
        return pointInfos;
    }

    public org.omg.CORBA.Object get_service() {
        return null;
    }

    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
        pointInfos = new ActivityPointInfo[1];
        ActivityPointInfo info = new ActivityPointInfo(
            ActivityPointMonitoringModel.PUSH_MONITORING_MODEL, null);
        pointInfos[0] = info;
    }
    
    public void next_step() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep();
    }

    public void run() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {
        // do nothing
    }

    public void stop() {
        // do nothing
    }
    
    static class DefaultBehaviour extends ProcessBehaviour {

        public DefaultBehaviour() {
            // do nothing
        }

        public void initialise(
            com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
            System.out.println("initialise requested");
        }

        public void run() 
            throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {
            System.out.println("run requested");
        }

        public void stop() {
            System.out.println("stop requested");
        }

    }

}

