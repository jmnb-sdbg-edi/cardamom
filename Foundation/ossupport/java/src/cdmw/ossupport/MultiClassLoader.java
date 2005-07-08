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

import java.util.Hashtable;

/**
 * A simple test class loader capable of loading from
 * multiple sources, such as local files or a URL.
 *
 * This class is derived from an article by Jack Harich and 
 * John D. Mitchell on JavaWorld with modifications.
 *
 */
public abstract class MultiClassLoader extends ClassLoader {

    /**
     * Internal cache of classes
     */
    private Hashtable classes = new Hashtable();
    
    /**
     * Replacement char used when classname is formatted
     */
    private char classNameReplacementChar;
    
    /**
     * Loads the class with the specified name.
     * 
     * @param className the name of the class
     * @return the resulting class object
     * @exception ClassNotFoundException if the class was not found
     */
    public Class loadClass(String className) throws ClassNotFoundException {
        return (loadClass(className, true));
    }

    /**
     * Loads the class with the specified name.
     * 
     * @param className the name of the class
     * @param resolveIt if true then resolve the class
     * @return the resulting class object
     * @exception ClassNotFoundException if the class was not found
     */
    public synchronized Class loadClass(String className, boolean resolveIt)
        throws ClassNotFoundException {

        Class result;
        byte[] classBytes;

        // Check our local cache of classes
        result = (Class) classes.get(className);
        if (result != null) {
            return result;
        }

        // Check with the primordial class loader
        try {
            result = super.findSystemClass(className);
            return result;
        } catch (ClassNotFoundException e) {}

        // Try to load it from preferred source
        // Note loadClassBytes() is an abstract method
        classBytes = loadClassBytes(className);
        if (classBytes == null) {
            throw new ClassNotFoundException();
        }

        // Define it (parse the class file)
        result = defineClass(className, classBytes, 0, classBytes.length);
        if (result == null) {
            throw new ClassFormatError();
        }

        // Resolve if necessary
        if (resolveIt)
            resolveClass(result);

        // Done
        classes.put(className, result);
        return result;
    }


    /**
     * This optional call allows a class name such as
     * "COM.test.Hello" to be changed to "COM_test_Hello",
     * which is useful for storing classes from different
     * packages in the same retrieval directory.
     */
    public void setClassNameReplacementChar(char replacement) {
        classNameReplacementChar = replacement;
    }
    
    /**
     * Returns the bytes that make up the class data.
     * 
     * @param className the name of the class
     * @return the bytes that make up the class data
     */
    protected abstract byte[] loadClassBytes(String className);

    /**
     * Format a class name by replacing the '.' char with the '/' char 
     * by default or the one specified with the method 
     * setClassNameReplacementChar and by adding '.class' at the end.
     * 
     * @param The name of the class
     * @return The formatted class name
     */
    protected String formatClassName(String className) {
        if (classNameReplacementChar == '\u0000') {
            // '/' is used to map the package to the path
            return className.replace('.', '/') + ".class";
        } else {
            // Replace '.' with custom char, such as '_'
            return className.replace('.', classNameReplacementChar) + ".class";
        }
    }

}

