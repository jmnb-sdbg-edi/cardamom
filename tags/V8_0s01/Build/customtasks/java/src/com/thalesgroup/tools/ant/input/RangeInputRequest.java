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


import java.util.StringTokenizer;


/**
 * Encapsulate an input request.
 */
public class RangeInputRequest extends RegexInputRequest {


    // The smallest valid integer value.
    private int m_lowerBound;

    // The biggest valid integer value.
    private int m_upperBound;


    /**
     * Constructor.
     *
     * @param prompt     The prompt to show to the user.
     * @param pattern    The regular expression pattern.
     * @param lowerBound The smallest valid integer value.
     * @param upperBound The biggest valid integer value.
     */
    public RangeInputRequest(String prompt,
                             String pattern,
                             int lowerBound,
                             int upperBound) {

        this(prompt, pattern, lowerBound, upperBound, null);
    }


    /**
     * Constructor.
     *
     * @param prompt     The prompt to show to the user.
     * @param pattern    The regular expression pattern.
     * @param lowerBound The smallest valid integer value.
     * @param upperBound The biggest valid integer value.
     * @param theDefault The default input value.
     */
    public RangeInputRequest(String prompt,
                             String pattern,
                             int lowerBound,
                             int upperBound,
                             String theDefault) {
    
        super(prompt, pattern, theDefault);

        if (upperBound < lowerBound) {
            throw new IllegalArgumentException(
                "upperBound must be >= lowerBound");
        }

        setLowerBound(lowerBound);
        setUpperBound(upperBound);
    }


    /**
     * Specify the lower bound.
     *
     * @param lowerBound The lower bound value.
     */
    public void setLowerBound(int lowerBound) {
        if (lowerBound < 0) {
            throw new IllegalArgumentException("lowerBound must be >= 0");
        }

        m_lowerBound = lowerBound;
    }


    /**
     * Return the lower bound.
     *
     * @return The lower bound.
     */
    public int getLowerBound() {
        return m_lowerBound;
    }


    /**
     * Specify the upper bound.
     *
     * @param upperBound The upper bound value.
     */
    public void setUpperBound(int upperBound) {
        if (upperBound < 0) {
            throw new IllegalArgumentException("upperBound must be >= 0");
        }

        m_upperBound = upperBound;
    }


    /**
     * Return the upper bound.
     *
     * @return The upper bound.
     */
    public int getUpperBound() {
        return m_upperBound;
    }


    /**
     * Test if the input is valid.
     *
     * @return <code>true</code> if the input matches the pattern and contains
     *         values within the lower and upper bounds.
     */
    public boolean isInputValid() {
        boolean isValid = true;

        if (super.isInputValid()) {
            // A valid input could be "1; 3;5-12 ;4".
            StringTokenizer st = new StringTokenizer(getInput().trim(), ";");
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();

                if (token.indexOf("-") > -1) {
                    // Range of numbers, extract the min and the max values.
                    StringTokenizer st2 = new StringTokenizer(token, "-");

                    int min = new Integer(st2.nextToken()).intValue();
                    int max = new Integer(st2.nextToken()).intValue();

                    if ((min < m_lowerBound) || (max > m_upperBound) ||
                        (min > max)) {

                        isValid = false;
                        break;
                    }
                } else {
                    // Simple number.
                    int value = new Integer(token).intValue();
                    if ((value < m_lowerBound) || (value > m_upperBound)) {
                        isValid = false;
                        break;
                    }
                }
            }
        } else {
            isValid = false;
        }

        return isValid;
    }


}

