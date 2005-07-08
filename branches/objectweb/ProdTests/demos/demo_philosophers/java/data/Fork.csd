<?xml version="1.0" encoding="utf-8"?>
<!--
 * =========================================================================== *
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
 * =========================================================================== *
-->

<!DOCTYPE softpkg SYSTEM "softpkg.dtd">

<softpkg name="ForkManager" version="1,0,0,0">
    <pkgtype>CORBA Component</pkgtype>

    <idl id="IDL:acme.com/Dinner/ForkManager:1.0" >
        <fileinarchive name="Philo.idl"/>
    </idl>
        
    <implementation id="DCE:700dc518-0110-11ce-ac8f-090009053001">
        <os name="Solaris" version="2,8,0,0" />
        <processor name="sparc" />
        <compiler name="javac" version="1,3,1,3" />
        <programminglanguage name="Java" />
                
        <descriptor type="CORBA Component">
            <fileinarchive name="ForkHome.ccd" />
        </descriptor>
        
        <code type="Java Class">
            <fileinarchive name="libForkManager.jar"/>
            <entrypoint>cdmw.dinnerdemo.CCMForkHomeImpl.createCCMForkHome</entrypoint>
        </code>
        
    </implementation>

</softpkg>

