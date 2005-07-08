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


package cdmw.ossupport;

/**
 * JarClassLoader provides a minimalistic ClassLoader which shows how to
 * instantiate a class which resides in a .jar file.
 * 
 */
public class JarClassLoader extends MultiClassLoader {

    /**
     * Internal storage of jar file content
     */
    private JarResources jarResources;

    /**
     * Creates a loader for a specified jar file
     * 
     * @param jarName The name of the jar file.
     */
    public JarClassLoader(String jarName) {
        // Create the JarResource and suck in the .jar file.
        jarResources = new JarResources(jarName);
    }

    /**
     * Returns the bytes that make up the class data.
     * 
     * @param className the name of the class
     * @return the bytes that make up the class data
     */
    protected byte[] loadClassBytes(String className) {
        // Support the MultiClassLoader's class name munging facility.
        className = formatClassName(className);

        // Attempt to get the class data from the JarResource.
        return (jarResources.getResource(className));
    }

}

