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


// Note: TwoRootsBinomeImpl must inherit from cdmw.lifecycle.LifeCycleSingleObject.
//       Therefore, a TIE pattern have to be used for PolynomeModule.Polynome implementation.

class TwoRootsBinomeImpl
    extends cdmw.lifecycle.LifeCycleSingleObject
    implements com.thalesgroup.PolynomeModule.PolynomeOperations 
{

    private double a;
    private double b;
    private double delta;
    private double x1;
    private double x2;
    
    
    
    
    public TwoRootsBinomeImpl(cdmw.lifecycle.ObjectDeactivator objDeact,
                              double a, double b, double delta) 
    {
        super(objDeact);
        this.a = a;
        this.b = b;
        this.delta = delta;
    }
    
    
    //
    // Compute roots of equations
    //
    public void computeRoots() {
        double square_root = Math.sqrt(delta);
            
        x1 = (-b + square_root) / (2*a);
            
        x2 = (-b - square_root) / (2*a);
            
        // if x1 is greater than x2, swap the values
        if (x1 > x2)
        {
            double x1_memo = x1;
            x1 = x2;
            x2 = x1_memo;
        }
    }
    
    //
    // Print the roots
    //
    public void printRoots() {
        System.out.println("x1 = " + x1);
        System.out.println("x2 = " + x2);
        System.out.println();
    }
    
    
    //
    // Check if value is in roots interval
    //
    public boolean isInBracket(double x) {
        return (x >= x1 && x <= x2);
    }
       
}
