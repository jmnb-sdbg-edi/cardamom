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


/**
 * Encapsulate an input request.
 */
public class InputRequest {


    // The prompt.
    private String m_prompt = null;

    // The user input.
    private String m_input = null;

    // The default input value.
    private String m_default = null;

    // Allow empty string as valid input.
    private boolean m_allowEmptyString = false;
    

    /**
     * Constructor.
     *
     * @param prompt The prompt to be displayed.
     */
    public InputRequest(String prompt) {
        this(prompt, true, null);
    }


    /**
     * Constructor.
     *
     * @param prompt           The prompt to be displayed.
     * @param allowEmptyString If <code>true</code> then an empty string is
     *                         considered as valid input.
     */
    public InputRequest(String prompt, boolean allowEmptyString) {
        this(prompt, allowEmptyString, null);
    }


    /**
     * Constructor.
     *
     * @param prompt           The prompt to be displayed.
     * @param allowEmptyString If <code>true</code> then an empty string is
     *                         considered as valid input.
     * @param theDefault       The default input value.
     */
    public InputRequest(String prompt,
                        boolean allowEmptyString,
                        String theDefault) {

        setPrompt(prompt);
        setAllowEmptyString(allowEmptyString);
        setDefault(theDefault);
    }


    /**
     * Set the prompt text.
     *
     * @param prompt The prompt text.
     */
    public void setPrompt(String prompt) {
        if (prompt == null) {
            throw new IllegalArgumentException("prompt must not be null");
        }

        m_prompt = prompt;
    }


    /**
     * Return the prompt text.
     *
     * @return The prompt text.
     */
    public String getPrompt() {
        return m_prompt;
    }


    /**
     * Set the allow empty string flag value.
     *
     * @param allowEmptyString If <code>true</code> then an empty string is
     *                         considered as valid input.
     */
    public void setAllowEmptyString(boolean allowEmptyString) {
        m_allowEmptyString = allowEmptyString;
    }


    /**
     * Get the allow empty string flag value.
     *
     * @return The allow empty string flag value.
     */
    public boolean getAllowEmptyString() {
        return m_allowEmptyString;
    }


    /**
     * Set the default input value.
     *
     * @param theDefault The default input value.
     */
    public void setDefault(String theDefault) {
        if ((theDefault != null) && !theDefault.equals("")) {
            m_default = theDefault;
        }
    }


    /**
     * Return the default input value.
     *
     * @return The default input value.
     */
    public String getDefault() {
        return m_default;
    }


    /**
     * Set the user input.
     *
     * @param input The user input.
     */
    public void setInput(String input) {
        if (input.equals("") && (m_default != null)) {
            m_input = m_default;
        } else {
            m_input = input;
        }
    }


    /**
     * Return the user input.
     *
     * @return The user input.
     */
    public String getInput() {
        return m_input;
    }


    /**
     * Check the user input.
     *
     * @return <code>true</code> if the input value is valid.
     */
    public boolean isInputValid() {
        return (!m_allowEmptyString && m_input.equals("")) ? false : true;
    }


}

