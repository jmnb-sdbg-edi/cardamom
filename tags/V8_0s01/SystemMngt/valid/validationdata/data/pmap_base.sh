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

declare -a DONE_LIBS

unset DONE_LIBS

declare -i VERBOSE=false

declare -i TOTAL_TEXT_KBS=0
declare -i TOTAL_DATA_KBS=0
declare -i TOTAL_ANON_KBS=0

FIRST_TIME_SEEN_RESULT=false

function first_time_seen ()
{
   if [ `echo ${DONE_LIBS[*]} |grep -c $1` -ne 0 ]
   then
#      echo "Seen library $1 before..."
      FIRST_TIME_SEEN_RESULT=false
   else
#      echo "New library $1 stored"
      DONE_LIBS[${#DONE_LIBS[*]}]=$1
      FIRST_TIME_SEEN_RESULT=true
   fi

   return 
}

function process_pid ()
{
   declare -a TEXT_LIBS=(`pmap $1 |grep r-xp |grep -o /.*$`)
   declare -a TEXT_SIZES=(`pmap $1 |grep r-xp |grep -o [0-9]*[\ ]KB\) |grep -o ^[0-9]*`)

   declare -i TEMP_TOTAL=$TOTAL_TEXT_KBS

   for (( I=0; I < ${#TEXT_LIBS[*]}; I++ )) 
   do
      first_time_seen ${TEXT_LIBS[$I]}
      if [ $FIRST_TIME_SEEN_RESULT == true ]
      then
#         if [ $VERBOSE == true ]
#         then
            echo "Recording text size of ${TEXT_SIZES[$I]} for lib ${TEXT_LIBS[$I]}"
#         fi
         TOTAL_TEXT_KBS=$TOTAL_TEXT_KBS+${TEXT_SIZES[$I]}
      else
         if [ $VERBOSE == true ]
         then
            echo "Disregarding duplicate lib  ${TEXT_LIBS[$I]}"
         fi
      fi
   done

   TEMP_TOTAL=$TOTAL_TEXT_KBS-$TEMP_TOTAL

   echo "   New text mapped for this process : $TEMP_TOTAL"

   declare -a DATA_SIZES=(`pmap $1 |grep -v r-xp |grep / |grep -o [0-9]*[\ ]KB\) |grep -o ^[0-9]*`)

   TEMP_TOTAL=$TOTAL_DATA_KBS

   for (( I=0; I < ${#DATA_SIZES[*]}; I++ ))
   do
      TOTAL_DATA_KBS=$TOTAL_DATA_KBS+${DATA_SIZES[$I]}
   done

   TEMP_TOTAL=$TOTAL_DATA_KBS-$TEMP_TOTAL

   echo "   New data mapped for this process : $TEMP_TOTAL"

   declare -a ANON_SIZES=(`pmap $1 |grep -v r-xp |grep -v / |grep -o [0-9]*[\ ]KB\) |grep -o ^[0-9]*`)

   TEMP_TOTAL=$TOTAL_ANON_KBS

   for (( I=0; I < ${#ANON_SIZES[*]}; I++ ))
   do
      TOTAL_ANON_KBS=$TOTAL_ANON_KBS+${ANON_SIZES[$I]}
   done

   TEMP_TOTAL=$TOTAL_ANON_KBS-$TEMP_TOTAL

   echo "   New anon mapped for this process : $TEMP_TOTAL"
}





declare -a PROCESS_PIDS=(`ps -e | cut -c0-5`)

echo
echo "There are : ${#PROCESS_PIDS[*]} processes running."

for (( J=0; J < ${#PROCESS_PIDS[*]}; J++)) 
do
   echo
   echo "Processing with PID ${PROCESS_PIDS[$J]}"
   process_pid ${PROCESS_PIDS[$J]}
done

echo "libraries already into the memory"
echo ${DONE_LIBS[*]}
echo ${DONE_LIBS[*]} > lib_already_in_charge.txt


echo
echo "Total text memory used: $TOTAL_TEXT_KBS KB"
echo "Total data memory used: $TOTAL_DATA_KBS KB"
echo "Total anonymous memory: $TOTAL_ANON_KBS KB"
echo

declare -i TOTAL=$TOTAL_TEXT_KBS
TOTAL=$TOTAL+$TOTAL_DATA_KBS
TOTAL=$TOTAL+$TOTAL_ANON_KBS

echo "Total memory in use   : $TOTAL KB"
