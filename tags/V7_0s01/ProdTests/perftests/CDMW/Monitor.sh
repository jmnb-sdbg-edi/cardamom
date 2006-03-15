#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI. All rights reserved.
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

source `dirname $0`/Testparams.sh
BUTTON=".toggle"

# monitoring delay:
MONITOR_DELAY=1
RESDIR=.

function startmon(){    # RESDIR dlay

    test -n "$1" && RESDIR=$1
    test $2 -gt 0 2>/dev/null && MONITOR_DELAY=$2

    CPU=$RESDIR/$CPUmon
    RAM=$RESDIR/$RAMmon
    MONBUTTON=$RESDIR/$BUTTON
    echo "START" > $MONBUTTON

    rm -f $CPU $RAM
    MONITOR_DELAY=$(( $MONITOR_DELAY - 1 ))
    while [ "`< $MONBUTTON`" == "START" ]; do
        free -t -o |awk ' /^Total/ { print $3 ; fflush()}' >> $RAM
##  RH 7.2: vmstat -n 1 2| awk 'NR>3 { print (($14 + $15)) ; fflush()}' >> $CPU
##  RH ES3: vmstat -n 1 2| awk 'NR>3 { print (($13 + $14 + $16)) ; fflush()}' >> $CPU
        vmstat -n 1 2| awk 'NR>3 { print (($13 + $14 + $16)) ; fflush()}' >> $CPU
        sleep $MONITOR_DELAY 
    done
}


function killmon(){
    test -n "$1" && RESDIR=$1
    test $2 -gt 0 2>/dev/null && MONITOR_DELAY=$2
    MONBUTTON=$RESDIR/$BUTTON

    echo "STOP" >  $MONBUTTON
    sleep $MONITOR_DELAY
    rm $MONBUTTON
}

function Usage(){
        echo
        echo "Usage: $0 start|stop outdir delay"
        echo
}

##### MAIN ###########
npara=${#@}
if [ $npara -ne 3 ]; then
    Usage
    exit
fi


case $1 in
    stop)
        echo kill monitor
        killmon $2 $3
        ;;
    start)
        echo start monitor
        (startmon $2 $3) &
        ;;
    *)
        Usage
        ;;
esac
