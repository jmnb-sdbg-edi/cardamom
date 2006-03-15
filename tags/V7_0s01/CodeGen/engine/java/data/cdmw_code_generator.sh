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
params=
generate_xmi="yes"
generate_idl="yes"
generate_xmi_only="no"
generate_idl2_only="no"
force_generation="no"

##
# Function : display help.
#
display_help() {
   echo
   echo "NAME"
   echo "     $script_name"
   echo
   echo "SYNOPSIS"
   echo "     $script_name --xml=<FILE> --dir=<DIR> [--validate]"
   echo "     [ --lang=<cpp|ada|java> ] [ --generate-idl2-only ]"
   echo "     [ --generate-xmi-only ] [ --validate-only ] [ --help ]"
   echo
   echo "DESCRIPTION"
   echo "     The $script_name script reads an XML file used for"
   echo "     the CDMW code generation and generates source code in native"
   echo "     language for the CDMW  services and/or CIF using information"
   echo "     gathered from the XML file."
   echo
   echo "OPTION"
   echo "     --help                            display this screen"
   echo "     --xml=<FILE>               (*)    file name of the XML doc."
   echo "     --dir=<DIR>                (*)    default output directory"
   echo "     --lang=<cpp|ada|java>             implementation languages"
   echo "     --validate                        validate all XML documents"
   echo "     --validate-only                   validate input XML and exit"
   echo "     --generate-xmi-only               export to XMI and exit"
   echo "     --generate-idl2-only              export to IDL2 and exit"
   echo "     --force-generation                force code generation, overwriting any file"
   echo
   echo "(*) = those options are mandatory"
   echo
}

##
# Function : print an error message.
#
print_error() {
   msg_prefix="`date +'%y/%m/%d %H:%M:%S'` ERR CDGN : $script_name: error: $1"
   if [ $# -eq 2 ]; then
      if [ `echo $2 | grep -c missing` -eq 0 ]; then
         echo "$msg_prefix: $2"
      else
         echo "$msg_prefix: $2; use --help to show usage"
      fi
   else
      if [ `echo $1 | grep -c missing` -eq 0 ]; then
         echo "$msg_prefix"
      else
         echo "$msg_prefix; use --help to show usage"
      fi
   fi
   exit $failed
}

##
# Function : analyze the command line arguments.
#
analyze_command_line_arguments() {
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
         --dir | --dir= | -dir | -dir=)
            print_error "--dir" "missing argument"
            ;;
         --dir=* | -dir=*)
            outdir=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
            ;;
         --lang | --lang= | -lang | -lang=)
            print_error "--lang" "missing argument"
            ;;
         --lang=* | -lang=*)
            impl_lang=`echo $1 | sed 's/[-_a-zA-Z0-9]*=//'`
            ;;
         --validate | -validate)
            validate="yes"
            ;;
         --validate-only | -validate-only)
            validate_only="yes"
            ;;
         --generate-xmi-only | -generate-xmi-only)
            generate_xmi_only="yes"
            ;;
         --generate-idl2-only | -generate-idl2-only)
            generate_idl2_only="yes"
            ;;
         --force-generation)
            force_generation="yes"
            ;;
         *)
            print_error $1 "invalid option"
            ;;
      esac
      shift;
   done
}

##
# Function : check the mandatory options.
#
check_mandatory_options() {
   nb_of_mandatory_options=$1
   shift

   if [ ! $# -eq $nb_of_mandatory_options ]; then
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
      else
         xmlfile=`basename $xml`
         xmldir=`dirname $xml`
         xml=`cd $xmldir; pwd`/$xmlfile
         params="$params -c $xml"
      fi
   fi
}

##
# Function : check the default output directory.
#
check_output_dir() {
   if [ -n "$outdir" ]; then
      if [ ! -d "$outdir" ]; then
         print_error "--dir" "directory $outdir does not exist"
      else
         outdir=`cd $outdir; pwd`
         params="$params -o $outdir"
      fi
   fi
}

##
# Function : check the implementation language.
#
check_impl_lang() {
   if [ -n "$impl_lang" ]; then
      if [ "$impl_lang" != "cpp" -a "$impl_lang" != "c++" \
          -a "$impl_lang" != "ada" -a "$impl_lang" != "java" ]; then
         print_error "--lang" "$impl_lang is not a valid implementation language"
      else
         params="$params -l $impl_lang"
      fi
   else
      if [ "$generate_xmi_only" = "no" -a "$generate_idl2_only" = "no" ]; then
         echo "`date +'%y/%m/%d %H:%M:%S'` INF CDGN : No implentation language specified at command line, using default : CPP"
      fi
      params="$params -l cpp"
      impl_lang="cpp"
   fi
}

##
# Function : check the validate option
#
check_validate() {
   if [ -z "$validate" ]; then
      validate="no"
   fi
}

##
# Function : check the validate-only option
#
check_validate_only() {
   if [ -z "$validate_only" ]; then
      validate_only="no"
   fi
}

##
# Function : validate the XML document against a DTD or an XML Schema
#
validate_xml_document() {
   if [ "$validate" = "yes" -o "$validate_only" = "yes" ]; then
      if [ -f "$1" ]; then
         dummy_xml_file=$tmpdir/`basename $1`

         cat > $dummy_xml_file << __EOF__
<!DOCTYPE  dummy-document-element [
<!ELEMENT  dummy-document-element      ( cdmw )>
<!ENTITY % dummy-document-dtd    SYSTEM "$share_dir/CDMW.dtd">
<!ENTITY   dummy-document-entity SYSTEM "$1">
%dummy-document-dtd;
]>
<dummy-document-element>&dummy-document-entity;</dummy-document-element>
__EOF__

         my_classpath=@config.XercesJAVA.lib.dir@/xercesImpl.jar
         my_classpath=$my_classpath:@config.XercesJAVA.lib.dir@/xmlParserAPIs.jar
         my_classpath=$my_classpath:@config.install.dir@/lib/java/libcdmwcgengine.jar

         java -Xbootclasspath/p:$my_classpath cdmw.codegenerator.XMLValidator $dummy_xml_file

         if [ $? -eq $fatal -o $? -eq $failed ]; then
            rm -f $dummy_xml_file
            exit $?
         fi

         rm -f $dummy_xml_file
      fi
   fi
}

##
# Function : export IR to XMI and/or IDL2
#
export_ir() {
   has_only_factories_defs="`java -jar $saxon_jar_path/saxon.jar $xml \
      $share_dir/ExportHelpers.xslt TEMPLATE=has_only_factories_defs`"

   if [ "$has_only_factories_defs" = "true" ]; then
      generate_idl2_only="yes"
   fi

   if [ "$generate_xmi_only" = "yes" ]; then
      generate_only="XMI"
   elif [ "$generate_idl2_only" = "yes" ]; then
      generate_only="IDL2"
   else
      generate_only="no"
   fi
   cdmw_export_sh_args="--xml=$xml --lang=$impl_lang `java -jar $saxon_jar_path/saxon.jar $xml \
      $share_dir/ExportHelpers.xslt TEMPLATE=build_export_command_line_args \
      CDMWIDL=$cdmw_idl_path TMPDIR=$tmpdir XMIFILE=$xmi ONLY=$generate_only`"

   preprocessing_flags=`java -jar $saxon_jar_path/saxon.jar $xml \
      $share_dir/ExportHelpers.xslt TEMPLATE=get_preproc_flags \
      CDMWIDL=$cdmw_idl_path TMPDIR=$tmpdir XMIFILE=$xmi ONLY=$generate_only`
   
   if [ `echo $cdmw_export_sh_args | grep -c 'xmi='` -eq 0 ]; then
      generate_xmi="no"
   fi
   if [ `echo $cdmw_export_sh_args | grep -c 'idl='` -eq 0 ]; then
      generate_idl="no"
   fi

   if [ "$generate_xmi" = "no" -a "$generate_idl" = "no" ]; then
      echo "<XMI/>" > $xmi
   else
      $bin_dir/cdmw_export.sh $cdmw_export_sh_args --preproc-flags="$preprocessing_flags"
      if [ $? -gt 0 ]; then
         exit $?
      fi
   fi

   if [ "$generate_idl" = "yes" ]; then
      if [ `find $tmpdir -name "*_cif.idl" | wc -l` -gt 0 ]; then
         cp $tmpdir/*_cif.idl $outdir
      fi
   fi
   if [ "$generate_xmi" = "yes" ]; then
      if [ "$generate_only" = "XMI" -o "$generate_only" = "xmi" ]; then
         if [ -f "$xmi" ]; then
            cp $xmi $outdir
         fi
      fi
   fi
}

##
# Function : check if the code generator must be run.
#
# Run the code generator when any of these conditions are true :
#    - the output directory is empty,
#    - the files found in the output directory are older than either
#      the code generator templates files or the XML main description file.
#
check_start_conditions() {
   runme="false"

   # Check if the output directory is empty
   if [ `find $outdir -name "*.*" | wc -l` -eq 0 ]; then
      runme="true"
   fi

   # Check modified dates only if there are
   # files in the output directory
   if [ "$runme" = "false" ]; then
      # Get the most recent file in $outdir
      file1=`ls -t $outdir/* | head -1`
      
      # Get the most recent code generator template file
      file2=`ls -t $share_dir/*.xslt | head -1`

      # Get the list of IDL files referred by the main description file
      idls_from_xml="`java -jar $saxon_jar_path/saxon.jar $xml \
         $share_dir/ExportHelpers.xslt TEMPLATE=get_idls`"
      # Remove IDL files that do not exist
      for f in $idls_from_xml; do
         if [ -f "$f" ]; then
            idl_filelist="$idl_filelist $f"
         fi
      done
      
      # Compare the most recent file in $outdir with $xml and $file2
      if [ ! "`ls -t $file1 $file2 $idl_filelist $xml | head -1`" = "$file1" ]; then
         runme="true"
      fi
   fi

   # Finally, if none of the previous tests updated $runme to true 
   # then obviously there is nothing to do.
   if [ "$runme" = "false" ]; then
      echo "$script_name: files in $outdir are up-to-date; exiting..."
      exit $ok
   fi
}

###############################################################################
# Main section
#

# Create a temporary directory for the export process
# and ensure that it will be destroyed whenever the script ends or aborts
tmpdir="${TMP:-/tmp}/${USER}_$$"
trap 'rm -rf "$tmpdir"' 0
trap 'exit' 1 2 15
mkdir $tmpdir

# Variables
share_dir=@config.install.dir@/share
bin_dir=@config.install.dir@/bin
saxon_jar_path=@config.Saxon.lib.dir@
cdmw_idl_path=@config.install.dir@/idl
xmi="$tmpdir/generated.xmi"

# Checkings
analyze_command_line_arguments $*
check_validate_only
check_xml_filename

if [ "$validate_only" = "yes" ]; then
   check_mandatory_options 1 $xml
   validate_xml_document $xml
else
   check_mandatory_options 2 $xml $outdir
   check_output_dir
   check_validate
   check_impl_lang
   if [ "$force_generation" = "no" ]; then
        check_start_conditions
   fi
   validate_xml_document $xml
   export_ir
   if [ "$generate_xmi_only" = "no" -a "$generate_idl2_only" = "no" ]; then
        java -cp @config.install.dir@/lib/java/libcdmwcgengine.jar:$saxon_jar_path/saxon.jar \
            cdmw.codegenerator.TemplateManager $params -I $share_dir $xmi
   fi
fi


