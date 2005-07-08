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


import java.util.Vector;


/**
 * Encapsulate an input request.
 */
public class MultipleChoiceInputRequest extends InputRequest {


    // The list of choices.
    private Vector m_choices = new Vector();
    

    /**
     * Constructor.
     *
     * @param prompt  The prompt to show to the user.
     * @param choices Holds all input values that are allowed.
     */
    public MultipleChoiceInputRequest(String prompt, Vector choices) {
        this(prompt, choices, null);
    }


    /**
     * Constructor.
     *
     * @param prompt     The prompt to show to the user.
     * @param choices    Holds all input values that are allowed.
     * @param theDefault The default input value.
     */
    public MultipleChoiceInputRequest(String prompt,
                                      Vector choices,
                                      String theDefault) {

        super(prompt, false, theDefault);
        
        setChoices(choices);

        if ((theDefault != null) && !theDefault.equals("")) {
            if (!m_choices.contains(theDefault)) {
                throw new IllegalArgumentException(
                    "default value must be a valid argument");
            }
        }
    }


    /**
     * Set the possible values.
     *
     * @param choices The possible values.
     */
    public void setChoices(Vector choices) {
        if (choices == null) {
            throw new IllegalArgumentException("choices must not be null");
        }

        m_choices = choices;
    }


    /**
     * Return the possible values.
     *
     * @return The possible values.
     */
    public Vector getChoices() {
        return m_choices;
    }
    

    /**
     * Check the user input.
     *
     * @return <code>true</code> if the input is one of the allowed values.
     */
    public boolean isInputValid() {
        if (super.isInputValid()) {
            return m_choices.contains(getInput());
        } else {
            return false;
        }
    }


}

