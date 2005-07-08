#!/bin/sh
#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI.
#* 
#* It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
#* All rights reserved.
#* 
#* CARDAMOM is free software; you can redistribute it and/or modify it under
#* the terms of the GNU Library General Public License as published by the
#* Free Software Foundation; either version 2 of the License, or (at your
#* option) any later version.
#* 
#* CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
#* License for more details.
#* 
#* You should have received a copy of the GNU Library General
#* Public License along with CARDAMOM; see the file COPYING. If not, write to
#* the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#* =========================================================================== *


# Qualification test name
TEST=Q-009

# List of files including ORBACUS API calls
RESULTFILE=$TEST.OS.txt

# CARDAMOM ROOT Directory
ROOT="../../../../.."

# File including all keywords to look for
KEYWORDSFILE=./OS-Keywords.txt


awkcode='BEGIN { FileName=""; Revision=""; State=""; }
{
  if (State=="") {
    # remove string definition
    gsub("\".*\"","\"\"")

    # remove comment starting by // 
    if (index($0,"\/\/")) {
        gsub("\/\/.*$","")
    } 

    if (index($0,"\/\*")) {
      # a "start comment" has been found
      State="StartCommentFound"
    } else {
        print 
    }
  }

  if (State=="StartCommentFound") {
    if (index($0,"\*\/")) {
      # a "end of comment" has been found
      # on the same line
      State="SimpleCommentFound"
    } else {
      State="MultiLineCommentFound"
      gsub("\/\\*.*$","")
      print
    }
  }

  if (State=="SimpleCommentFound") {
    gsub("\/\\*.*\\*\/","")
    print 
    State="";
  }

  if (State=="MultiLineCommentFound") {
    if (index($0,"\*\/")) {
      # a "end of comment" has been found
      gsub("^.*\\*\/","")
      print
      State="";
    } 
  }
}
END { } '

echo " "
echo "Processing data for test $TEST (OS)"
echo " "

echo "List of files including references to OS API" > $RESULTFILE
echo "============================================" >> $RESULTFILE
echo " "  >> $RESULTFILE

for i in `find $ROOT -name prod -prune -o -name delivery -prune -o -name generated -prune -o -name test -prune -o -name valid -prune -o -name ProdTests -prune -o -name ossupport -prune -o -name osthreads -prune -o -name Tutorial -prune -o -name \*.cpp -print -o -name \*.hpp -print`; do 
     `cat $i | awk "$awkcode" | grep -w -f $KEYWORDSFILE > /tmp/$RESULTFILE`;

#        exclude some found inappropriate keywords
         result=`cat /tmp/$RESULTFILE | \
                 grep -v "\.abort"  |\
                 grep -v "\->abort" |\
                 grep -v "\.bind"  |\
                 grep -v "\->bind" |\
                 grep -v "\.close"  |\
                 grep -v "\->close" |\
                 grep -v "\.open"  |\
                 grep -v "\->open" |\
                 grep -v "\.select"  |\
                 grep -v "\->select" |\
                 grep -v "\.shutdown"  |\
                 grep -v "\->shutdown" |\
                 grep -v "\.signal"  |\
                 grep -v "\->signal" |\
                 grep -v "OS::sleep" |\
                 grep -v "clock::" |\
                 grep -v "system\." |\
                 grep -v "system\->" |\
                 grep -v "time\." |\
                 grep -v "time\->" |\
                 grep -v "\.wait"  |\
                 grep -v "\->wait"`

         if [ -n "$result" ]; then
	    file_str=`echo $i | sed -e 's/\.\.\///g'`
   	    echo "==========================" >> $RESULTFILE;
   	    echo "file $file_str :" >> $RESULTFILE;
   	    echo "--------------------------" >> $RESULTFILE;
   	    echo "$result" >> $RESULTFILE;
  	    echo "==========================" >> $RESULTFILE;
   	    echo " " >> $RESULTFILE;
   	fi
done

rm -rf /tmp/$RESULTFILE

echo "Processing data for test $TEST (OS) completed";
