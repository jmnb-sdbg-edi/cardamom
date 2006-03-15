<?xml version="1.0" encoding="utf-8"?>
<!-- ===================================================================== -->
<!--
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
-->
<!-- ===================================================================== -->

<!DOCTYPE softpkg SYSTEM "softpkg.dtd">


<softpkg name="CsdTest" version="0,1,2,3">

    <pkgtype>CORBA Component</pkgtype>

    <description>Example of CSD for test of CsdReader</description>

    <descriptor type="CORBA Component">
        <fileinarchive name="foo.ccd"/>
    </descriptor>

    <implementation id="DCE:12345678-1234-1234-0003-123456789abc">
        <processor name="x86"/>
        <os name="WinNT" version="4,0,0,0"/>
        <os name="Win95"/>
        <compiler name="gcc" version="2,95,2,0"/>
        <programminglanguage name="C++"/>
        <runtime name="gdb" version="5,0,0,0"/>

        <code type="DLL">
            <fileinarchive name="libtest.so"/>
            <entrypoint>createTestHome</entrypoint>
        </code>
    </implementation>
       
</softpkg>

