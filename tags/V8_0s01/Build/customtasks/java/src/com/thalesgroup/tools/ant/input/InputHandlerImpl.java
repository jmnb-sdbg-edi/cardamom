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


package com.thalesgroup.tools.ant.input;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import java.util.Iterator;

import org.apache.tools.ant.BuildException;


/**
 * Prompt on System.err, read input from System.in.
 */
public class InputHandlerImpl {


    /**
     * Default constructor.
     */
    public InputHandlerImpl() {
    }


    /**
     * Prompt and request input.
     *
     * @param request The input request handler.
     */
    public void handleInput(InputRequest request) throws BuildException {
        String prompt = getPrompt(request);
        
        BufferedReader in = 
            new BufferedReader(new InputStreamReader(getInputStream()));

        do {
            System.err.println(prompt);
            
            try {
                String input = in.readLine();
                request.setInput(input);
            } catch (IOException e) {
                throw new BuildException(
                    "Failed to read input from Console.", e);
            }
        } while (!request.isInputValid());
    }
    

    /**
     * Construct user prompt from a request.
     *
     * @param request The request to construct the prompt for.
     */
    protected String getPrompt(InputRequest request) {
        String prompt = request.getPrompt();
        String theDefault = request.getDefault();

        if (request instanceof MultipleChoiceInputRequest) {
            StringBuffer sb = new StringBuffer(prompt);
            sb.append("(");

            Iterator iter =
                ((MultipleChoiceInputRequest) request).getChoices().iterator();

            boolean first = true;
            while (iter.hasNext()) {
                if (!first) {
                    sb.append(",");
                }
                sb.append(iter.next());
                first = false;
            }
            
            sb.append(")");
            prompt = sb.toString();
        }

        if (theDefault != null) {
            prompt += " [" + theDefault + "]";
        }

        return prompt;
    }


    /**
     * Return the input stream from which the user input should be read.
     */
    protected InputStream getInputStream() {
        return System.in;
    }
    

}

