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


// Note: SingleRootBinomeImpl must inherit from cdmw.lifecycle.LifeCycleSingleObject.
//       Therefore, a TIE pattern have to be used for PolynomeModule.Polynome implementation.

class SingleRootBinomeImpl
    extends cdmw.lifecycle.LifeCycleSingleObject
    implements com.thalesgroup.PolynomeModule.PolynomeOperations 
{

    private double a;
    private double b;
    private double x1;
    
    
    
    
    public SingleRootBinomeImpl(cdmw.lifecycle.ObjectDeactivator objDeact,
                                double a, double b) 
    {
        super(objDeact);
        this.a = a;
        this.b = b;
    }
    
    
    //
    // Compute roots of equations
    //
    public void computeRoots() {
        x1 = -b / (2*a);
    }
    
    //
    // Print the roots
    //
    public void printRoots() {
        System.out.println("x1 = " + x1);
        System.out.println();
    }
    
    
    //
    // Check if value is in roots interval
    //
    public boolean isInBracket(double x) {
        return x == x1;
    }
       
}