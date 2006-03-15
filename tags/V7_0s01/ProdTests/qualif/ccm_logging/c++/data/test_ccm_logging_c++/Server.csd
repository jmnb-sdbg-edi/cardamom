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


<softpkg name="Server" version="1,0,1,0">
    <pkgtype>CORBA Component</pkgtype>

    <idl id="IDL:acme.com/Hello/Server:1.0" >
        <fileinarchive name="Hello.idl" />
    </idl>

    <implementation id="DCE:700dc518-0110-11ce-ac8f-0800090b5d3e">
        <os name="Linux RedHat" version="7,3,0,0" />
        <processor name="x86" />
        <compiler name="gcc" version="3,2,0,0" />
        <programminglanguage name="C++" />

        <descriptor type="CORBA Component">
            <fileinarchive name="ServerHome.ccd" />
        </descriptor>

        <code type="DLL">
            <fileinarchive name="libcdmwCcmLoggingServer.so" />
            <entrypoint>create_ServerHomeExecutor</entrypoint>
        </code>
    </implementation>
</softpkg>
