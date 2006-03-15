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


package cdmw.performance.test;

import cdmw.testutils.Testable;

public class TargetThread extends Thread {

    private static final int APINBR = 3;

    private static final String[] targetApiName =
        { "TargetApi_1", "TargetApi_2", "TargetApi_3" };

    private static final int[] targetApiCount = { 2, 3, 4 };

    private static final int[] targetApiSleep = { 100, 200, 300 };

    public TargetThread() {
        super();
    }

    public int getApiNbr() {
        return APINBR;
    }

    public String getApiName(int apiIndex) {
        if (apiIndex >= 0 && apiIndex < APINBR) {
            return targetApiName[apiIndex];
        }

        return null;
    }

    public int getApiIndex(String apiName) {
        for (int i = 0; i < APINBR; i++) {
            if (targetApiName[i].compareTo(apiName) == 0) {
                return i;
            }
        }
        return -1;
    }

    public int getApiCount(int apiIndex) {
        if (apiIndex >= 0 && apiIndex < APINBR) {
            return targetApiCount[apiIndex];
        }

        return -1;
    }

    public int getApiSleepTime(int apiIndex) {
        if (apiIndex >= 0 && apiIndex < APINBR) {
            return targetApiSleep[apiIndex];
        }

        return -1;
    }

    public void run() {
        for (int i = 0; i < targetApiCount[0]; i++) {
            targetApi1();
        }

        for (int i = 0; i < targetApiCount[1]; i++) {
            targetApi2();
        }

        for (int i = 0; i < targetApiCount[2]; i++) {
            targetApi3();
        }

    }

    private void targetApi1() {
        try {
            cdmw.performance.Probe probe =
                new cdmw.performance.Probe(targetApiName[0]);

            cdmw.ossupport.OS.sleep(Testable.getTimescale() * targetApiSleep[0]);

            probe.stop();
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void targetApi2() {
        try {
            cdmw.performance.Probe probe = 
                new cdmw.performance.Probe(targetApiName[1]);

            cdmw.ossupport.OS.sleep(Testable.getTimescale() * targetApiSleep[1]);

            probe.stop();
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void targetApi3() {
        try {
            cdmw.performance.Probe probe =
                new cdmw.performance.Probe(targetApiName[2]);

            cdmw.ossupport.OS.sleep(Testable.getTimescale() * targetApiSleep[2]);

            probe.stop();
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

}

