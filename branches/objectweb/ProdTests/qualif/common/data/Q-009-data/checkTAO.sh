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

# ORB Name
ORBNAME=TAO

# List of files including ORBACUS API calls
RESULTFILE=$TEST.$ORBNAME.txt

# CARDAMOM ROOT Directory
ROOT="../../../../.."

echo " "
echo "Processing data for test $TEST ($ORBNAME)"
echo " "

echo "List of files including references to $ORBNAME API" > $RESULTFILE
echo "=================================================" >> $RESULTFILE
echo " "  >> $RESULTFILE

for i in `find $ROOT -name prod -prune -o -name delivery -prune -o -name generated -prune -o -name test -prune -o -name valid -prune -o -name ProdTests -prune -o -name orbsupport -prune -o -name Tutorial -prune -o -name \*.cpp -o -name \*.hpp -print`; do 
	result1=`grep ACE $i | \
                grep -v FACET | \
                grep -v TRACE | \
                grep -v ADJACENT | \
                grep -v INTERFACE`;

	result2=`grep -i TAO $i | \
                 grep -v "// for TAO test" | \
                 grep -v "xxxxxxxx"`;

	if [ -n "$result1" ]; then
		file_str=`echo $i | sed -e 's/\.\.\///g'`
		echo "==========================" >> $RESULTFILE;
		echo "file $file_str :" >> $RESULTFILE;
		echo "--------------------------" >> $RESULTFILE;
		echo "$result1" >> $RESULTFILE;
		echo "==========================" >> $RESULTFILE;
		echo " " >> $RESULTFILE;
	fi

	if [ -n "$result2" ]; then
		file_str=`echo $i | sed -e 's/\.\.\///g'`
		echo "==========================" >> $RESULTFILE;
		echo "file $file_str :" >> $RESULTFILE;
		echo "--------------------------" >> $RESULTFILE;
		echo "$result2" >> $RESULTFILE;
		echo "==========================" >> $RESULTFILE;
		echo " " >> $RESULTFILE;
	fi
done

echo "Processing data for test $TEST ($ORBNAME) completed";
