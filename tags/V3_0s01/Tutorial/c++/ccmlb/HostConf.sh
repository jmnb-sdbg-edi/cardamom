#!/bin/sh
#* =========================================================================== *
#* THALES (R) - CARDAMOM
#*
#* Copyright (c) THALES 2000-2004 All rights reserved
#* This file and the information  it contains  are   property  of  THALES  and
#* confidential.   They  shall  not be reproduced nor disclosed  to any person
#* except to those having  a need  to know them without  prior written consent
#* of THALES.
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
func_return=""

# Possible return code from the test functions
ret_fatal=2
ret_failed=1
ret_ok=0

# ----------------------------------------------------------------
# Print support information 
#
# ----------------------------------------------------------------
print_support()
{

    $echo ""

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

    func_return="$VALUE"
}


# ----------------------------------------------------------------
# Set hosts
#
# ----------------------------------------------------------------
set_hosts()
{
    ret_status=$ret_ok

    #..............................................
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
# Print information in case of successful check
#
# ----------------------------------------------------------------
print_success()
{
    $echo ""
    $echo "The CCM LB demo is now configured."
    $echo ""

    print_support
}


# ----------------------------------------------------------------
# Print information in case of installation failure
#
# ----------------------------------------------------------------
print_failure()
{

    $echo "The CCM LB demo configuration has failed"
    $echo "Please check all your installation information"
    $echo "In case of problem, contact the CARDAMOM support"
    print_support

}


# ----------------------------------------------------------------
# Print Welcome message
#
# ----------------------------------------------------------------
print_welcome()
{

    $echo "=========================================================="
    $echo "CCM LB demo host configuration tool"
    $echo ""
    $echo "This tool will guide you along the host configuration for the demo"
    $echo "=========================================================="

}



# **********************************************************************
# **********************************************************************
#
# MAIN PROCEDURE
#
# **********************************************************************
# **********************************************************************

print_welcome
#check if HOSTNAME1, HOSTNAME2 and HOSTNAME3 are set
set_hosts
print_success


