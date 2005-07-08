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
 * This class manages the ir3_xmi's command line options.
 */
public class IR3XMIOptionsManager
    extends org.objectweb.ccm.util.compiler.OptionsManager_impl {

    /**
     * The IR object name.
     */
    private java.lang.String objectName_;

    /**
     * The command name.
     */
    private java.lang.String commandName_;

    /**
     * Constructor.
     *
     * @param identation The indentation to use.
     * @param commandName The command name.
     */
    public IR3XMIOptionsManager(java.lang.String indentation,
                                java.lang.String commandName) {
    
        super(indentation);

        addOption(new org.objectweb.ccm.util.compiler.Option(
            "-h", 0, "-h                Display this help."));
        addOption(new org.objectweb.ccm.util.compiler.Option(
            "-o", 1, "-o <file>         Output filename."));
        addOption(new org.objectweb.ccm.util.compiler.Option(
            "-dtd", 1, "-dtd <file>     Location of the XMI DTD."));

        objectName_ = "";
        commandName_ = commandName;
    }

    /**
     * This method displays the usage.
     */
    public void usage() {
        System.err.println("usage :");
        System.err.println(commandName_ + " [options] <ir_object>\n");
        System.err.println("options :");
        optionsUsage("");
    }

    /**
     * This method analyzes the command line arguments.
     *
     * @param cmdline The command line arguments.
     */
    public int analyse(java.lang.String[] cmdline) {
        int length = cmdline.length;

        if (length < 1) {
            System.err.println("Missing <ir_object>");
            usage();
            return -1;
        }

        if (analyseOptions(cmdline, 1) == -1) {
            return -1;
        }

        if (isSet("-h") || cmdline[length - 1].equals("-h")) {
            usage();
            return -1;
        }

        if (cmdline[length - 1].startsWith("-")) {
            System.err.println("Invalid <ir_object> : " + cmdline[length - 1]);
            usage();
            return -1;
        }
        
        objectName_ = cmdline[length - 1];
        
        return 0;
    }

    /**
     * This method returns the IR object name.
     */
    public java.lang.String getObjectName() {
        return objectName_;
    }
}


