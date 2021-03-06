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
#  


if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi

prompt="$echo"
prompt_input="$echo"
info_checking="Checking for"
info_check_OK=" OK!"
info_check_Failed=" FAILED !!!"
cache_file="$PWD/.config.cache"
site_config="site.mk"
func_return=""
CDMW_TUTORIAL_HOME=$PWD

# Possible return code from the test functions
ret_fatal=2
ret_failed=1
ret_ok=0

# Variable to be set
#MAKE=""
#CXX=""
TARGET_HOST=""
#LD_ARGS=""


#CDMW_VERSION="V1.0 Beta0"
CDMW_HOME_BIN_SUFFIX=bin
CDMW_HOME_IDL_SUFFIX=idl
CDMW_HOME_LIB_SUFFIX=lib
CDMW_HOME_INC_SUFFIX=include
CDMW_HOME_DATA_SUFFIX=share
CDMW_HOME_DOC_SUFFIX=doc

# ----------------------------------------------------------------
# Default value used to ask user
# ----------------------------------------------------------------

CDMW_HOME_DEFAULT=/usr/local/

ORB_PREFIX_DEFAULT=/usr/local
ORB_IDL_PATH_DEFAULT=idl
ORB_BIN_PATH_DEFAULT=bin
ORB_LIB_PATH_DEFAULT=lib
ORB_INC_PATH_DEFAULT=include

ORB_JAVA_PREFIX_DEFAULT=/usr/local
ORB_JAVA_IDL_PATH_DEFAULT=idl

XERCES_PREFIX_DEFAULT=/usr/local
XERCES_INC_PATH_DEFAULT=include
XERCES_LIB_PATH_DEFAULT=lib
XERCES_JAR_PATH_DEFAULT=lib

XALAN_PREFIX_DEFAULT=/usr/local
XALAN_JAR_PATH_DEFAULT=lib

ZIP_PREFIX_DEFAULT=/usr/local
ZIP_BIN_PATH_DEFAULT=bin

STLPORT_PREFIX_DEFAULT=/usr/local
STLPORT_INC_PATH_DEFAULT=include
STLPORT_LIB_PATH_DEFAULT=lib

FTP_DIR_DEFAULT=/home/ftp



# ----------------------------------------------------------------
# Store the specified varaiable and value in a cache file
#
# First  argument the variable to be cached
# Second argument the value to be cached
# ----------------------------------------------------------------
cache_variable()
{
    $echo "$1\c" >> $cache_file
    $echo "=\c"  >> $cache_file
    $echo "$2"  >> $cache_file
}


# ----------------------------------------------------------------
# Prompt the user to enter a variable value
#
# ----------------------------------------------------------------
prompt_for()
{
    VALUE_NAME=$1
    VALUE=$2
    DEFAULT_VALUE=$3
    PROMPT_STRING="$4"

    if test "$VALUE" = ""
    then
        VALUE_PROMPT=$DEFAULT_VALUE
    else
        VALUE_PROMPT=$VALUE
    fi

    $prompt "$PROMPT_STRING"
    $prompt_input "[$VALUE_PROMPT] \c"

    read input
    if test "$input" != ""
    then
        VALUE="$input"
    else
        VALUE="$VALUE_PROMPT"
    fi

    cache_variable $VALUE_NAME $VALUE
    func_return="$VALUE"
}


# ----------------------------------------------------------------
# Check the existence of a specified file
#
# ----------------------------------------------------------------
check_for_file()
{
    ret_status=$ret_ok
    FILE_TO_CHECK=$1
    PROMPT_STRING=$2
    $echo "$info_checking $PROMPT_STRING"

    if test ! -f $FILE_TO_CHECK 
    then
        $echo " ... Not found $info_check_Failed"
        ret_status=$ret_fatal
    else
        $echo "$info_check_OK"
    fi

    return $ret_status

}
    
    
# ----------------------------------------------------------------
# Ask the path where CARDAMOM must be installed
#
# ----------------------------------------------------------------
ask_cdmw_install()
{

    ret_status=$ret_ok

    #..............................................
    prompt_for  CDMW_HOME_DEFAULT\
               "$CDMW_HOME" \
                $CDMW_HOME_DEFAULT\
               '\nEnter the folder where CARDAMOM has been installed'
    CDMW_HOME=$func_return

    return $ret_status;

}

# ----------------------------------------------------------------
# Set hosts
#
# ----------------------------------------------------------------
set_hosts()
{
    ret_status=$ret_ok

    #..............................................
    echo
    echo "Now set host names"
    echo "This is usefull for a set of usage examples (demo_lb, ccm_lb, ft_clock..)"
    echo "Warning: this will work only if you type '. ./configure.sh'"
    prompt_for   HOSTNAME1 \
                 "$HOSTNAME1" \
                 HOSTNAME1 \
                 '\nEnter the HOSTNAME1'
                 export HOSTNAME1=$func_return

    prompt_for   HOSTNAME2 \
                 "$HOSTNAME2" \
                 HOSTNAME2 \
                 '\nEnter the HOSTNAME2'
                 export HOSTNAME2=$func_return

    prompt_for   HOSTNAME3 \
                 "$HOSTNAME3" \
                 HOSTNAME3 \
                 '\nEnter the HOSTNAME3'
                 export HOSTNAME3=$func_return

    return $ret_status;
}

# ----------------------------------------------------------------
# Copy the site configuration file from the information 
#   previously collected
#
# ----------------------------------------------------------------
build_site_config_file()
{

    if test -f $site_config
    then
        date_of_the_day="`date '+%y%m%d-%H%M%S'`"
        mv $site_config .$site_config.$date_of_the_day
    fi

    cp $CDMW_HOME/share/tutorialconfigs/$site_config .
    cat >> $site_config <<__EOF__

CDMW_TUTORIAL_HOME=${PWD}
ORB=\$(ORB_CPP)
	 
__EOF__

}


# ----------------------------------------------------------------
# Copy the site configuration file from the information 
#   previously collected
#
# ----------------------------------------------------------------
build_available_tutorial_list_file()
{
    if test -f list.mk
    then
        date_of_the_day="`date '+%y%m%d-%H%M%S'`"
        mv list.mk .list.mk.$date_of_the_day
    fi

   TUTORIAL_LIST=""

   PACKAGE_LIST=`cat $CDMW_HOME/share/tutorialconfigs/cdmw.config | grep config.packages | cut -d= -f2`
	CDMW_INTERFACES=`cat $CDMW_HOME/share/tutorialconfigs/cdmw.config | grep config.interfaces | cut -d= -f2`

   for intf  in `echo $CDMW_INTERFACES`; do
	   case $intf in
		   c++)
			   # C++ interface => turorial c++ available
				for pkg in `echo $PACKAGE_LIST`; do
				  case $pkg in
				    DEFAULT_CONFIGURATION)
					    TUTORIAL_LIST="$TUTORIAL_LIST hello supervision supervisionMngt1 supervisionMngt2 event lifeCycleService thread trace init_generationCode namingAndRepository"
					   ;;
						
					DEFAULT_CONFIGURATION_CCM)
					   TUTORIAL_LIST="$TUTORIAL_LIST ccm010 ccm020 ccm030 ccm040 ccm050 ccm060 ccm070 ccm080 ccm110 ccm120 ccm130 ccm140 ccm220 ccm230 ccm310"
                       TEST_FT=`echo $PACKAGE_LIST | grep DEFAULT_CONFIGURATION_FT`
                       if ! [ "X$TEST_FT" = "X" ]
                       then
                           TUTORIAL_LIST="$TUTORIAL_LIST ccmft"
                       fi
                       TEST_LB=`echo $PACKAGE_LIST | grep LOAD_BALANCING`
                       if ! [ "X$TEST_LB" = "X" ]
                       then
                           TUTORIAL_LIST="$TUTORIAL_LIST ccmlb"
                       fi
						;;

				        LOAD_BALANCING)
					   TUTORIAL_LIST="$TUTORIAL_LIST lb_initialization demo_lb"
					       ;;

				        TIME)
					   TUTORIAL_LIST="$TUTORIAL_LIST clock controlled_clock"
					       ;;

				        TIME_FT)
					   TUTORIAL_LIST="$TUTORIAL_LIST ftclock"
					       ;;

					SYST_MNGT)
					   TUTORIAL_LIST="$TUTORIAL_LIST"
						;;

					ORB_SUPPORT)
					   TUTORIAL_LIST="$TUTORIAL_LIST hello"
						;;

					LIFECYCLE)
					   TUTORIAL_LIST="$TUTORIAL_LIST lifeCycleService"
						;;

					NAMING_ITF)
					   TUTORIAL_LIST="$TUTORIAL_LIST namingAndRepository"
						;;

					CODE_GENERATION)
						TUTORIAL_LIST="$TUTORIAL_LIST init_generationCode"
						;;
					esac
				done
			;;
		esac
	done

echo "TUTORIAL_LIST=$TUTORIAL_LIST"

    cat > list.mk <<__EOF__

TUTORIAL_LIST=${TUTORIAL_LIST}
	 
__EOF__


}
   
	 

# ----------------------------------------------------------------
# Determine host
#
# ----------------------------------------------------------------
determine_host()
{
    ret_status=$ret_ok
    TARGET_HOST="`$CDMW_HOME/bin/config.guess`"
    if test "$TARGET_HOST" = ""
    then
        $echo " ... unable to determine the platform"
	ret_status=$ret_fatal
    else
        $echo " ... Configuration for $TARGET_HOST platform"
    fi

    return $ret_status
}



# ----------------------------------------------------------------
# Print information in case of successful check
#
# ----------------------------------------------------------------
print_success()
{
    $echo ""
    $echo "The CARDAMOMtutorial build is now configured."
    $echo ""

    print_support
}


# ----------------------------------------------------------------
# Print information in case of installation failure
#
# ----------------------------------------------------------------
print_failure()
{

    $echo "The CARDAMOMtutorial configuration has failed"
    $echo "Please check all your installation information"
    $echo "In case of problem, contact the CARDAMOM support"
    print_support

}


# ----------------------------------------------------------------
# Print support information 
#
# ----------------------------------------------------------------
print_support()
{

    $echo ""

}


# ----------------------------------------------------------------
# Print Welcome message
#
# ----------------------------------------------------------------
print_welcome()
{

    $echo "=========================================================="
    $echo "CARDAMOMtutorial configuration tool"
    $echo ""
    $echo "This tool will guide you along the installation"
    $echo "=========================================================="

}


# ----------------------------------------------------------------
# Check the result of command
# ----------------------------------------------------------------
check_status()
{

    if test "$1" = "$ret_ok"
    then
        return
    fi

    if test "$1" = "$ret_failed"
    then
        $echo "WARNING: CARDAMOMtutorial has not been tested with this release ! "
        $echo "Continue anyway" 
    fi

    if test "$1" = "$ret_fatal"
    then
        $echo "FATAL: CARDAMOMtutorial cannot be built"
        exit $ret_fatal
    fi


}


# ----------------------------------------------------------------
# Generate all file whose the behaviour depend on installation data
#
# ----------------------------------------------------------------
generate_install_dependency()
{
    ret_status=$ret_ok

    $echo "\nCustomization of source files in accordance with selected configuration"
    $echo "------------------------------------------------------------------------------"

#    ORB_CPP_NAME=`cat $site_config | grep ORB_CPP_NAME | cut -d= -f2 | cut -d'_' -f1`

    echo "ORB_CPP_NAME=$ORB_CPP_NAME"

    xml_files=`find . -name "*.$ORB_CPP_NAME" -type f -print | sed "s/\.$ORB_CPP_NAME//"`
    for f in $xml_files; do
        if test -f $f
        then
            $echo "removing $f ..."
            rm -f $f
        fi
        $echo "creating $f from $f.$ORB_CPP_NAME ..."
        cp $f.$ORB_CPP_NAME $f
    done
	 
    return $ret_status
}


# **********************************************************************
# **********************************************************************
#
# MAIN PROCEDURE
#
# **********************************************************************
# **********************************************************************

print_welcome

ask_cdmw_install
determine_host
set_hosts

# We build the file containing all folder and tool required to build Cardamom
build_site_config_file
build_available_tutorial_list_file

#generate_install_dependency

# add +x for all script files
chmod +x `find . -name "*.sh"`

print_success


