/* =========================================================================== *
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
 * =========================================================================== */


package cdmw.tutorial;


// Note: ColorPrinterImpl must inherit from cdmw.lifecycle.LifeCycleSingleObject.
//       Therefore, a TIE pattern have to be used for HelloInterface implementation.

class ColorPrinterImpl extends cdmw.lifecycle.LifeCycleSingleObject
                       implements com.thalesgroup.PrinterModule.ColorPrinterOperations
{

    private com.thalesgroup.PrinterModule.ColorPrinterPackage.ColorMode mode;
    
    
    public ColorPrinterImpl(cdmw.lifecycle.ObjectDeactivator objDeact) {
        super(objDeact);
        System.out.println("<# ColorPrinter servant constructed #>");
        mode = com.thalesgroup.PrinterModule.ColorPrinterPackage.ColorMode.BlackAndWhite;
    }
    
    
    //
    // IDL:thalesgroup.com/PrinterModule/Printer/print_message:1.0
    //
    public void print_message(String message) {
        if (mode.equals(com.thalesgroup.PrinterModule.ColorPrinterPackage.ColorMode.BlackAndWhite)) {
            System.out.println("---> print in black and white");
        } else {
            System.out.println("---> print in color");
        }            
        
        System.out.println(message);
    }

    
    //
    // IDL:thalesgroup.com/PrinterModule/ColorPrinter/set_color:1.0
    //
    public void set_color(com.thalesgroup.PrinterModule.ColorPrinterPackage.ColorMode mode) {
        this.mode = mode;
    }
    
}

