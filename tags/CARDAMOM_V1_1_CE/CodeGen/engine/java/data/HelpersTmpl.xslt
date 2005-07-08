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


<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">



<!--
   For use in <b>translate()</b> to lower/upper case a string.
-->
<xsl:variable name="lcase" select="'abcdefghijklmnopqrstuvwxyz-_.'"/>

<!--
   For use in <b>translate()</b> to lower/upper case a string.
-->
<xsl:variable name="ucase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ-_.'"/>



<!--
   This template returns the last token of a string.

   @param _string      The tokenized string.
   @param _separator   The token separator used in the string.
-->
<xsl:template name="getLastToken">
   <xsl:param name="_string"/>
   <xsl:param name="_separator" select="$xmlSep"/>

   <xsl:variable name="substringAfter" select="substring-after($_string, $_separator)"/>

   <xsl:choose>
      <!--
         Does the string contain any separator ?
         If true and the string does not end with a separator,
         then proceed with the remaining substring.
      -->
      <xsl:when test="contains($_string, $_separator) and string-length($substringAfter) > 0">
         <xsl:call-template name="getLastToken">
            <xsl:with-param name="_string" select="$substringAfter"/>
            <xsl:with-param name="_separator" select="$_separator"/>
         </xsl:call-template>
      </xsl:when>
      <!--
         Recursiveness end statement : string with no separators or string ending with a separator.
      -->
      <xsl:otherwise>
         <xsl:choose>
            <xsl:when test="contains($_string, $_separator) and string-length($substringAfter) = 0">
               <xsl:value-of select="substring-before($_string, $_separator)"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$_string"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template getLastToken -->



<!--
   This template replaces characters in a string.

   @param _stringIn   The input string.
   @param _charsIn    The characters to replace.
   @param _charsOut   The characters to use for substitutions.
-->
<xsl:template name="replaceCharsInString">
   <xsl:param name="_stringIn"/>
   <xsl:param name="_charsIn"/>
   <xsl:param name="_charsOut"/>

   <xsl:choose>
      <!--
         Find an occurence of _charsIn, replace it with _charsOut
         and proceed with the remaining substring.
      -->
      <xsl:when test="contains($_stringIn, $_charsIn)">
         <xsl:value-of select="concat(substring-before($_stringIn, $_charsIn), $_charsOut)"/>
         <xsl:call-template name="replaceCharsInString">
            <xsl:with-param name="_stringIn" select="substring-after($_stringIn, $_charsIn)"/>
            <xsl:with-param name="_charsIn" select="$_charsIn"/>
            <xsl:with-param name="_charsOut" select="$_charsOut"/>
         </xsl:call-template>
      </xsl:when>
      <!--
         Recursiveness end statement : no more characters to replace.
      -->
      <xsl:otherwise>
         <xsl:value-of select="$_stringIn"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template replaceCharsInString -->



<!--
   This template appends a token to a list.<br>
   The default separator is set to the 'space' character.

   @param _list        The list.
   @param _token       The token to add.
   @param _separator   The token separator used in the list.
-->
<xsl:template name="addTokenToList">
   <xsl:param name="_list"/>
   <xsl:param name="_token"/>
   <xsl:param name="_separator" select="' '"/>

   <xsl:choose>
      <!--
         Are there any tokens in the list ?
      -->
      <xsl:when test="string-length($_list) > 0">
         <xsl:choose>
            <!--
               Compare _token with the first token of the list.
            -->
            <xsl:when test="not(starts-with($_list, concat($_token, $_separator)))">
               <!--
                  They are not equals, keep the first token of the list
                  and proceed with the following token.
               -->
               <xsl:choose>
                  <xsl:when test="contains($_list, $_separator)">
                     <xsl:value-of select="concat(substring-before($_list, $_separator), $_separator)"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_list, $_separator)"/>
                  </xsl:otherwise>
               </xsl:choose>
               <xsl:call-template name="addTokenToList">
                  <xsl:with-param name="_list" select="substring-after($_list, $_separator)"/>
                  <xsl:with-param name="_token" select="$_token"/>
                  <xsl:with-param name="_separator" select="$_separator"/>
               </xsl:call-template>
            </xsl:when>
            <!--
               We found duplicates, do nothing.
               Return _list as we are done with the job.
            -->
            <xsl:otherwise>
               <xsl:value-of select="$_list"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         No more tokens in the list, return _token.
      -->
      <xsl:otherwise>
         <xsl:value-of select="$_token"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template addTokenToList -->



<!--
   This template merges two lists and removes duplicates.<br>
   The default separator is set to the 'space' character.

   @param _list1       The first list.
   @param _list2       The second list.
   @param _separator   The token separator used in the lists.
-->

<xsl:template name="mergeLists">
   <xsl:param name="_list1"/>
   <xsl:param name="_list2"/>
   <xsl:param name="_separator" select="' '"/>

   <xsl:choose>
      <xsl:when test="string-length($_list2) > 0">
         <!--
            Get the first token of _list2.
         -->
         <xsl:variable name="token">
            <xsl:choose>
               <xsl:when test="contains($_list2, $_separator)">
                  <xsl:value-of select="substring-before($_list2, $_separator)"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="$_list2"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>
         <!--
            If _list1 does not contain this token already then append it.
         -->
         <xsl:variable name="newList1">
            <xsl:call-template name="addTokenToList">
               <xsl:with-param name="_list" select="$_list1"/>
               <xsl:with-param name="_token" select="$token"/>
            </xsl:call-template>
         </xsl:variable>
         <!--
            Proceed with the tokens left in _list2.
         -->
         <xsl:call-template name="mergeLists">
            <xsl:with-param name="_list1" select="$newList1"/>
            <xsl:with-param name="_list2" select="substring-after($_list2, $_separator)"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="$_list1"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template mergeLists -->



<!--
   This template reverses a list.<br>
   The default separator is set to the 'space' character.

   @param _list        The list to reverse.
   @param _separator   The token separator used in the list.
-->

<xsl:template name="reverseList">
   <xsl:param name="_list"/>
   <xsl:param name="_separator" select="' '"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_result" select="''"/>

   <xsl:variable name="lastToken">
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="$_list"/>
         <xsl:with-param name="_separator" select="$_separator"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:variable name="remainingList">
      <xsl:value-of select="substring($_list, 0, string-length($_list) - string-length($lastToken))"/>
   </xsl:variable>

   <xsl:variable name="result">
      <xsl:choose>
         <xsl:when test="string-length($_result) > 0">
            <xsl:value-of select="concat($_result, $_separator, $lastToken)"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$lastToken"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>

   <xsl:choose>
      <xsl:when test="string-length($remainingList) > 0">
         <xsl:call-template name="reverseList">
            <xsl:with-param name="_list" select="$remainingList"/>
            <xsl:with-param name="_result" select="$result"/>
            <xsl:with-param name="_separator" select="$_separator"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="$result"/>
      </xsl:otherwise>
   </xsl:choose>

</xsl:template> <!-- end of template reverseList -->



<!--
   This template counts the number of time a substring occurs in a string.

   @param _string   The string for use in the searching.
   @param _chars    The substring or character to look for.
-->

<xsl:template name="count-substring">
   <xsl:param name="_string"/>
   <xsl:param name="_chars"/>

   <xsl:choose>
      <!--
         Nothing to count here, return 0.
      -->
      <xsl:when test="string-length($_string) = 0 or string-length($_chars) = 0">
         <xsl:text>0</xsl:text>
      </xsl:when>
      <!--
         Found an occurence, increase the counter
         and proceed with the remaining substring.
      -->
      <xsl:when test="contains($_string, $_chars)">
         <xsl:variable name="remaining">
            <xsl:call-template name="count-substring">
               <xsl:with-param name="_string" select="substring-after($_string, $_chars)"/>
               <xsl:with-param name="_chars" select="$_chars"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:value-of select="$remaining + 1"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:text>0</xsl:text>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template count-substring -->



<!--
   This template removes duplicate tokens within a string.

   @param _string      The string to work on.
   @param _separator   The token separator used in the string.
-->
<xsl:template name="removeDuplicateTokens">
   <xsl:param name="_string"/>
   <xsl:param name="_separator" select="' '"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_stringOut" select="''"/>

   <!--
      How it works : this template goes recursively through each token of the string
      and, by the same time, builds a new string that is returned to the user.
      For each token, if it is already contained in the _stringOut, then it won't be
      added a second time. If it is not, then add it to the _stringOut.
   -->
   <xsl:choose>
      <xsl:when test="contains($_string, $_separator)">
         <xsl:variable name="firstToken" select="substring-before($_string, $_separator)"/>
         <xsl:choose>
            <xsl:when test="not(contains($_stringOut, $firstToken))">
               <xsl:call-template name="removeDuplicateTokens">
                  <xsl:with-param name="_string" select="substring-after($_string, $_separator)"/>
                  <xsl:with-param name="_separator" select="$_separator"/>
                  <xsl:with-param name="_stringOut" select="concat($_stringOut, $_separator, $firstToken)"/>
               </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
               <xsl:call-template name="removeDuplicateTokens">
                  <xsl:with-param name="_string" select="substring-after($_string, $_separator)"/>
                  <xsl:with-param name="_separator" select="$_separator"/>
                  <xsl:with-param name="_stringOut" select="$_stringOut"/>
               </xsl:call-template>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="$_stringOut"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template removeDuplicateTokens -->



<!--
   This template counts the number of tokens in a string.

   @param _string      The string to work on.
   @param _separator   The token separator used in the string.
-->
<xsl:template name="countTokens">
   <xsl:param name="_string"/>
   <xsl:param name="_separator" select="' '"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_counter" select="0"/>

   <xsl:choose>
      <xsl:when test="contains($_string, $_separator)">
         <xsl:choose>
            <xsl:when test="starts-with($_string, $_separator)">
               <xsl:call-template name="countTokens">
                  <xsl:with-param name="_string" select="substring-after($_string, $_separator)"/>
                  <xsl:with-param name="_separator" select="$_separator"/>
                  <xsl:with-param name="_counter" select="$_counter"/>
               </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
               <xsl:call-template name="countTokens">
                  <xsl:with-param name="_string" select="substring-after($_string, $_separator)"/>
                  <xsl:with-param name="_separator" select="$_separator"/>
                  <xsl:with-param name="_counter" select="$_counter + 1"/>
               </xsl:call-template>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
         <xsl:choose>
            <xsl:when test="string-length($_string) > 0">
               <xsl:value-of select="$_counter + 1"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$_counter"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template countTokens -->



<!--
   This template adds the specified file extension to a filename.

   @param _filename    The filename.
   @param _extension   The file extension.
-->
<xsl:template name="addFileExtension">
   <xsl:param name="_filename"/>
   <xsl:param name="_extension"/>

   <xsl:choose>
      <xsl:when test="contains($_filename, $_extension)">
         <xsl:value-of select="$_filename"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="concat($_filename, $_extension)"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template>



</xsl:stylesheet>


