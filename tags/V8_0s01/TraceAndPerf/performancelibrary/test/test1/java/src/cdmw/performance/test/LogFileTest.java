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

public class LogFileTest extends cdmw.testutils.Testable {

    private String perfStatFile;

    private boolean logCreationSuccess;

    private int maxTarget;

    public LogFileTest(
        String perfStatFile,
        boolean logCreationSuccess,
        int maxTarget) {
        super();
        this.out = System.out;
        this.name = "LogFile";

        this.perfStatFile = perfStatFile;
        this.logCreationSuccess = logCreationSuccess;
        this.maxTarget = maxTarget;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(13);
    	
        println("Test Performance library Log File");

        check(logCreationSuccess == true);

        // if log file creation successfull by ProbeCollector
        if (!logCreationSuccess) {
            return;
        }

        TargetThread target = new TargetThread();

        try {
            java.io.FileReader fileReader =
                new java.io.FileReader(perfStatFile);
            java.io.BufferedReader result =
                new java.io.BufferedReader(fileReader);

            String line;
            while ((line = result.readLine()) != null) {
                java.util.StringTokenizer st =
                    new java.util.StringTokenizer(line, " \t\n\r\f;");

                if (st.countTokens() == 0) {
                    continue;
                }

                if (st.countTokens() != 5) {
                    fail(
                        "Error in file '"
                            + perfStatFile
                            + "':\n"
                            + "  \""
                            + line
                            + "\" has not 5 expected arguments");
                }

                String apiName = st.nextToken();
                String minTimeStr = st.nextToken();
                String maxTimeStr = st.nextToken();
                String sumTimeStr = st.nextToken();
                String hitCountStr = st.nextToken();

                int apiIndex = target.getApiIndex(apiName);
                if (apiIndex == -1) {
                    fail("API name not found : " + apiName);
                    continue;
                }

                try {
                    long minTime = Long.parseLong(minTimeStr);
                    long maxTime = Long.parseLong(maxTimeStr);
                    long sumTime = Long.parseLong(sumTimeStr);
                    long hitCount = Long.parseLong(hitCountStr);

                    // sleep time is returned in milli sec 
                    // and is converted in micro sec
                    long targTime = target.getApiSleepTime(apiIndex) * 1000;
                    int targHitCount = maxTarget * target.getApiCount(apiIndex);
                    long targSumTime = targTime * targHitCount;

                    // Strange behaviour on Sun bi-processor:
                    // when sleep(100), may do a real sleep of ~95 !!
                    // => accept error of 10000 on minTime.
                    if (minTime < (targTime - 10000)
                        || minTime > (targTime + 200000000)) {
                        fail(
                            "error for api "
                                + apiName
                                + " in min time : "
                                + minTime);
                    } else {
                        succeed();
                    }

                    if (maxTime < (targTime - 10000)
                        || maxTime > (targTime + 200000000)
                        || maxTime < minTime) {
                        fail(
                            "error for api "
                                + apiName
                                + " in max time : "
                                + maxTime);
                    } else {
                        succeed();
                    }

                    if (sumTime < (targSumTime - hitCount * 10000)
                        || sumTime > (targSumTime + 1000000000)) {
                        fail(
                            "error for api "
                                + apiName
                                + " in sum time : "
                                + sumTime);
                    } else {
                        succeed();
                    }

                    if (hitCount != targHitCount) {
                        fail(
                            "error for api "
                                + apiName
                                + " in hit count : "
                                + hitCount);
                    } else {
                        succeed();
                    }

                } catch (NumberFormatException e) {
                    fail("Error for '" + line + "' : a value is not a long");
                }

            } // while

        } catch (java.io.FileNotFoundException e) {
            fail("Perf file '" + perfStatFile + "' not found");
        } catch (java.io.IOException e) {
            fail("Error while reading " + perfStatFile);
            e.printStackTrace(out);
        }

    }

}

