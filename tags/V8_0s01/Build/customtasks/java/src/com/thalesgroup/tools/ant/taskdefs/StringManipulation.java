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


package com.thalesgroup.tools.ant.taskdefs;


import com.thalesgroup.tools.ant.util.StringUtils;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;

import java.util.Iterator;
import java.util.StringTokenizer;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.filters.util.ChainReaderHelper;
import org.apache.tools.ant.taskdefs.Property;
import org.apache.tools.ant.types.FilterChain;


/**
 * Provide several string manipulations in a single ANT task.
 */
public class StringManipulation extends Task {


    // The property to be created.
    private String m_property = null;

    // The source string.
    private String m_string = null;

    // The substring to look for when action="occurrences" or "replace".
    private String m_substring = null;

    // The new value when action="replace".
    private String m_replaceWith = null;

    // Specify whether substring is a word.
    private boolean m_isWord = false;

    // Specify the delimiter.
    private String m_delimiter = null;

    // The action to be executed.
    private String m_action = null;

    // The begin index when action="substring".
    private int m_beginIndex = 0;

    // The end index when action="substring".
    private int m_endIndex = 0;

    // List of actions.
    private Vector m_actions = new Vector();

    // Holds FilterChains.
    private Vector filterChains = new Vector();

    // Allow property overriding.
    private boolean m_override = false;


    /**
     * Specify the property to be created.
     *
     * @param property The property to be created.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Get the property to be created.
     *
     * @return The property to be created.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Specify the source string.
     *
     * @param string The source string.
     */
    public void setString(String string) {
        m_string = string;
    }


    /**
     * Get the source string.
     *
     * @return The source string.
     */
    public String getString() {
        return m_string;
    }


    /**
     * Specify the substring to count.
     *
     * @param substring The substring to count.
     */
    public void setSubstring(String substring) {
        m_substring = substring;
    }


    /**
     * Get the substring to count.
     *
     * @return The substring count.
     */
    public String getSubstring() {
        return m_substring;
    }


    /**
     * Specify the replacement string.
     *
     * @param replaceWith The replacement string.
     */
    public void setReplaceWith(String replaceWith) {
        m_replaceWith = replaceWith;
    }


    /**
     * Get the replacement string.
     *
     * @return The replacement string.
     */
    public String getReplaceWith() {
        return m_replaceWith;
    }


    /**
     * Specify whether substring is a word.
     *
     * @param isWord If <code>true</code> then substring is a word.
     */
    public void setIsWord(boolean isWord) {
        m_isWord = isWord;
    }


    /**
     * Get the isWord flag value.
     *
     * @return The isWord flag value.
     */
    public boolean getIsWord() {
        return m_isWord;
    }


    /**
     * Specify the delimiter.
     *
     * @param delimiter The delimiter.
     */
    public void setDelimiter(String delimiter) {
        m_delimiter = delimiter;
    }


    /**
     * Get the delimiter.
     *
     * @return The delimiter.
     */
    public String getDelimiter() {
        return m_delimiter;
    }


    /**
     * Specify the action to be executed.
     *
     * @param action The action to be executed.
     */
    public void setAction(String action) {
        m_action = action;
    }


    /**
     * Get the action to be executed.
     *
     * @return The action to be executed.
     */
    public String getAction() {
        return m_action;
    }


    /**
     * Set the begin index.
     *
     * @param beginIndex The value.
     */
    public void setBeginIndex(int beginIndex) {
        m_beginIndex = beginIndex;
    }


    /**
     * Get the begin index.
     *
     * @return The begin index.
     */
    public int getBeginIndex() {
        return m_beginIndex;
    }


    /**
     * Set the end index.
     *
     * @param endIndex The value.
     */
    public void setEndIndex(int endIndex) {
        m_endIndex = endIndex;
    }


    /**
     * Get the end index.
     *
     * @return The end index.
     */
    public int getEndIndex() {
        return m_endIndex;
    }


    /**
     * Set the override flag value.
     *
     * @param override The override flag value.
     */
    public void setOverride(boolean override) {
        m_override = override;
    }


    /**
     * Return the override flag value.
     *
     * @return The override flag value.
     */
    public boolean getOverride() {
        return m_override;
    }


    /**
     * Add the FilterChain element.
     *
     * @param filter The filter chain.
     */
    public void addFilterChain(FilterChain filter) {
        filterChains.addElement(filter);
    }


    /**
     * Default constructor.
     */
    public StringManipulation() {
        super();

        m_actions.add("lowercase");
        m_actions.add("uppercase");
        m_actions.add("trim");
        m_actions.add("occurrences");
        m_actions.add("replace");
        m_actions.add("normalizespaces");
        m_actions.add("removeduplicates");
        m_actions.add("counttokens");
        m_actions.add("length");
        m_actions.add("substring");
        m_actions.add("removetoken");
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        // Apply the filters to the string prior to any actions.
        applyFilters();

        String result = "";

        if (m_action.equals("lowercase")) {
            result = runActionLowercase();
        } else if (m_action.equals("uppercase")) {
            result = runActionUppercase();
        } else if (m_action.equals("trim")) {
            result = runActionTrim();
        } else if (m_action.equals("occurrences")) {
            result = runActionOccurrences();
        } else if (m_action.equals("replace")) {
            result = runActionReplace();
        } else if (m_action.equals("normalizespaces")) {
            result = runActionNormalizeSpaces();
        } else if (m_action.equals("removeduplicates")) {
            result = runActionRemoveDuplicates();
        } else if (m_action.equals("counttokens")) {
            result = runActionCountTokens();
        } else if (m_action.equals("length")) {
            result = runActionLength();
        } else if (m_action.equals("substring")) {
            result = runActionSubstring();
        } else if (m_action.equals("removetoken")) {
            result = runActionRemoveToken();
        }

        if (m_override) {
            if (getProject().getUserProperty(m_property) == null) {
                getProject().setProperty(m_property, result);
            } else {
                getProject().setUserProperty(m_property, result);
            }
        } else {
            Property p = (Property) getProject().createTask("property");
            p.setName(m_property);
            p.setValue(result);
            p.execute();
        }
    }


    /**
     * Apply the filters.
     */
    protected void applyFilters() {
        Reader instream = null;

        try {
            instream = new StringReader(m_string);

            // Apply all the filters on the string.
            ChainReaderHelper crh = new ChainReaderHelper();
            crh.setBufferSize(m_string.length());
            crh.setPrimaryReader(instream);
            crh.setFilterChains(filterChains);
            crh.setProject(getProject());
            instream = crh.getAssembledReader();

            String text = crh.readFully(instream);

            if (text != null) {
                if (text.length() > 0) {
                    m_string = text;
                }
            }
        } catch (IOException ioe) {
            throw new BuildException(ioe, getLocation());
        } finally {
            try {
                if (instream != null) {
                    instream.close();
                }
            } catch (IOException ioex) {
                //ignore
            }
        }
    }


    /**
     * Lowercase a string.
     *
     * @return The string in lowercase.
     */
    protected String runActionLowercase() {
        return m_string.toLowerCase();
    }


    /**
     * Uppercase a string.
     *
     * @return The string in uppercase.
     */
    protected String runActionUppercase() {
        return m_string.toUpperCase();
    }


    /**
     * Trim a string.
     *
     * @return The trimmed string.
     */
    protected String runActionTrim() {
        return m_string.trim();
    }


    /**
     * Count the number of occurrences of a substring in a string.
     *
     * @return The number of occurrences of a substring.
     */
    protected String runActionOccurrences() {
        if ((m_substring == null) || m_substring.equals("")) {
            throw new BuildException("attribute substring must not be null.");
        }

        int count = 0;

        if (m_isWord) {
            StringTokenizer st = null;
            if (m_delimiter != null) {
                st = new StringTokenizer(m_string, m_delimiter);
            } else {
                st = new StringTokenizer(m_string);
            }

            while (st.hasMoreTokens()) {
                if (m_substring.equals(st.nextToken())) {
                    count++;
                }
            }
        } else {
            int pos = 0;
            do {
                pos = m_string.indexOf(m_substring, pos);

                if (pos >= 0) {
                    count++;
                    pos++;
                }
            } while (pos >= 0);
        }

        return String.valueOf(count);
    }


    /**
     * Replace a substring by another.
     *
     * @return The modified string.
     */
    protected String runActionReplace() {
        if ((m_substring == null) || m_substring.equals("")) {
            throw new BuildException("attribute substring must not be null.");
        }

        if ((m_replaceWith == null) || m_replaceWith.equals("")) {
            throw new BuildException("attribute replaceWith must not be null.");
        }

        return m_string.replaceAll(m_substring, m_replaceWith);
    }


    /**
     * Normalize the spaces in the string.
     *
     * @return The normalized string.
     */
    protected String runActionNormalizeSpaces() {
        return StringUtils.normalizeSpaces(m_string);
    }


    /**
     * Remove the duplicate tokens in a string.
     *
     * @return The string without duplicate tokens.
     */
    protected String runActionRemoveDuplicates() {
        Vector v = new Vector();

        StringTokenizer st = null;
        if (m_delimiter != null) {
            st = new StringTokenizer(m_string, m_delimiter);
        } else {
            st = new StringTokenizer(m_string);
        }
        while (st.hasMoreTokens()) {
            String token = st.nextToken();

            if (!v.contains(token)) {
                v.add(token);
            }
        }

        StringBuffer sb = new StringBuffer();
        Iterator iter = v.iterator();
        while (iter.hasNext()) {
            sb.append((String) iter.next());
            sb.append((m_delimiter == null) ? " " : m_delimiter);
        }

        return sb.toString().trim();
    }


    /**
     * Return the number of tokens in a string.
     *
     * @return The number of tokens.
     */
    protected String runActionCountTokens() {
        int n = 0;

        StringTokenizer st = null;
        if (m_delimiter != null) {
            st = new StringTokenizer(m_string, m_delimiter);
        } else {
            st = new StringTokenizer(m_string);
        }
        while (st.hasMoreTokens()) {
            st.nextToken();
            n++;
        }

        return String.valueOf(n);
    }


    /**
     * Return the length of the string.
     *
     * @return The length of the string.
     */
    protected String runActionLength() {
        return String.valueOf(m_string.length());
    }


    /**
     * Return a new string that is a substring of this string.
     *
     * @return A new string that is a substring of this string.
     */
    protected String runActionSubstring() {
        if (m_endIndex > m_beginIndex) {
            throw new BuildException(
                "endIndex must be set to a value " +
                "equal or greater than beginIndex.");
        }

        return m_string.substring(m_beginIndex, m_endIndex);
    }


    /**
     * Return a new string that is not containing the substring
     *
     * @return A new string that is not containing the substring
     */
    protected String runActionRemoveToken() {
        if ((m_substring == null) || m_substring.equals("")) {
            throw new BuildException("attribute substring must not be null.");
        }
        
        StringBuffer sb = new StringBuffer();
        StringTokenizer st = new StringTokenizer(m_string);
        while (st.hasMoreTokens()) {
            String s = st.nextToken();
            if (!s.equals(m_substring)) {
                sb.append(s).append(" ");
            }
        }

        return sb.toString().trim();
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_string == null) || m_string.equals("")) {
            throw new BuildException("attribute string must not be null.");
        }

        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException("attribute property must not be null.");
        }

        if ((m_action == null) || m_action.equals("")) {
            throw new BuildException("attribute action must not be null.");
        }

        if (!m_actions.contains(m_action)) {
            throw new BuildException(m_action + " is not a supported action.");
        }
    }


}


