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
# mk_idl_include:
# Tool for generating ORB independent include files.
# (C) Arnaud Desitter - Nag Ltd. - arnaud@nag.co.uk, 1999
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
#      This product includes software developed by Arnaud Desitter, 
#      Oxford, UK and his contributors.
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
ORB='ORBacus'

#---------------------------------------------------------
# Print version and usage.
#---------------------------------------------------------
version() {
cat >&2 << __EOF__
`basename ${0}`: IDL include file generator.
(C) Arnaud Desitter - Nag Ltd. <arnaud@nag.co.uk>, 1999.
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
--generated-files      Print the names of the generated files 
--h-dir DIR            Write generated header files to directory DIR.
--h-suffix SUFFIX      Use SUFFIX as suffix for header files.
--client-suffix SUFFIX Use SUFFIX as suffix for client files.
--server-suffix SUFFIX Use SUFFIX as suffix for server files.
                       The suffixes default are ORB dependent.
--client-only          Generate only client dependencies.
--server-only          Generate only server dependencies.
                       The default is to generate both.
--header-dir DIR       Append DIR within the include statement.
--tie                  Support for ORBacus --tie option.
--orb ORB              Generate ORB compliant file names.
                       The default is \`${ORB}'.

This tool generates stub and skeleton headers files associated with
a specific ORB.
Two files name are created:
 - <idl_name>.stub.hpp
 - <idl_name>.skel.hpp
Additionally, <idl_name.skel_tie.h> is generated if --tie is selected.
__EOF__
}

#---------------------------------------------------------
# Useful functions.
#---------------------------------------------------------
print_header_cc() {
cat << __EOF__
/*
  File generated by `basename ${0}`. DO NOT EDIT
  Created on: `date`.
  ORB: ${ORB}
*/

__EOF__
}

add_dir() {
    DIR=${1}; shift
    ret=
    for i do
        ret="${ret} ${DIR}/${i}"
    done
    echo ${ret}
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
generated=0

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
        --idl-file|--idl_file)
            verif_option "${#}" "${1}"; shift
            IDLS="${IDLS} ${1}"
            ;;
        --h-dir|--h_dir)
            verif_option "${#}" "${1}"; shift
            H_DIR="${1}"
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
        --header-dir|--header_dir)
            verif_option "${#}" "${1}"; shift
            HEADER_DIR_FLAG=1
            HEADER_DIR="${1}"
            ;;
        --tie)
            TIE=1
            ;;
        --server-only|--server_only)
            unset NO_SERVER
            NO_CLIENT=1
            ;;
        --client-only|--client_only)
            unset NO_CLIENT
            NO_SERVER=1
            ;;
        --list-files)
            LIST_FILE=1
            ;;
        --version)
            version
            exit
            ;;
        --help)
            usage
            exit
            ;;
        --generated-files)
            generated=1
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
# Arnaud Samama 29/09/2000
# Print the files generated by the ORB
#---------------------------------------------------------
if [ "${generated}" = "1" ]; then
    for i in ${IDLS}; do  
        j=`basename ${i} .idl`

        # Header client
        echo "${j}${CL_SUFFIX_H}${H_SUFFIX}"
        # Header server
        echo "${j}${SKEL_SUFFIX_H}${H_SUFFIX}"

        # Body client
        echo "${j}${CL_SUFFIX}${C_SUFFIX}"
        # Body server
        echo "${j}${SKEL_SUFFIX}${C_SUFFIX}"
    done
    exit
fi

#---------------------------------------------------------
# Loop on each idl file.
#---------------------------------------------------------
for i in ${IDLS}; do

    #---------------------------------------------------------
    # Initialise the file names.
    #---------------------------------------------------------
    j=`basename ${i} .idl`
    NAMEU=`echo ${j} | tr [a-z] [A-Z]`
    CL_H="${j}${CL_SUFFIX_H}${H_SUFFIX}"
    SKEL_H="${j}${SKEL_SUFFIX_H}${H_SUFFIX}"

    corba_idl="${i}"
    # Stub
    if [ "x${NO_CLIENT-}" = "x" ]; then
        NAME="${j}.stub.hpp"
        NAME_H="${NAMEU}_STUB_H"
        INC_H=${CL_H}
        if [ "x${HEADER_DIR_FLAG-}" != x ]; then
            INC_H=`add_dir ${HEADER_DIR} ${INC_H}`
        fi 
        if [ "x${H_DIR-}" != "x" ]; then
            NAME=`add_dir ${H_DIR} ${NAME}`
        fi
        : > ${NAME}
        print_header_cc >> ${NAME}
cat >> ${NAME} << __EOF__
#ifndef ${NAME_H}
#define ${NAME_H}
#include <${INC_H}>
#endif /* ${NAME_H} */
__EOF__

    fi

    # Skeleton
    if [ "x${NO_SERVER-}" = "x" ]; then
        NAME="${j}.skel.hpp"
        NAME_H="${NAMEU}_SKEL_H"
        INC_H=${SKEL_H}
        if [ "x${HEADER_DIR_FLAG-}" != x ]; then
            INC_H=`add_dir ${HEADER_DIR} ${INC_H}`
        fi 
        if [ "x${H_DIR-}" != "x" ]; then
            NAME=`add_dir ${H_DIR} ${NAME}`
        fi
        : > ${NAME}
        print_header_cc >> ${NAME}
cat >> ${NAME} << __EOF__
#ifndef ${NAME_H}
#define ${NAME_H}
#include <${INC_H}>
#endif /* ${NAME_H} */
__EOF__

        if [ "x${TIE-}" != "x" ]; then
            case "${jtkORB}" in
                jtkORBacus|jtkTAO)
                    SKEL_TIE_H="${j}${SKEL_TIE_SUFFIX_H}${H_SUFFIX}"
                    ;;
                *)
                    SKEL_TIE_H="${SKEL_H}"
                    ;;
            esac
            NAME="${j}.skeltie.new"
            NAME_H="${NAMEU}_SKELTIE_H"
            INC_H=${SKEL_TIE_H}
            if [ "x${HEADER_DIR_FLAG-}" != x ]; then
                INC_H=`add_dir ${HEADER_DIR} ${INC_H}`
            fi 
            if [ "x${H_DIR-}" != "x" ]; then
                NAME=`add_dir ${H_DIR} ${NAME}`
            fi
            : > ${NAME}
            print_header_cc >> ${NAME}
cat >> ${NAME} << __EOF__
#ifndef ${NAME_H}
#define ${NAME_H}
#include <${INC_H}>
#endif /* ${NAME_H} */
__EOF__
        fi
    fi

#---------------------------------------------------------
# End of loop
#---------------------------------------------------------
done


