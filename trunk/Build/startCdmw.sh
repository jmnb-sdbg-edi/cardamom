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

SCRIPTNAME=`basename $0`

GENERIC_OPTS="[-f CFG_FILE | --cfgfile CFG_FILE]"
GENERIC_OPTS="$GENERIC_OPTS [-c CFG_NUM | --cfgnum CFG_NUM]"
GENERIC_OPTS="$GENERIC_OPTS [-v | --versbose] [-V | --moreverbose]"
GENERIC_OPTS="$GENERIC_OPTS [-a | --color] [-h | --help]"
TARGET_OPTS_LIST="TARGET_OPT1;TARGET_OPT2;..."
GENERIC_TRGT_OPTS="<TARGET | -t TARGET | --target TARGET>"
GENERIC_TRGT_OPTS="$GENERIC_TRGT_OPTS [-O \"$TARGET_OPTS_LIST\" |"
GENERIC_TRGT_OPTS="$GENERIC_TRGT_OPTS --targetopts \"$TARGET_OPTS_LIST\"]"

# Define env vars for ANT.
ANT_OPTS="-Xms64M -Xmx500M"
ANT_ARGS=""
export ANT_OPTS ANT_ARGS

# Basic ANT command.
ANT_CMD="ant -f MasterBuild.xml"

# Initialize the variables to their default values.
TARGET=
VERBOSE_MODE="-q -emacs"
ANT_LOGGER=
ANSI_COLOR_LOGGER="-logger org.apache.tools.ant.listener.AnsiColorLogger"
DEF_CONFIG_FILE="-Dmb.site.properties"
DEF_CONFIG_NUMBER="-Dmb.config.number"
SELECT_CONFIG=
LOOSE_CHECK_MODE="-Drequire.csci.check.mode=loose"
STRICT_CHECK_MODE="-Drequire.csci.check.mode=strict"
PASSIVE_MODE="-Dpassive.mode=true"
GENERATE_GCOV="-Dgenerate.gcov.infos=true"
SLOPPY_CHECK="-Dsloppy.check=true"
HAS_VALIDATION="-Dhas.validation=true"
HAS_INTEGRATION="-Dhas.integration=true"
HAS_QUALIFICATION="-Dhas.qualification=true"
WITHOUT_BINARY_VERSION=
IGNORE_CCACHE="-Dignore.ccache=true"
OVERRIDE_OLDCONFIG="-Doverride.oldconfig=true"
WITHOUT_UNIT_TESTS="-Dwithout.unittests=true"
CFG_OPTS=
CPL_OPTS=
INS_OPTS=
RUN_OPTS=
CPL_CSCIS=
INS_BIN_OPTS=
GEN_OPTS=

print_usage() {
    echo "usage: $SCRIPTNAME $GENERIC_TRGT_OPTS $GENERIC_OPTS"
    exit 0
}

# Display the usage if the script has no command-line arguments.
if [ $# -eq 0 ]; then
    print_usage
fi

print_help() {
    S1="passivemode"
    S2="validation; integration; qualification"
    S3="sloppycheck; ignoreccache"
    S4="cscis=CSCI_NAME1,CSCI_NAME2,..."
    S5="host1=name host2=name"
    S6="host3=name host4=name host5=name target1=name target2=name target3=name target4=name target5=name"
    S7="c++; java; ada; timescale=n; CSCI; CSCI/CSC"
    S8="withoutbinaryversion"
    S9="onlyvalidationtest"
    S10="onlyintergrationtest"
    S11="onlyqualificationtest"
    S12="withouttools"
    S13="onlytools"
    S14="gccdir=<path|none> makedir=<path|none> binutilsdir=<path|none>"
    S15="generategcov"
    S16="withoutunittests"
    
    echo "usage: $SCRIPTNAME $GENERIC_TRGT_OPTS $GENERIC_OPTS"
    echo
    echo "General options"
    echo "---------------"
    echo "    -f | --cfgfile CFG_FILE   use a specific configuration file"
    echo "    -c | --cfgnum  CFG_NUM    use a specific configuration number"
    echo "    -v | --verbose            verbose"
    echo "    -V | --moreverbose        more verbose"
    echo "    -a | --color              use ANSI color logger"
    echo "    -h | --help               help screen"
    echo
    echo "    -O <loosecheckmode|strictcheckmode> check mode for the CSCIs version"
    echo
    echo "List of targets"
    echo "---------------"
    echo "* get the list of needed tools:"
    echo "    needed-tools"
    echo "* return the version of the product and the version of each CSCIs:"
    echo "    version"
    echo
    echo "* configure of the build environment:"
    echo "    configure         | optional: $S1; $S2; $S15"
    echo "    configure-valid   | optional: $S1"
    echo "    configure-integ   | optional: $S1"
    echo "    configure-qualif  | optional: $S1"
    echo "    reconfigure"
    echo
    echo "* compile the source files:"
    echo "    compile           | optional: $S3; $S2; $S16; $S4"
    echo "    compile-valid     | optional: $S3; $S4"
    echo "    compile-integ     | optional: $S3; $S4"
    echo "    compile-qualif    | optional: $S3; $S4"
    echo
    echo "* install the product:"
    echo "    install           | optional: $S4 "
    echo "    install-valid     | mandatory: $S5 | optional: $S6; $S4; $S9"
    echo "    install-integ     | mandatory: $S5 | optional: $S6; $S4; $S10"
    echo "    install-qualif    | mandatory: $S5 | optional: $S6; $S4; $S11; $S8; $S12; $S13; $S14"
    echo "    install-binary    | optional: $S14"
    echo
    echo "* run the test applications:"
    echo "    runtest           | optional: $S7"
    echo
    echo "* install files to inspect:"
    echo "    inspect-valid     | mandatory: $S4"
    echo "    inspect-qualif    | mandatory: $S4"
    echo
    echo "* generate the API documentation:"
    echo "    genpublicdoc"
    echo "    genfulldoc"
    echo
    echo "* combo targets:"
    echo "    compile-and-install"
    echo
    echo "* other targets:"
    echo "    addcsc        helper to create a new CSC"
    echo "    clean         thoroughly clean the CSCI directories"
    echo "    distclean     clean the CSCI directories"
    echo "    configsum     print a summary of the configuration files"
    exit 0
}

print_msg() {
    echo "$SCRIPTNAME: $1"
    exit 1
}

target_already_set() {
    if [ -n "$TARGET" ]; then
        print_msg "only one target can be set at once"
    fi
}

check_target() {
    if [ "$1" = "needed-tools" -o \
         "$1" = "version" -o \
         "$1" = "configure" -o \
         "$1" = "configure-valid" -o \
         "$1" = "configure-integ" -o \
         "$1" = "configure-qualif" -o \
         "$1" = "reconfigure" -o \
         "$1" = "compile" -o \
         "$1" = "compile-valid" -o \
         "$1" = "compile-integ" -o \
         "$1" = "compile-qualif" -o \
         "$1" = "install" -o \
         "$1" = "install-valid" -o \
         "$1" = "install-integ" -o \
         "$1" = "install-qualif" -o \
         "$1" = "install-binary" -o \
         "$1" = "runtest" -o \
         "$1" = "genpublicdoc" -o \
         "$1" = "genfulldoc" -o \
         "$1" = "compile-and-install" -o \
         "$1" = "addcsc" -o \
         "$1" = "clean" -o \
         "$1" = "distclean" -o \
         "$1" = "configsum" -o \
         "$1" = "inspect-valid" -o \
         "$1" = "inspect-qualif" -o \
         "$1" = "staticdeps" \
        ]; then

        TARGET=$1
    else
        print_msg "unknown target \"$1\""
    fi
}

parse_target_options() {
    for o in `echo $1 | sed -e 's:;: :g'`; do
        case $o in
        
            generategcov)
                if [ -z "`echo  $CFG_OPTS | grep 'generate.gcov.infos'`" ]; then
                    CFG_OPTS="$CFG_OPTS $GENERATE_GCOV"
                fi
                ;;
        
            loosecheckmode)
                if [ -z "`echo $GEN_OPTS | grep 'csci.check.mode'`" ]; then
                    GEN_OPTS="$GEN_OPTS $LOOSE_CHECK_MODE"
                fi
                ;;

            strictcheckmode)
                if [ -z "`echo $GEN_OPTS | grep 'csci.check.mode'`" ]; then
                    GEN_OPTS="$GEN_OPTS $STRICT_CHECK_MODE"
                fi
                ;;
            passivemode)
                if [ -z "`echo $CFG_OPTS | grep 'passive.mode'`" ]; then
                    CFG_OPTS="$CFG_OPTS $PASSIVE_MODE"
                fi
                ;;

            sloppycheck)
                if [ -z "`echo $CPL_OPTS | grep 'sloppy.check'`" ]; then
                    CPL_OPTS="$CPL_OPTS $SLOPPY_CHECK"
                fi
                ;;

            ignoreccache)
                if [ -z "`echo $CPL_OPTS | grep 'ignore.ccache'`" ]; then
                    CPL_OPTS="$CPL_OPTS $IGNORE_CCACHE"
                fi
                ;;
                
            withoutbinaryversion)
                WITHOUT_BINARY_VERSION="-Dwithout.binary.version=true"
                ;;

            onlyvalidationtest)
                ONLY_VALIDATION_TEST="-Donly.validation.test=true"
                ;;

            onlyintergrationtest)
                ONLY_INTEGRATION_TEST="-Donly.integration.test=true"
                ;;

            onlyqualificationtest)
                ONLY_QUALIFICATION_TEST="-Donly.qualification.test=true"
                ;;

            withouttools)
                WITHOUT_TOOLS="-Dwithout.tools=true"
                ;;

            onlytools)
                ONLY_TOOLS="-Donly.tools=true"
                ;;

            withoutunittests)
                if [ -z "`echo $CPL_OPTS | grep 'without.unittests'`" ]; then
                    CPL_OPTS="$CPL_OPTS $WITHOUT_UNIT_TESTS"
                fi
                ;;

            validation)
                if [ -z "`echo $CFG_OPTS | grep 'has.validation'`" ]; then
                    CFG_OPTS="$CFG_OPTS $HAS_VALIDATION"
                fi
                if [ -z "`echo $CPL_OPTS | grep 'has.validation'`" ]; then
                    CPL_OPTS="$CPL_OPTS $HAS_VALIDATION"
                fi
                ;;

            integration)
                if [ -z "`echo $CFG_OPTS | grep 'has.integration'`" ]; then
                    CFG_OPTS="$CFG_OPTS $HAS_INTEGRATION"
                fi
                if [ -z "`echo $CPL_OPTS | grep 'has.integration'`" ]; then
                    CPL_OPTS="$CPL_OPTS $HAS_INTEGRATION"
                fi
                ;;

            qualification)
                if [ -z "`echo $CFG_OPTS | grep 'has.qualification'`" ]; then
                    CFG_OPTS="$CFG_OPTS $HAS_QUALIFICATION"
                fi
                if [ -z "`echo $CPL_OPTS | grep 'has.qualification'`" ]; then
                    CPL_OPTS="$CPL_OPTS $HAS_QUALIFICATION"
                fi
                ;;

            host*=*)
                INS_OPTS="$INS_OPTS $o"
                ;;

            gccdir=*)
                INS_BIN_OPTS="$INS_BIN_OPTS -Dinstall.$o"
                ;;

            makedir=*)
                INS_BIN_OPTS="$INS_BIN_OPTS -Dinstall.$o"
                ;;

            binutilsdir=*)
                INS_BIN_OPTS="$INS_BIN_OPTS -Dinstall.$o"
                ;;

            target*=*)
                INS_OPTS="$INS_OPTS `echo $o | sed -e 's:target:targethost:'`"
                ;;

            cscis=*)
                CPL_CSCIS="-Dconfig.sel.min.cscis=`echo $1 | cut -d= -f2 | \
                          sed -e 's: ::g' -e 's:,: :g'`"
                ;;

            overrideoldconfig)
                CFG_OPTS="$CFG_OPTS $OVERRIDE_OLDCONFIG"
                ;;
                
            *)
                RUN_OPTS="${RUN_OPTS} $o";
                ;;
        esac
    done
}

# Parse the command-line arguments.
while [ -n "$*" ]; do
    case "$1" in
        -h | --help)
            print_help
            ;;

        -v)
            VERBOSE_MODE=
            ;;

        -V)
            VERBOSE_MODE="-v"
            ;;

        -a | --color)
            if [ -z "$ANT_LOGGER" ]; then
                ANT_LOGGER=$ANSI_COLOR_LOGGER
            fi
            ;;

        -f | --cfgfile)
            if [ -n "$2" -a -z "`echo $2 | grep '^-'`" ]; then
                DIRNAME=`dirname $1`
                DIRNAME=`cd $DIRNAME; pwd`
                SELECT_CONFIG="$DEF_CONFIG_FILE=$DIRNAME/`basename $2`"
            else
                print_msg "missing configuration file name"
            fi
            shift
            ;;

        -c | --cfgnum)
            if [ -n "$2" -a -z "`echo $2 | grep '^-'`" ]; then
                # Check if it's an integer
                if [ -n "`echo $2 | sed -e 's:[0-9]::g'`" ]; then
                    print_msg "$2 is not an integer"
                fi
                SELECT_CONFIG="$DEF_CONFIG_NUMBER=$2"
            else
                print_msg "missing configuration number"
            fi
            shift
            ;;

        -t | --target)
            if [ -n "$2" -a -z "`echo $2 | grep '^-'`" ]; then
                check_target $2
            else
                print_msg "missing target name"
            fi
            shift
            ;;

        -O | --targetopts)
            if [ -n "$2" -a -z "`echo $2 | grep '^-'`" ]; then
                parse_target_options "$2"
            else
                print_msg "missing target options"
            fi
            shift
            ;;

        -*)
            print_msg "unknown option \"$1\""
            ;;

        *)
            check_target $1
            ;;
    esac
    shift
done

# Check if the ANT target is set.
if [ -z "$TARGET" ]; then
    print_msg "you must specify one target"
fi

ALL_TARGET_OPTIONS=
case "$TARGET" in
    configure)
        ALL_TARGET_OPTIONS=$CFG_OPTS

        # Alter the target name (see below for explanation)
        if [ -n "`echo $CFG_OPTS | grep 'has.validation'`" -o \
             -n "`echo $CFG_OPTS | grep 'has.integration'`" -o \
             -n "`echo $CFG_OPTS | grep 'has.qualification'`" ]; then

            TARGET="reconfigure"
        fi
        ;;
   
    configure-valid | configure-integ | configure-qualif)
        case "$TARGET" in
            *-valid)
                if [ -z "`echo $CFG_OPTS | grep 'has.validation'`" ]; then
                    CFG_OPTS="$CFG_OPTS $HAS_VALIDATION"
                fi
                ;;
            *-integ)
                if [ -z "`echo $CFG_OPTS | grep 'has.integration'`" ]; then
                    CFG_OPTS="$CFG_OPTS $HAS_INTEGRATION"
                fi
                ;;
            *-qualif)
                if [ -z "`echo $CFG_OPTS | grep 'has.qualification'`" ]; then
                    CFG_OPTS="$CFG_OPTS $HAS_QUALIFICATION"
                fi
                ;;
        esac

        if [ -n "`echo $CFG_OPTS | grep 'has.qualification'`" -a \
             -z "`echo $CFG_OPTS | grep 'has.integration'`" ]; then

            CFG_OPTS="$CFG_OPTS $HAS_INTEGRATION"
        fi

        ALL_TARGET_OPTIONS=$CFG_OPTS

        # Alter the target name as "configure" will always create a new config
        # file which is not really what we have in mind when we are configuring
        # the environment for the validation, integration or qualification.
        TARGET="reconfigure"
        ;;

    compile | compile-valid | compile-integ | compile-qualif)
        case "$TARGET" in
            *-valid)
                if [ -z "`echo $CPL_OPTS | grep 'has.validation'`" ]; then
                    CPL_OPTS="$CPL_OPTS $HAS_VALIDATION"
                fi
                ;;
            *-integ)
                if [ -z "`echo $CPL_OPTS | grep 'has.integration'`" ]; then
                    CPL_OPTS="$CPL_OPTS $HAS_INTEGRATION"
                fi
                ;;
            *-qualif)
                if [ -z "`echo $CPL_OPTS | grep 'has.qualification'`" ]; then
                    CPL_OPTS="$CPL_OPTS $HAS_QUALIFICATION"
                fi
                ;;
        esac

        ALL_TARGET_OPTIONS=$CPL_OPTS
        TARGET=`echo $TARGET | sed -e 's:-valid::' \
                                   -e 's:-integ::' \
                                   -e 's:-qualif::'`

        ANT_LOGGER="-logger com.thalesgroup.tools.ant.listener.CompilationLogger"
        CLASSPATH=$PWD/delivery/lib/libcdmwtasks.jar:$CLASSPATH
        export CLASSPATH
        ;;

    install-qualif)
        if [ -z "`echo ${INS_OPTS} | grep host1`" -o \
             -z "`echo ${INS_OPTS} | grep host2`" ]; then

             print_msg "missing mandatory target options, check the usage"
        fi

        ALL_TARGET_OPTIONS=$INS_OPTS 
        ;;
    install-valid | install-integ )
        if [ -z "`echo ${INS_OPTS} | grep host1`" -o \
             -z "`echo ${INS_OPTS} | grep host2`" ]; then

             print_msg "missing mandatory target options, check the usage"
        fi

        ALL_TARGET_OPTIONS=$INS_OPTS 
        ;;

    install-binary)
        ALL_TARGET_OPTIONS=$INS_BIN_OPTS
        ;;

    runtest)
        ANT_LOGGER="-logger com.thalesgroup.tools.ant.listener.XDefaultLogger"
        VERBOSE_MODE="-emacs"
        ALL_TARGET_OPTIONS=$RUN_OPTS
        CLASSPATH=$PWD/delivery/lib/libcdmwtasks.jar:$CLASSPATH
        export CLASSPATH
        ;;

    inspect-valid | inspect-qualif)
        if [ -z "$CPL_CSCIS" ]; then
            print_msg "missing mandatory target options, check the usage"
        fi
        ;;
esac

# Run ANT.
case "$TARGET" in
    install-valid | install-integ | install-qualif)
        $ANT_CMD $ANT_LOGGER $VERBOSE_MODE $SELECT_CONFIG \
             $TARGET $WITHOUT_BINARY_VERSION $ONLY_VALIDATION_TEST \
             $ONLY_INTEGRATION_TEST $ONLY_QUALIFICATION_TEST \
             $WITHOUT_TOOLS $ONLY_TOOLS $INS_BIN_OPTS $GEN_OPTS\
             -Dinstall.options="$ALL_TARGET_OPTIONS" "$CPL_CSCIS" 
        ;;

    runtest)
        $ANT_CMD $ANT_LOGGER $VERBOSE_MODE $SELECT_CONFIG \
            $TARGET $GEN_OPTS -Druntest.options="$ALL_TARGET_OPTIONS"
        ;;

    *)
        $ANT_CMD $ANT_LOGGER $VERBOSE_MODE $SELECT_CONFIG \
            $TARGET $GEN_OPTS $ALL_TARGET_OPTIONS "$CPL_CSCIS"
esac
