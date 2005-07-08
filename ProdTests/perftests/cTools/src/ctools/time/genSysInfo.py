#######################################################################
# $Id: genSysInfo.py,v 1.1.1.1 2005-07-08 14:31:04 moghrabi Exp $
#######################################################################
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
#
# This script takes care of generating the SysInfo.h file. This file
# contains some information that are used by the Real-Time Java
# implementation. 
#
###

import os
import sys
import string

def genSysInfo():

    sysInfo = open("SysInfo.h", "w")
    cpuInfo = open("/proc/cpuinfo")
    
    line = cpuInfo.readline()
    cpuMHz = 0
    procNum = 0

    #
    # Assume SMP machine
    #
    while line:
        if (string.find(line, "cpu MHz") != -1):
            cpuMHz = string.split(line, ":")[1]
            procNum = procNum + 1
        line = cpuInfo.readline()

    #
    # Header Guard Generation
    #
    sysInfo.write("//////////////////////////////////////////////////////////////////////\n")
    sysInfo.write("// THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT MODIFY!!!\n//")
    sysInfo.write("//////////////////////////////////////////////////////////////////////\n\n\n")
    sysInfo.write("#ifndef JRATE_SYS_INFO_H_\n")
    sysInfo.write("#define JRATE_SYS_INFO_H_\n\n")

    sysInfo.write("#define CLOCK_FREQUENCY ")
    sysInfo.write(cpuMHz)

    clockPeriodNS = 1000/float(cpuMHz)
    sysInfo.write("#define CLOCK_PERIOD_NS ")
    sysInfo.write(str(clockPeriodNS))
    sysInfo.write("\n")
    
    clockPeriodUS = 1/float(cpuMHz)
    sysInfo.write("#define CLOCK_PERIOD_US ")
    sysInfo.write(str(clockPeriodUS))
    sysInfo.write("\n")
    
    sysInfo.write("#define PROC_NUM ")
    sysInfo.write(str(procNum))
    sysInfo.write("\n")
    
    threadMax = open("/proc/sys/kernel/threads-max");
    sysInfo.write("\n#define THREAD_MAX ")
    sysInfo.write(threadMax.readline())
    sysInfo.write("\n")

    #
    # This is system dependent, and right now this script has to be
    # edited for platforms whose page size is not 4096 bytes
    #
    sysInfo.write("#define JRATE_PAGE_SIZE 4096\n")
    
    sysInfo.write("\n#define POW_3_TEN 1000\n")
    sysInfo.write("#define POW_6_TEN 1000000\n")
    sysInfo.write("#define POW_9_TEN 1000000000\n")

    sysInfo.write("\n#define TIMER_SLACK_NS 1436\n")
    #
    # Trailer Guard Generation
    #
    sysInfo.write("\n#endif // JRATE_SYS_INFO_H_\n")

if __name__ == '__main__':
    genSysInfo()
