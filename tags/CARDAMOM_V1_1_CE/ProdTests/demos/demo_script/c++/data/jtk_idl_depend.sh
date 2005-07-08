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
# idl_depend:
# Tool for generating make dependencies of idl.
# (C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 1999
#     Arnaud Desitter - Nag Ltd. - arnaud@nag.co.uk, 1999
# $Id: jtk_idl_depend.sh,v 1.1.1.1 2005-07-08 14:31:03 moghrabi Exp $
#
###########################################################
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#   must display the following acknowledgement:
#      This product includes software developed by Ruslan Shevchenko, 
#      Ukraine, Kiev and his contributors.
# 4. Neither the name of the original author nor the names of his contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
###########################################################

set -u
#set -x

#---------------------------------------------------------
# Default values.
#---------------------------------------------------------

if [ "`echo -e`" = "" ] ; then
    echo='echo -e'
else
    echo='echo'
fi
MV='mv'
CXX_COMPL='$(CXX) $(CPPFLAGS) $(CXXFLAGS)'
IDL_COMPL='$(IDL2CXX) $(IDLFLAGS)'
ORB='ORBacus'
VAR_PREFIX='CORBA'
MK_INCLUDE_PRG='$(IDLMKINCLUDE)'

#---------------------------------------------------------
# Print version and usage.
#---------------------------------------------------------
version() {
cat >&2 << __EOF__
`basename ${0}`: makefile generator for IDL to C++ translation.
(C) Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 1999.
    Arnaud Desitter - Nag Ltd. <arnaud@nag.co.uk>, 1999.
\$Id: jtk_idl_depend.sh,v 1.1.1.1 2005-07-08 14:31:03 moghrabi Exp $
__EOF__
}

usage() {
cat >&2 << __EOF__
Usage: `basename ${0}` [options] [idl-files]

Options:
--help                 Show this message.
--version              Show version.
--list-files           List idl files to process and exit.
--idl-dir DIR          Look for idl files within directory DIR.
--idl-file FILE        Process idl file FILE.
--h-dir DIR            Write generated header files to directory DIR.
--cpp-dir DIR          Write generated cpp files to directory DIR.
--obj-dir DIR          Write generated object files to directory DIR.
--absolute-path        Use absolute path names.
--rules-only           Generate only make rules.
--variables-only       Generate only make variables.
                       The default is to generate both.
--no-object-rules      Don't generate object rules.
--no-all-variables     Don't generate general make variables.
--additive-vars        Don't set make variables to empty initial value.
--var-prefix PREFIX    Add PREFIX to make variables.
                       The default value is \`${VAR_PREFIX}'.
--old-make             Don't use += in generated Makefile.
--idl-compiler NAME    Use NAME as idl compiler.
                       The default value is \`${IDL_COMPL}'.
--extra-idl-flag FLAGS Additional flags FLAGS passed to idl compiler.
--cxx-compiler NAME    Use NAME as C++ compiler.
                       The default value is \`${CXX_COMPL}'.
--mv NAME              Use NAME as move program.
                       The default value is \`${MV}'.
--c-suffix SUFFIX      Use SUFFIX as suffix for source files.
--h-suffix SUFFIX      Use SUFFIX as suffix for header files.
--client-suffix SUFFIX Use SUFFIX as suffix for client files.
--server-suffix SUFFIX Use SUFFIX as suffix for server files.
                       The suffixes default are ORB dependent.
--client-only          Generate only client dependencies.
--server-only          Generate only server dependencies.
                       The default is to generate both.
--tie                  Support for ORBacus --tie option.
--mico-skel            Support for Mico --c++-skel option.
--omniorb-dyn          Support for OmniORB2 -a option.
--mk-include           Support for the generator of ORB independent headers.
--mk-include-prg NAME  Use NAME as make idl header program.
                       The default value is \`${MK_INCLUDE_PRG}'.
--orb ORB              Generate ORB compliant file names.
                       The default is \`${ORB}'.

This tool generates Makefile rules associated the transformation
IDL to C++ for various ORB, hence providing portability.
A set of variables are built, containing the name of C++ files (such
as client or server sources, headers and objects) associated with
a specific idl file.
In addition, cumulative variables contain all files by category.
Typical use consists of using \`include <GeneratedMakefile>' within 
your own Makefile and referring to the defined variables.
__EOF__
}

#---------------------------------------------------------
# Useful functions.
#---------------------------------------------------------
print_header() {
cat << __EOF__
# File generated by `basename ${0}`. DO NOT EDIT
# Created on: `date`.
# ORB: ${ORB}

__EOF__
}

give_obj() {
    ret=
    for i do
        ret="${ret} `basename ${i} | sed -e 's%\.[^.]*$%%'`.o"
    done
    echo ${ret}
}

add_dir() {
    DIR=${1}; shift
    ret=
    for i do
        ret="${ret} ${DIR}/${i}"
    done
    echo ${ret}
}

absolute_name() {
    PWD=`pwd`
    ret=
    for i do
        case "${i}" in
            /*)
                ret="${ret} ${i}"
                ;;
            *)
                ret="${ret} ${PWD}/${i}"
                ;;
        esac
    done
    echo ${ret}
}

print_eq() {
    ${echo} "${1}"'\t= '"${2}"
}

print_peq() {
    ${echo} "${1}"'\t+= '"${2}"
}

print_rule_depend() {
    ${echo} "${1}: ${2}"
}

print_rule_action() {
    # don't expand arguments
    for i do
        ${echo} "\t${i}"
    done
}

print_mv() {
    # expand arguments
    for i in $*; do
        ${echo} "\t${MV} "`basename ${i}`" "`dirname ${i}`
    done
}

verif_option() {
    [ "${1}" -eq 1 ] && {
        echo "`basename ${0}`: Argument expected after ${2}." >&2;
        usage; exit 1;
    }
}

#---------------------------------------------------------
# Parse options.
#---------------------------------------------------------
IDL_DIR=
IDLS=
H_DIR=
EXTRA_IDLFLAGS=

while [ "x${*-}" != "x" -a "x${stop_parse-}" = "x" ]; do
    case "${1}" in
        --orb)
            verif_option "${#}" "${1}"; shift
            ORB="${1}"
            ;;
        --idl-dir|--idl_dir)
            verif_option "${#}" "${1}"; shift
            IDL_DIR="${IDL_DIR} ${1}"
            ;;
        --extra-idl-flags|--extra_idl_flags|--extra-idl-flag|--extra_idl_flag)
            verif_option "${#}" "${1}"; shift
            EXTRA_IDLFLAGS="${EXTRA_IDLFLAGS} ${1}"
            ;;
        --idl-file|--idl_file)
            verif_option "${#}" "${1}"; shift
            IDLS="${IDLS} ${1}"
            ;;
        --var-prefix|--var_prefix)
            verif_option "${#}" "${1}"; shift
            VAR_PREFIX="${1}"
            ;;
        --h-dir|--h_dir)
            verif_option "${#}" "${1}"; shift
            H_DIR="${1}"
            ;;
        --cpp-dir|--cpp_dir)
            verif_option "${#}" "${1}"; shift
            CPP_DIR="${1}"
            ;;
        --obj-dir|--obj_dir)
            verif_option "${#}" "${1}"; shift
            OBJ_DIR="${1}"
            ;;
        --absolute-path|--absolute_path)
            ABSOLUTE=1
            ;;
        --additive-vars|--additive_vars)
            ADDITIVE_VARS=1
            ;;
        --no-all-variables|--no_all_variables)
            NO_ALL_VARIABLES=1
            ;;
        --c-suffix|--c_suffix)
            verif_option "${#}" "${1}"; shift
            C_SUFFIX_FLAG=1
            C_SUFFIX="${1}"
            ;;
        --h-suffix|--h_suffix)
            verif_option "${#}" "${1}"; shift
            H_SUFFIX_FLAG=1
            H_SUFFIX="${1}"
            ;;
        --client-suffix|--client_suffix)
            verif_option "${#}" "${1}"; shift
            CL_SUFFIX_FLAG=1
            CL_SUFFIX="${1}"
            ;;
        --server-suffix|--server_suffix)
            verif_option "${#}" "${1}"; shift
            SKEL_SUFFIX_FLAG=1
            SKEL_SUFFIX="${1}"
            ;;
        --tie)
            TIE=1
            ;;
        --mico-skel|--mico_skel)
            MICO_SKEL=1
            ;;
        --omniorb-dyn|--omniorb_dyn)
            OMNIORB_DYN=1
            ;;
        --server-only|--server_only)
            unset NO_SERVER
            NO_CLIENT=1
            ;;
        --client-only|--client_only)
            unset NO_CLIENT
            NO_SERVER=1
            ;;
        --idl-compiler|--idl_compiler)
            verif_option "${#}" "${1}"; shift
            IDL_COMPL="${1}"
            ;;
        --cxx-compiler|--cxx_compiler)
            verif_option "${#}" "${1}"; shift
            CXX_COMPL="${1}"
            ;;
        --mv)
            verif_option "${#}" "${1}"; shift
            MV="${1}"
            ;;
        --old-make|--old_make)
            OLD_MAKE=1
            ;;
        --variables-only|--variables_only)
            unset NO_VARIABLES
            NO_RULES=1
            ;;
        --rules-only|--rules_only)
            unset NO_RULES
            NO_VARIABLES=1
            ;;
        --no-object-rules|--no_object_rules)
            NO_OBJECT_RULES=1
            ;;
        --list-files)
            LIST_FILE=1
            ;;
        --mk-include|--mk_include)
            MK_INCLUDE=1
            ;;
        --mk-include-prg|--mk_include_prg)
            verif_option "${#}" "${1}"; shift
            MK_INCLUDE_PRG="${1}"
            ;;
        --version)
            version
            exit
            ;;
        --help)
            usage
            exit
            ;;
        --)
            stop_parse=1
            ;;
        -*)
            echo "`basename ${0}`: unknown option ${1}." >&2
            usage
            exit 1
            ;;
        *)
            break
            ;;
    esac
    shift
done

#---------------------------------------------------------
# ORB handling.
#---------------------------------------------------------
case "${ORB}" in
    ORBacus|ORBacus3|ORBacus4)
        jtkORB="jtkORBacus"
        CL_SUFFIX=""
        CL_SUFFIX_H="${CL_SUFFIX}"
        SKEL_SUFFIX="_skel"
        SKEL_SUFFIX_H="${SKEL_SUFFIX}"
        SKEL_TIE_SUFFIX_H="_skel_tie"
        if [ "x${H_SUFFIX_FLAG-}" = "x" ]; then
            H_SUFFIX='.h'
        fi
        if [ "x${C_SUFFIX_FLAG-}" = "x" ]; then
            C_SUFFIX='.cpp'
        fi
        ;;
    Visibroker)
        jtkORB="jtkVisibroker"
        if [ "x${CL_SUFFIX_FLAG-}" = "x" ]; then
            CL_SUFFIX='_c'
        fi
        CL_SUFFIX_H="${CL_SUFFIX}"
        if [ "x${SKEL_SUFFIX_FLAG-}" = "x" ]; then
            SKEL_SUFFIX='_s'
        fi
        SKEL_SUFFIX_H="${SKEL_SUFFIX}"
        if [ "x${H_SUFFIX_FLAG-}" = "x" ]; then
            H_SUFFIX='.hh'
        fi
        if [ "x${C_SUFFIX_FLAG-}" = "x" ]; then
            C_SUFFIX='.cc'
        fi
        ;;
    Orbix)
        jtkORB="jtkOrbix"
        if [ "x${CL_SUFFIX_FLAG-}" = "x" ]; then
            CL_SUFFIX='C'
        fi
        CL_SUFFIX_H=
        if [ "x${SKEL_SUFFIX_FLAG-}" = "x" ]; then
            SKEL_SUFFIX='S'
        fi
        SKEL_SUFFIX_H=
        if [ "x${H_SUFFIX_FLAG-}" = "x" ]; then
            H_SUFFIX='.hh'
        fi
        if [ "x${C_SUFFIX_FLAG-}" = "x" ]; then
            C_SUFFIX='.C'
        fi
        ;;
    OmniORB2)
        jtkORB="jtkOmniORB2"
        if [ "x${CL_SUFFIX_FLAG-}" = "x" ]; then
            CL_SUFFIX=
        fi
        CL_SUFFIX_H=
        if [ "x${SKEL_SUFFIX_FLAG-}" = "x" ]; then
            SKEL_SUFFIX=
        fi
        SKEL_SUFFIX_H=
        if [ "x${OMNIORB_DYN-}" != x ]; then
            SKELDYN_SUFFIX='Dyn'
        fi
        if [ "x${H_SUFFIX_FLAG-}" = "x" ]; then
            H_SUFFIX='.hh'
        fi
        if [ "x${C_SUFFIX_FLAG-}" = "x" ]; then
            C_SUFFIX='SK.cc'
        fi
        ;;
    Mico)
        jtkORB="jtkMico"
        CL_SUFFIX=
        CL_SUFFIX_H=
        SKEL_SUFFIX=
        SKEL_SUFFIX_H=
        if [ "x${H_SUFFIX_FLAG-}" = "x" ]; then
            H_SUFFIX='.h'
        fi
        if [ "x${C_SUFFIX_FLAG-}" = "x" ]; then
            C_SUFFIX='.cc'
        fi
        # Support for the -c++-skel option of the idl compiler.
        if [ "x${MICO_SKEL-}" != "x" ]; then
            jtkORB="jtkMicoSkel"
            SKEL_SUFFIX='_skel'
        fi
        ;;
    TAO|TAOpenFusion|tao13|tao14-OF)
        jtkORB="jtkTAO"
        CL_SUFFIX="C"
        CL_SUFFIX_H="${CL_SUFFIX}"
        SKEL_SUFFIX="S"
        SKEL_SUFFIX_H="${SKEL_SUFFIX}"
        SKEL_TIE_SUFFIX_H="S_T"
        if [ "x${H_SUFFIX_FLAG-}" = "x" ]; then
            H_SUFFIX='.h'
        fi
        if [ "x${C_SUFFIX_FLAG-}" = "x" ]; then
            C_SUFFIX='.cpp'
        fi
        ;;
    *)
        echo "`basename ${0}`: Unsupported ORB." >&2
        exit 1             
        ;;             
esac

#---------------------------------------------------------
# Initialise some variables.
#---------------------------------------------------------
if [ "x${EXTRA_IDLFLAGS-}" != "x" ]; then
    IDL_COMPL="${IDL_COMPL} ${EXTRA_IDLFLAGS}"
fi

if [ "x${MK_INCLUDE-}" != x ]; then
    MK_INCLUDE_OPT="--orb ${ORB}"
    if [ "x${H_SUFFIX_FLAG-}" != "x" ]; then
        MK_INCLUDE_OPT="--h-suffix ${H_SUFFIX} ${MK_INCLUDE_OPT}"
    fi
    if [ "x${CL_SUFFIX_FLAG-}" != "x" ]; then
        MK_INCLUDE_OPT="--client-suffix ${CL_SUFFIX} ${MK_INCLUDE_OPT}"
    fi
    if [ "x${SKEL_SUFFIX_FLAG-}" != "x" ]; then
        MK_INCLUDE_OPT="--server-suffix ${SKEL_SUFFIX} ${MK_INCLUDE_OPT}"
    fi  
    if [ "x${NO_SERVER-}" != x ]; then
        MK_INCLUDE_OPT="${MK_INCLUDE_OPT} --client-only"
    elif [ "x${NO_CLIENT-}" != x ]; then
        MK_INCLUDE_OPT="${MK_INCLUDE_OPT} --server-only"
    fi
    if [ "x${TIE-}" != "x" ]; then
        MK_INCLUDE_OPT="${MK_INCLUDE_OPT} --tie"
    fi
fi 

#---------------------------------------------------------
# Files handling.
#---------------------------------------------------------
if [ "x${*-}" != "x" ]; then
    IDLS="${IDLS} ${*}"
fi
for i in ${IDL_DIR}; do
    file_in_dir="${i}/*.idl"
    TESTEMPTY=`echo ${file_in_dir} | sed '/\*/d'`
    if [ "x${TESTEMPTY-}" = "x" ]; then
        echo "`basename ${0}`: No idl files found in directory ${i}." >&2
    else
        IDLS="${IDLS} ${file_in_dir}"
    fi
done

if [ "x${IDLS-}" = "x" ]; then
    echo "`basename ${0}`: No idl files to process." >&2
    exit 1
fi

#---------------------------------------------------------
# List idl files
#---------------------------------------------------------
if [ "x${LIST_FILE-}" != "x" ]; then
    IDLFILE=
    for i in ${IDLS}; do
        IDLFILE="${IDLFILE} ${i}"
    done
    echo "${IDLFILE}"
    exit
fi

#---------------------------------------------------------
# Print Message.
#---------------------------------------------------------
print_header

#---------------------------------------------------------
# Initialise cumulative variables.
#---------------------------------------------------------
if [ "x${NO_VARIABLES-}" = "x" -a "x${NO_ALL_VARIABLES-}" = "x" ]; then
    if [ "x${OLD_MAKE-}" = "x" ]; then
        if [ "x${ADDITIVE_VARS-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_ALL_IDL" ""
            if [ "x${NO_CLIENT-}" = "x" ]; then
                print_eq "${VAR_PREFIX}_CL_ALL_SRC" ""
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                print_eq "${VAR_PREFIX}_SRV_ALL_SRC" ""
            fi

            if [ "x${NO_CLIENT-}" = "x" ]; then
                print_eq "${VAR_PREFIX}_CL_ALL_OBJS" ""
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                print_eq "${VAR_PREFIX}_SRV_ALL_OBJS" ""
            fi

            if [ "x${NO_CLIENT-}" = "x" ]; then
                print_eq "${VAR_PREFIX}_CL_ALL_HDRS" ""
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                print_eq "${VAR_PREFIX}_SRV_ALL_HDRS" ""
            fi
            echo
        fi
    else 
        if [ "x${ADDITIVE_VARS-}" = "x" ]; then
            corba_all_idl=
            if [ "x${NO_CLIENT-}" = "x" ]; then
                corba_cl_all_hdrs=
                corba_cl_all_src=
                corba_cl_all_objs=
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                corba_srv_all_hdrs=
                corba_srv_all_src=
                corba_srv_all_objs=
            fi
        else
            echo "`basename ${0}`: Options --additive-vars and \
--old-make are incompatible." >&2
            exit 1 
        fi
    fi
fi

#---------------------------------------------------------
# Loop on each idl file.
#---------------------------------------------------------
for i in ${IDLS}; do

    #---------------------------------------------------------
    # Initialise the file names.
    #---------------------------------------------------------
    j=`basename ${i} .idl`
    CL_H="${j}${CL_SUFFIX_H}${H_SUFFIX}"
    SKEL_H="${j}${SKEL_SUFFIX_H}${H_SUFFIX}"
    CL_CPP="${j}${CL_SUFFIX}${C_SUFFIX}"
    SKEL_CPP="${j}${SKEL_SUFFIX}${C_SUFFIX}"

    corba_idl="${i}"
    if [ "x${NO_CLIENT-}" = "x" ]; then
        corba_cl_hdrs="${CL_H}"
        corba_cl_src="${CL_CPP}"
        case "${jtkORB}" in
            jtkOmniORB2)
                if [ "x${OMNIORB_DYN-}" != x ]; then
                    SKELDYN_CPP="${j}${SKELDYN_SUFFIX}${C_SUFFIX}"
                    corba_cl_src="${corba_cl_src} ${SKELDYN_CPP}"
                fi
                ;;
        esac
        if [ "x${MK_INCLUDE-}" != "x" ]; then 
            corba_cl_hdrs_mk="${j}.stub.hpp"
            corba_cl_hdrs="${corba_cl_hdrs} ${corba_cl_hdrs_mk}"
        fi 
    fi
    if [ "x${NO_SERVER-}" = "x" ]; then
        case "${jtkORB}" in
            jtkORBacus|jtkTAO)
                corba_srv_hdrs="${CL_H} ${SKEL_H}"
                if [ "x${TIE-}" != "x" ]; then
                    SKEL_TIE_H="${j}${SKEL_TIE_SUFFIX_H}${H_SUFFIX}"
                    corba_srv_hdrs="${corba_srv_hdrs} ${SKEL_TIE_H}"
                fi
                corba_srv_src="${CL_CPP} ${SKEL_CPP}"
                ;;
            jtkVisibroker)
                corba_srv_hdrs="${CL_H} ${SKEL_H}"
                corba_srv_src="${CL_CPP} ${SKEL_CPP}"
                ;;
            jtkMicoSkel)
                corba_srv_hdrs="${SKEL_H}"
                corba_srv_src="${CL_CPP} ${SKEL_CPP}"
                ;;
            jtkOrbix|jtkOmniORB2|jtkMico)
                corba_srv_hdrs="${SKEL_H}"
                corba_srv_src="${SKEL_CPP}"
                if [ "x${OMNIORB_DYN-}" != "x" ]; then
                    SKELDYN_CPP="${j}${SKELDYN_SUFFIX}${C_SUFFIX}"
                    corba_srv_src="${corba_srv_src} ${SKELDYN_CPP}"
                fi
                ;;
            *)
                echo "`basename ${0}`: Unsupported ORB." >&2
                exit 1
                ;;
        esac
        if [ "x${MK_INCLUDE-}" != "x" ]; then 
            corba_srv_hdrs_mk="${j}.skel.hpp"
            if [ "x${TIE-}" != "x" ]; then
                corba_srv_hdrs_mk="${corba_srv_hdrs_mk} ${j}.skeltie.h"
            fi
            corba_srv_hdrs="${corba_srv_hdrs} ${corba_srv_hdrs_mk}"
        fi 
    fi
    if [ "x${ABSOLUTE-}" != "x" ]; then
        corba_idl=`absolute_name ${corba_idl}`
    fi
    if [ "x${NO_CLIENT-}" = "x" ]; then
        corba_cl_objs=`give_obj ${corba_cl_src}`
        if [ "x${H_DIR-}" != "x" ]; then
            corba_cl_hdrs=`add_dir ${H_DIR} ${corba_cl_hdrs}`
        fi
        if [ "x${CPP_DIR-}" != "x" ]; then
            corba_cl_src=`add_dir ${CPP_DIR} ${corba_cl_src}`
        fi
        if [ "x${OBJ_DIR-}" != "x" ]; then
            corba_cl_objs=`add_dir ${OBJ_DIR} ${corba_cl_objs}`
        fi
        if [ "x${ABSOLUTE-}" != "x" ]; then
            corba_cl_hdrs=`absolute_name ${corba_cl_hdrs}`
            corba_cl_src=`absolute_name ${corba_cl_src}`
            corba_cl_objs=`absolute_name ${corba_cl_objs}`
        fi
    fi
    if [ "x${NO_SERVER-}" = "x" ]; then
        corba_srv_objs=`give_obj ${corba_srv_src}`
        if [ "x${H_DIR-}" != "x" ]; then
            corba_srv_hdrs=`add_dir ${H_DIR} ${corba_srv_hdrs}`
        fi
        if [ "x${CPP_DIR-}" != "x" ]; then
            corba_srv_src=`add_dir ${CPP_DIR} ${corba_srv_src}`
        fi
        if [ "x${OBJ_DIR-}" != "x" ]; then
            corba_srv_objs=`add_dir ${OBJ_DIR} ${corba_srv_objs}`
        fi
        if [ "x${ABSOLUTE-}" != "x" ]; then
            corba_srv_hdrs=`absolute_name ${corba_srv_hdrs}`
            corba_srv_src=`absolute_name ${corba_srv_src}`
            corba_srv_objs=`absolute_name ${corba_srv_objs}`
        fi
    fi

    if [ "x${NO_RULES-}" = "x" ]; then
        case "${jtkORB}" in
            jtkORBacus|jtkVisibroker|jtkOmniORB2|jtkMico|jtkMicoSkel|jtkTAO)
                if [ "x${NO_SERVER-}" = "x" ]; then
                    corba_target_src="${corba_srv_src}"
                    corba_target_objs="${corba_srv_objs}"
                    corba_target_hdrs="${corba_srv_hdrs}"
                    if [ "x${NO_CLIENT-}" = "x" -a "x${MK_INCLUDE-}" != x ]; then 
                        corba_target_hdrs="${corba_target_hdrs} ${corba_cl_hdrs_mk}"
                    fi
                elif [ "x${NO_CLIENT-}" = "x" ]; then
                    corba_target_src="${corba_cl_src}"
                    corba_target_objs="${corba_cl_objs}"
                    corba_target_hdrs="${corba_cl_hdrs}"
                fi
                ;;
            jtkOrbix)
                corba_target_src=
                corba_target_objs=
                if [ "x${NO_SERVER-}" = "x" ]; then
                    corba_target_hdrs="${corba_srv_hdrs}"
                    corba_target_objs="${corba_target_objs} ${corba_srv_objs}"
                    corba_target_src="${corba_target_src} ${corba_srv_src}"
                fi
                if [ "x${NO_CLIENT-}" = "x" ]; then
                    corba_target_hdrs="${corba_cl_hdrs}"
                    corba_target_objs="${corba_target_objs} ${corba_cl_objs}"
                    corba_target_src="${corba_target_src} ${corba_cl_src}"
                fi
                corba_target_src=`echo ${corba_target_src} | sed 's/^[ ]*//'`
                corba_target_objs=`echo ${corba_target_objs} | sed 's/^[ ]*//'`
                ;;
            *)
                echo "`basename ${0}`: Unsupported ORB." >&2
                exit 1
                ;;
        esac
    fi

    #---------------------------------------------------------
    # Update cumulative variables.
    #---------------------------------------------------------
    if [ "x${NO_VARIABLES-}" = "x" -a "x${NO_ALL_VARIABLES-}" = "x" ]; then
        if [ "x${OLD_MAKE-}" != "x" ]; then
            sep=' \\\n\t'
            corba_all_idl="${corba_all_idl}${sep}${corba_idl}"
            if [ "x${NO_CLIENT-}" = "x" ]; then
                corba_cl_all_hdrs="${corba_cl_all_hdrs}${sep}${corba_cl_hdrs}"
                corba_cl_all_objs="${corba_cl_all_objs}${sep}${corba_cl_objs}"
                corba_cl_all_src="${corba_cl_all_src}${sep}${corba_cl_src}"
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                corba_srv_all_hdrs="${corba_srv_all_hdrs}${sep}${corba_srv_hdrs}"
                corba_srv_all_src="${corba_srv_all_src}${sep}${corba_srv_src}"
                corba_srv_all_objs="${corba_srv_all_objs}${sep}${corba_srv_objs}"
            fi
        else
            print_peq "${VAR_PREFIX}_ALL_IDL" "${corba_idl}"
            if [ "x${NO_CLIENT-}" = "x" ]; then
                print_peq "${VAR_PREFIX}_CL_ALL_SRC" "${corba_cl_src}"
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                print_peq "${VAR_PREFIX}_SRV_ALL_SRC" "${corba_srv_src}"
            fi
            if [ "x${NO_CLIENT-}" = "x" ]; then
                print_peq "${VAR_PREFIX}_CL_ALL_OBJS" "${corba_cl_objs}"
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                print_peq "${VAR_PREFIX}_SRV_ALL_OBJS" "${corba_srv_objs}"
            fi
            if [ "x${NO_CLIENT-}" = "x" ]; then
                print_peq "${VAR_PREFIX}_CL_ALL_HDRS" "${corba_cl_hdrs}"
            fi
            if [ "x${NO_SERVER-}" = "x" ]; then
                print_peq "${VAR_PREFIX}_SRV_ALL_HDRS" "${corba_srv_hdrs}"
            fi
            echo
        fi 
    fi

    #---------------------------------------------------------
    # Print variables for the current idl file.
    #---------------------------------------------------------
    if [ "x${NO_VARIABLES-}" = "x" ]; then
        IDLNAME=`echo ${j} | tr [a-z] [A-Z]`
        # specific variables (idl)
        print_eq "${VAR_PREFIX}_${IDLNAME}_IDL" "${corba_idl}"
        # specific variables (sources)
        if [ "x${NO_CLIENT-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_${IDLNAME}_CL_SRC" "${corba_cl_src}"
        fi
        if [ "x${NO_SERVER-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_${IDLNAME}_SRV_SRC" "${corba_srv_src}"
        fi
        # specific variables (objects)
        if [ "x${NO_CLIENT-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_${IDLNAME}_CL_OBJS" "${corba_cl_objs}"
        fi
        if [ "x${NO_SERVER-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_${IDLNAME}_SRV_OBJS" "${corba_srv_objs}"
        fi
        # specific variables (headers)
        if [ "x${NO_CLIENT-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_${IDLNAME}_CL_HDRS" "${corba_cl_hdrs}"
        fi
        if [ "x${NO_SERVER-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_${IDLNAME}_SRV_HDRS" "${corba_srv_hdrs}"
        fi
        echo
    fi

    if [ "x${NO_RULES-}" = x ]; then
        #---------------------------------------------------------
        # Print object generation rules for the current idl file.
        #---------------------------------------------------------
        if [ "x${NO_OBJECT_RULES-}" = "x" ]; then
            for i1 in ${corba_target_src}; do
                i2=`echo ${corba_target_objs} | cut -d' ' -f1`
                corba_target_objs=`echo ${corba_target_objs} | cut -d' ' -f2-`
                print_rule_depend ${i2} ${i1}
                print_rule_action "${CXX_COMPL} -c -o \$@ ${i1}"
            done
            echo
        fi

        #---------------------------------------------------------
        # Print idl generated rules for the current idl file.
        #---------------------------------------------------------
        print_rule_depend "${corba_target_src} ${corba_target_hdrs}" "${corba_idl}"
        print_rule_action "${IDL_COMPL} ${corba_idl}"
        if [ "x${MK_INCLUDE-}" != x ]; then 
            print_rule_action "${MK_INCLUDE_PRG} ${MK_INCLUDE_OPT} ${corba_idl}"
        fi    
        if [ "x${H_DIR-}" != "x" ]; then
            print_mv "${corba_target_hdrs}"
        fi
        if [ "x${CPP_DIR-}" != "x" ]; then
            print_mv "${corba_target_src}"
        fi
        echo
    fi

#---------------------------------------------------------
# End of loop
#---------------------------------------------------------
done

#---------------------------------------------------------
# Print cumulative variables for old Makefile.
#---------------------------------------------------------
if [ "x${NO_VARIABLES-}" = "x" -a "x${NO_ALL_VARIABLES-}" = "x" ]; then
    if [ "x${OLD_MAKE-}" != "x" ]; then
        print_eq "${VAR_PREFIX}_ALL_IDL" "${corba_all_idl}"
        if [ "x${NO_CLIENT-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_CL_ALL_SRC" "${corba_cl_all_src}"
        fi
        if [ "x${NO_SERVER-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_SRV_ALL_SRC" "${corba_srv_all_src}"
        fi
        if [ "x${NO_CLIENT-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_CL_ALL_OBJS" "${corba_cl_all_objs}"
        fi
        if [ "x${NO_SERVER-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_SRV_ALL_OBJS" "${corba_srv_all_objs}"
        fi
        if [ "x${NO_CLIENT-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_CL_ALL_HDRS" "${corba_cl_all_hdrs}"
        fi
        if [ "x${NO_SERVER-}" = "x" ]; then
            print_eq "${VAR_PREFIX}_SRV_ALL_HDRS" "${corba_srv_all_hdrs}"
        fi
    fi
fi

