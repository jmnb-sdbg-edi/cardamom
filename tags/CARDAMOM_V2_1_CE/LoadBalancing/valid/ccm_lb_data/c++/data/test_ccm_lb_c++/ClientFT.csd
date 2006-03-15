<?xml version="1.0" encoding="utf-8"?>
<!-- ===================================================================== -->
<!--
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

<softpkg name="Client" version="1,0,0,0">
    <pkgtype>CORBA Component</pkgtype>

    <idl id="IDL:acme.com/CcmLbTest/Client:1.0" >
        <fileinarchive name="ccm_lb.idl" />
    </idl>

    <implementation id="DCE:700dc518-0110-11ce-ac8f-0800090531ac">
        <os name="Linux" version="2.4.21-4.ELsmp" />
        <processor name="i686" />
        <compiler name="gcc" version="3.2.3" />
        <programminglanguage name="C++" />

        <descriptor type="CORBA Component">
            <fileinarchive name="ClientHome.ccd" />
        </descriptor>

        <code type="DLL">
            <fileinarchive name="libcdmwQualifCcmLbClientFT.so" />
            <entrypoint>create_CCM_ClientHome_FT</entrypoint>
        </code>
    </implementation>
</softpkg>
