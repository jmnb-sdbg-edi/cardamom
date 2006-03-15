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

##
# Possible return values.
#
ok=0
failed=1
fatal=2

##
# Initialize variables.
#
script_name=`basename $0`
xml=
obj=
xmi=
idl=
tmp=
noccm=
searchdir=
idl_to_include=
propfiles=
incr_interface=0

 
##
# Function : display help.
#
display_help() {
   echo
   echo "NAME"
   echo "     $script_name"
   echo
   echo "SYNOPSIS"
   echo "     $script_name [OPTION]..."
   echo
   echo "DESCRIPTION"
   echo "     The  $script_name  script reads an XML file used for the"
   echo "     CDMW code generation,  determines the IDLs to feed OpenCCM"
   echo "     with  and  exports  objects from  the  Interface Repository"
   echo "     into either XMI and/or IDL2 format."
   echo
   echo "OPTION"
   echo "     --help                    display this screen"
   echo "     --xml=<file> (*)          file name of the XML document to read"
   echo "     --obj=<name> (*)          name of the object to export from the IR"
   echo "     --xmi=<file>              file name to use when exporting XMI"
   echo "     --idl=<file>              file name to use when exporting IDL"
   echo "     --tmp=<dir>  (*)          temporary directory to use"
   echo "     --I=<dir>                 directory to visit when looking for IDLs"
   echo "     --preproc-flags=\"flags\" preprocessing flags to be used for IDL preprocessing"
   echo
   echo "(*) = those options are mandatory"
   echo
}

##
# Function : print an error message.
#
print_error() {
   if [ $# -eq 2 ]; then
      if [ `echo $2 | grep -c missing` -eq 0 ]; then
         echo "$script_name: error: $1: $2"
      else
         echo "$script_name: error: $1: $2; use --help to show usage"
      fi
   else
      if [ `echo $1 | grep -c missing` -eq 0 ]; then
         echo "$script_name: error: $1"
      else
         echo "$script_name: error: $1; use --help to show usage"
      fi
   fi
   exit $failed
}

##
# Function : check the mandatory options.
#
check_mandatory_options() {
   if [ ! $# -eq 3 ]; then
      print_error "missing mandatory options"
   fi
}

##
# Function : check the XML file name.
#
check_xml_filename() {
   if [ -n "$xml" ]; then
      if [ ! -f "$xml" ]; then
         print_error "--xml" "file $xml does not exist"
      fi
   else
      print_error "--xml" "missing argument"
   fi
}

##
# Function : check the temporary directory.
#
check_tmp_dir() {
   if [ -n "$tmp" ]; then
      if [ ! -d "$tmp" ]; then
         print_error "--tmp" "directory $tmp does not exist"
      fi
   else
      print_error "--tmp" "missing argument"
   fi
}

##
# Function : check if we can write output files onto disk.
#
check_output_dir() {
   while [ -n "$1" ]; do
      outfile=`basename $1`
      outdir=`dirname $1`
      if [ ! -d "$outdir" ]; then
         print_error "cannot write file $outfile into directory $outdir"
      fi
      shift
   done
}

##
# Function : check if the search directories are valid.
#
check_search_dirs() {
   for d in $searchdir; do
      idl_searchdir=`echo $d | sed 's/[-]*I//'`
      if [ ! -d "$idl_searchdir" ]; then
         print_error "--I" "directory $idl_searchdir does not exist"
      fi
   done
}

##
# Function : read the XML file and extract the IDL files to feed OpenCCM with.
#
find_idl() {
   idl_files_from_xml=`java -jar $saxon_jar_path/saxon.jar $xml \
      $share_dir/ExportHelpers.xslt TEMPLATE=get_idl_filenames`

   # Remove IDL files that cannot be found
   fqn_idl_to_include=
   for idlfile in $idl_files_from_xml; do
      filefound="false"
      for idl_dir in $searchdir; do
         idl_searchdir=`echo $idl_dir | sed 's/[-]*I//'`
         if [ -f "$idl_searchdir/$idlfile" ]; then
            idl_to_include="$idl_to_include $idlfile"
            if [ -z "`echo $fqn_idl_to_include | grep $idl_searchdir/$idlfile`" ]; then
                fqn_idl_to_include="$fqn_idl_to_include $idl_searchdir/$idlfile"
            fi
            filefound="true"
         fi
      done
      # Maybe this check could exit $failed (and so, could replace the ["$idl_to_include"=""])
      # because this is an important error the developper must take into account
      if [ "$filefound" = "false" ]; then
        echo "`date +'%y/%m/%d %H:%M:%S'` ERR CDGN : The IDL $idlfile from the XML document cannot be found in any idl-search-dir < `echo $searchdir | sed s/[-]*I//g` >"
        echo "So this file won't be in the list of IDL files to feed OpenCCM with"
        echo "Possible case: you have made a reference to generated idl in your xml"
      fi
   done
   if [ "$idl_to_include" = "" ]; then
      echo "`date +'%y/%m/%d %H:%M:%S'` FTL CDGN : The IDL < $idl_files_from_xml > from the XML document cannot be found in any idl-search-dir < `echo $searchdir | sed s/[-]*I//g` >"
      exit $failed;
   fi
}

##
# Function : create a dummy idl file
#
create_dummy_idl() {
   dummy_idl="$tmp/dummy.idl"

   if [ "xx$noccm" = "xx" ]; then
      echo \#include \<Components.idl\> >>$dummy_idl
   fi

   echo \#include \<CdmwLifeCycle.idl\> >>$dummy_idl
   
   for idlfile in $idl_to_include; do
      echo \#include \"${idlfile}\" >>$dummy_idl
   done
   echo "module Cdmw_dummy {" >>$dummy_idl
   echo "interface dummy {" >>$dummy_idl

   module_to_export=`echo $obj | sed 's%,% %g'  | sed 's%CORBA\.%%g' | sed 's%\.%::%g'`
   for obj_index in $module_to_export; do
      echo "$obj_index cdmw_dummy_interface$incr_interface();" >>$dummy_idl
      incr_interface=`expr $incr_interface + 1`
   done

   echo "}; // dummy interface" >>$dummy_idl
   echo "}; // Cdmw_dummy module" >>$dummy_idl

}

##
# Function : set OpenCCM environment
#
set_openccm_env() {
   if [ -z "${OpenCCM_CONFIG_DIR}" ]; then
      . $openccm_bindir/envi.OpenCCM.sh;
   fi
}

##
# Function : stop OpenCCM
#
stop_openccm() {
   if [ -n "${OpenCCM_CONFIG_DIR}" ]; then
      if [ -z "`ir3_started`" ]; then
         ir3_stop
      fi
   fi
}

##
# Function : check return status
#
check_return_status() {
   if [ "$1" = "$failed" -o "$1" = "$fatal" ]; then
      stop_openccm
      exit $1
   fi
}

##
# Function : export IR to XMI and/or IDL2
#
export_ir() {
   if [ -n "$xmi" -o -n "$idl" ]; then
      if [ -n "$obj" ]; then
         # If OpenCCM is running, stop it first
         stop_openccm
         
         # Start OpenCCM
         ir3_start

         # Feed IDLs
         feed_defines="-D__OpenCCM__"
         ir3_feed $feed_defines $preproc_flags $searchdir $dummy_idl
         check_return_status $?

         if [ -n "$xmi" ]; then
            echo "`date +'%y/%m/%d %H:%M:%S'` INF CDGN : Exporting to XMI..."
            ir3_xmi -dtd $xmi_dtd -o $xmi ::Cdmw_dummy
            check_return_status $?
         fi

         if [ -n "$idl" ]; then
            for f in `find $share_dir -name \*mapping.properties`; do
                propfiles="$propfiles -f $f"
            done
            echo "`date +'%y/%m/%d %H:%M:%S'` INF CDGN : Exporting to IDL2..."
            # $obj has following format:  module.component1,module.component2,...
            # Get name of the first module from $obj 
            module=`echo $obj | awk 'BEGIN {FS="."} {print $1}'`
            # Check that all elements of $obj are in the same module
            echo $obj | awk 'BEGIN {FS=","} {
                split($1, a, ".");
                for (i=1; i<=NF; i++) {
                    if ((length($i) != 0) && (index($i, a[1]) != 1)) {
                        print "ERROR: all elements to export ("$0") should be in module "a[1];
                        exit 1;
                    }
                }
            }'
            check_return_status $?
            ir3_cdmwidl2 $propfiles -o $idl ::$module
            check_return_status $?

            eei_output=`mktemp /tmp/eei_output.XXXXXX`
            for idlfile in $fqn_idl_to_include; do
                cdmw_extract_effective_includes.sh $feed_defines $preproc_flags $searchdir $idlfile >> $eei_output
                echo "" >> $eei_output
            done

            # insert the effective includes into the output idl2
            pos=`grep -n __INCLUDES__ $idl | cut -d: -f1`
            head -n`expr $pos - 1` $idl > $idl.tmp
            cat $eei_output >> $idl.tmp
            lines=`wc -l $idl | awk '{ print $1 }'`
            tail -n`expr $lines - $pos` $idl >> $idl.tmp
            cp -f $idl.tmp $idl

            # insert the effective includes into the output Cdmw idl2
            cdmw_idl=`dirname $idl`/Cdmw_`basename $idl`
            pos=`grep -n __INCLUDES__ $cdmw_idl | cut -d: -f1`
            head -n`expr $pos - 1` $cdmw_idl > $cdmw_idl.tmp
            cat $eei_output | awk '
                /#include "(.*)_cif.idl"/ { 
                    print gensub("#include \"(.*)\"", "#include \"Cdmw_\\1\"", "g", $0) 
                }' >> $cdmw_idl.tmp
            lines=`wc -l $cdmw_idl | awk '{ print $1 }'`
            tail -n`expr $lines - $pos` $cdmw_idl >> $cdmw_idl.tmp
            cp -f $cdmw_idl.tmp $cdmw_idl

            rm -f $eei_output
         fi

         # Stop OpenCCM
         stop_openccm
      fi
   fi
}

###############################################################################
# Main section
#

trap 'stop_openccm; exit 2' 1 2 15

# Variables
share_dir=@config.install.dir@/share
saxon_jar_path=@config.Saxon.lib.dir@
openccm_bindir=@config.install.dir@/bin
xmi_dtd=$share_dir/XMI.dtd

#
# Analyze the command line arguments.
#

# If no arguments, then display help
if [ $# -eq 0 ]; then
  display_help
  exit $ok
fi

while [ -n "$1" ]; do
  case "$1" in
     --help | -help)
        display_help
        exit $ok
        ;;
     --xml | --xml= | -xml | -xml=)
        print_error "--xml" "missing argument"
        ;;
     --xml=* | -xml=*)
        xml=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        ;;
     --obj | --obj= | -obj | -obj=)
        print_error "--obj" "missing argument"
        ;;
     --obj=* | -obj=*)
        obj=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        ;;
     --xmi | --xmi= | -xmi | -xmi=)
        print_error "--xmi" "missing argument"
        ;;
     --xmi=* | -xmi=*)
        xmi=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        ;;
     --idl | --idl= | -idl | -idl=)
        print_error "--idl" "missing argument"
        ;;
     --idl=* | -idl=*)
        idl=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        ;;
     --tmp | --tmp= | -tmp | -tmp=)
        print_error "--tmp" "missing argument"
        ;;
     --tmp=* | -tmp=*)
        tmp=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        ;;
     --lang | --lang= | -lang | -lang=)
        print_error "--impl_lang" "missing argument"
        ;;
     --lang=* | -lang=*)
        impl_lang=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        ;;
     --I | -I)
        print_error "--I" "missing argument"
        ;;
     --I* | -I*)
        searchdir="$searchdir -I`echo $1 | sed 's/[-]*I//'`"
        ;;
     --preproc-flags | --preproc-flags= | -preproc-flags | -preproc-flags=)
        # No flags specified. Ignore option.
        ;;
     --preproc-flags=* | -preproc-flags=*)
        echo "preproc_flags value=[$1]"
        preproc_flags=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
        echo "preproc_flags=[$preproc_flags]"
        ;;
     ## These options are hidden to the user
     --noccm)
        noccm="yes"
        ;;
 --requiresdatastore)
    requiresdatastore="yes"
    ;;
     --dev | -dev)
        dev_mode="yes"
        ;;
     *)
        print_error $1 "invalid option"
        ;;
  esac
  shift;
done


check_mandatory_options $xml $obj $tmp
check_xml_filename
check_tmp_dir
check_output_dir $xmi $idl
check_search_dirs

# Export IR
find_idl
create_dummy_idl
set_openccm_env
export_ir


