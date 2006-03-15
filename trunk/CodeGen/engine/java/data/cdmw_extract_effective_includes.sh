#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

export CPP="cpp -w"

cpp_err=`mktemp /tmp/cpp_errrors.XXXXXX`

$CPP $@ 2>$cpp_err | awk '
    # BEGIN AWK
    BEGIN {
        counter="0"
        curent_include=""
        direct_include_file=""
    }
    
    /#[[:space:]][[:digit:]]+[[:space:]]"[[:alnum:][:punct:]]+"[[:space:]](1|2)/ {
        line=2
        filename=3
        level=4

        # get include file currently considered        
        nfields = split($filename, fieldarray, "/")
        #curent_include=""$filename
        curent_include=""fieldarray[nfields]
        gsub("\"", "", curent_include)

        # If level is 1, this is a push in "include stack"
        if ($level == "1") {
            # incrase stack counter
            counter=counter+"1"

            # if counter is 1, this is a first pop
            if (counter == 1) {
                # Thus, this is a directely included file
                # This file has to be included again (as it or with _cif extension)
                direct_include_file=curent_include

                # presume it is not IDL3 by default
                is_direct_include_file_idl3="NO"
            }
        }
        
        # If level is 1, this is a pop in "include stack"      
        if ($level == "2") {
            # decrease stack counter
            counter=counter-"1"

            # if counter is 0, direct included file treatment ended
            # We know if it is an IDL3 or not.            
            if (counter == 0) {
                if (is_direct_include_file_idl3 == "YES") {
                    # add "_cif" to filename to be include
                    to_be_include=gensub("\.idl$", "\_cif.idl", "g", direct_include_file) 
                } else {
                    # keep include file as it is
                    to_be_include=direct_include_file
                }
                
                # print include directive
                print "#include \""to_be_include"\""
            }
        }
    }
    
    ! /#(.*)/ && /([[:space:]]|^)home([[:space:]]|$)/ {
        # Home declaration found in current included file
        # Thus, current directly included file is also IDL3 (by side effect)
        is_direct_include_file_idl3="YES"
    }

    ! /#(.*)/ && /([[:space:]]|^)component([[:space:]]|$)/ {
        # Component declaration found in current included file
        # Thus, current directly included file is also IDL3 (by side effect)
        is_direct_include_file_idl3="YES"
    }

    ! /#(.*)/ && /([[:space:]]|^)eventtype([[:space:]]|$)/ {
        # Eventtype declaration found in current included file
        # Thus, current directly included file is also IDL3 (by side effect)
        is_direct_include_file_idl3="YES"
    }
    
'

if [ -s $cpp_err ]; then
    cat $cpp_err | awk '{ print "#error during preprocessing of IDL3 file "$0 }'
fi

rm -f $cpp_err

