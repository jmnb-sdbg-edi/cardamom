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


import java.util.regex.Pattern;


/**
 * Encapsulate an input request.
 */
public class RegexInputRequest extends InputRequest {


    // The regular expression pattern.
    private String m_pattern = "";


    /**
     * Constructor.
     *
     * @param prompt  The prompt to show to the user.
     * @param pattern The regular expression pattern.
     */
    public RegexInputRequest(String prompt, String pattern) {
        this(prompt, pattern, null);
    }


    /**
     * Constructor.
     *
     * @param prompt     The prompt to show to the user.
     * @param pattern    The regular expression pattern.
     * @param theDefault The default input value.
     */
    public RegexInputRequest(String prompt, String pattern, String theDefault) {
        super(prompt, false, theDefault);
        setPattern(pattern);
    }


    /**
     * Specify the regular expression pattern.
     *
     * @param pattern The regular expression pattern.
     */
    public void setPattern(String pattern) {
        if (pattern == null) {
            throw new IllegalArgumentException("pattern must not be null");
        }

        m_pattern = pattern;
    }


    /**
     * Return the regular expression pattern.
     * 
     * @return The possible values.
     */
    public Pattern getPattern() {
        return Pattern.compile(m_pattern);
    }


    /**
     * Test if the input is valid against the regex pattern.
     *
     * @return <code>true</code> if the input matches the pattern.
     */
    public boolean isInputValid() {
        if (super.isInputValid()) {
            return Pattern.matches(m_pattern, getInput().trim());
        } else {
            return false;
        }
    }


}

