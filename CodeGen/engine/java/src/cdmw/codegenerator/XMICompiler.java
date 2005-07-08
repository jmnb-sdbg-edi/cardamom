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


package cdmw.codegenerator;

/**
 * This class manages the OpenCCM XMI Compiler.
 */
public class XMICompiler {
    /**
     * The options manager.
     */
    private IR3XMIOptionsManager manager_;

    /**
     * Version.
     */
    private java.lang.String version_;

    /**
     * Constructor.
     *
     * @param manager The options manager.
     * @param version The version.
     */
    public XMICompiler(IR3XMIOptionsManager manager,
                       java.lang.String version) {

        manager_ = manager;
        version_ = version;
    }

    /**
     * This method prints a message to the standard output.
     */
    protected void message(String text) {
        System.out.println(version_ + " : " + text);
    }

    public boolean run(java.lang.String[] cmdline)
    {
        if (manager_.analyse(cmdline) == -1) {
            return false;
        }

        return true;
    }

    /**
     * This method returns the options manager.
     */
    public IR3XMIOptionsManager getManager() {
        return manager_;
    }
}


