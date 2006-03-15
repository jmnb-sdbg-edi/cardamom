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


package cdmw.ccm.deployment;

import com.thalesgroup.CdmwDeployment.RequirementDescription;


public class RequirementList {

    private java.util.Set list;


    public RequirementList() {
        list = new java.util.HashSet();
    }

    public int size() {
        return list.size();
    }


    public synchronized boolean add(Requirement req) {
        return list.add(req);
    }

    public synchronized boolean addAll(RequirementList reqList) {
        return list.addAll(reqList.list);
    }


    public boolean contains(Requirement req) {
        return list.contains(req);
    }


    public synchronized RequirementDescription[] getDescriptions() {
        RequirementDescription[] reqList =
            new RequirementDescription[size()];

        java.util.Iterator it = list.iterator();
        int i = 0;
        while (it.hasNext()) {
            Requirement req = (Requirement) it.next();
            reqList[i] = req.getDescription();
            i++;
        }

        return reqList;
    }


    public synchronized RequirementList
        getIntersectionWith(RequirementList reqList)
    {
        RequirementList result = new RequirementList();

        java.util.Iterator it = list.iterator();
        while (it.hasNext()) {
            Requirement req = (Requirement) it.next();
            if (reqList.contains(req)) {
                result.add(req);
            }
        }
        return result;
    }



}




