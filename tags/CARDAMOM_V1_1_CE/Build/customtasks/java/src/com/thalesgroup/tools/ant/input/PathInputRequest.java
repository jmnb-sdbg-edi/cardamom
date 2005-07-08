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


package com.thalesgroup.tools.ant.input;


import com.thalesgroup.tools.ant.util.FileUtils;

import java.io.File;
import java.io.IOException;

import java.util.Vector;


/*
 * Encapsulates an input request.
 */
public class PathInputRequest extends InputRequest {


    // The default value.
    private File m_default = null;

    // Directory only flag.
    private boolean m_dirPathOnly = false;


    /**
     * Constructor.
     *
     * @param prompt      The message to display.
     * @param dirPathOnly If <code>true</code>, only directories are
     *                    considered as valid path.
     */
    public PathInputRequest(String prompt, boolean dirPathOnly) {
        this(prompt, dirPathOnly, null);
    }


    /**
     * Constructor.
     *
     * @param prompt      The message to display.
     * @param dirPathOnly If <code>true</code>, only directories are
     *                    considered as valid path.
     * @param theDefault  The default path value.
     */
    public PathInputRequest(String prompt,
                            boolean dirPathOnly,
                            String theDefault) {

        super(prompt, false, theDefault);

        m_dirPathOnly = dirPathOnly;

        if (theDefault != null) {
            m_default = new File(theDefault);
        }
    }


    /**
     * Test if the input is a valid path.
     *
     * @return <code>true</code> if the input is a valid path.
     */
    public boolean isInputValid() {
        boolean isValid = false;

        if (super.isInputValid()) {
            File f = new File(getInput());

            if (f.exists()) {
                if ((m_dirPathOnly && f.isDirectory()) ||
                   (!m_dirPathOnly && f.isFile())) {

                    try {
                        boolean isLnk = FileUtils.isLink(f);
                        setInput(isLnk ? f.getAbsolutePath()
                                       : f.getCanonicalPath());
                    } catch (IOException ioe) {
                        ioe.printStackTrace();
                    }

                    isValid = true;
                }
            } else {
                if (m_dirPathOnly) {
                    // Ask the user if he wants the directory to be created
                    Vector yesOrNo = new Vector();
                    yesOrNo.add("y");
                    yesOrNo.add("n");

                    StringBuffer sb = new StringBuffer();

                    try {
                        sb.append("The directory " + f.getCanonicalPath());
                        sb.append(" does not exist, do you want");
                        sb.append(" to create it ? ");

                        InputRequest requestCreate =
                        new MultipleChoiceInputRequest(sb.toString(),
                                                       yesOrNo, "n");

                        // Wait for the answer.
                        InputHandlerImpl inputHandler = new InputHandlerImpl();
                        inputHandler.handleInput(requestCreate);

                        if (requestCreate.getInput().equals("y")) {
                            f.mkdirs();
                            setInput(f.getCanonicalPath());
                            isValid = true;
                        }
                    } catch (IOException ioe) {
                        ioe.printStackTrace();
                    }
                }
            }
        }

        return isValid;
    }


}

