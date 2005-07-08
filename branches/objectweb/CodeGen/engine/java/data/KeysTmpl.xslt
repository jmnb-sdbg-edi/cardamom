<?xml version="1.0" encoding="utf-8"?>
<!-- ===================================================================== -->
<!--
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
-->
<!-- ===================================================================== -->


<xsl:stylesheet version="1.1"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns:UML="org.omg/UML1.3">



<!--
   This key finds a stereotype by its xmi.id.
-->
<xsl:key name="stereotypeById"
   match="//UML:Stereotype"
   use="@xmi.id"/>

<!--
   This key finds a stereotype by its name.
-->
<xsl:key name="stereotypeByName"
   match="//UML:Stereotype"
   use="@name"/>

<!--
   This key finds a class by its xmi.id.
-->
<xsl:key name="classById"
   match="//UML:Class"
   use="@xmi.id"/>

<!--
   This key finds a class by its name.
-->
<xsl:key name="classByName"
   match="//UML:Class"
   use="@name"/>

<!--
   This key finds an exception by its xmi.id.
-->
<xsl:key name="exceptionById"
   match="//UML:Exception"
   use="@xmi.id"/>

<!--
   This key finds an exception by its name.
-->
<xsl:key name="exceptionByName"
   match="//UML:Exception"
   use="@name"/>

<!--
   This key finds all the classes of a package.
-->
<xsl:key name="classByModuleId"
   match="//UML:Class"
   use="@namespace"/>

<!--
   This key finds all the classes of a given stereotype.
-->
<xsl:key name="classByStereotypeId"
   match="//UML:Class"
   use="@stereotype"/>

<!--
   This key finds all the operations of a class.
-->
<xsl:key name="operations"
   match="//UML:Operation"
   use="@owner"/>

<!--
   This key finds a package by its name.
-->
<xsl:key name="moduleByName"
   match="//UML:Package"
   use="@name"/>

<!--
   This key finds a package by its xmi.id.
-->
<xsl:key name="moduleById"
   match="//UML:Package"
   use="@xmi.id"/>

<!--
   This key finds a generalization by its xmi.id.
-->
<xsl:key name="generalizationById"
   match="//UML:Generalization"
   use="@xmi.id"/>

<!--
   This key finds all the generalizations
   whose child xmi.id is passed as parameter.
-->
<xsl:key name="generalizationByChildId"
   match="//UML:Generalization"
   use="UML:Generalization.subtype/UML:GeneralizableElement/@xmi.idref"/>

<!--
   This key finds all the generalizations of a package.
-->
<xsl:key name="generalizationByModuleId"
   match="//UML:Generalization"
   use="@namespace"/>

<!--
   This key finds one or more associations by their stereotype id.
-->
<xsl:key name="associationByStereotypeId"
   match="//UML:Association"
   use="@stereotype"/>

<!--
   This key finds one or more associations by the source association end xmi.id.
-->
<xsl:key name="associationBySourceId"
   match="//UML:Association"
   use="UML:Association.connection/UML:AssociationEnd[1]/@type"/>



</xsl:stylesheet>


