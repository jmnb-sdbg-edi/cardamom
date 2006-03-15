
#
#
#=========================================================================

Welcome to the CDMW DEMO
=========================

The purpose of CDMW DEMO is to provide an comprehensive example for writing
software based on CDMW runtime middleware. Provided Makefiles may be reused
to write more sofisticated ones that suit needs of CDMW users.

This demo is a Fault Tolerant application. Executables are run on two hosts. 
This is mandatory to create an object group reference used by CORBA FT (Fault Tolerance).

To build CDMW demo ftclock:
   export HOSTNAME2=<remote_host_name>
	make all

Running the demo:
	cd run
        ./start_demo_clock.sh
	in order to start demo related to FTClocks

	./start_demo_fttimers.sh
	in order to start demo related to FTTimers


Follow the steps indicated in main windows in order to start CDMW processes
(FTManager, Supervion) and to define and to start the system definited in the
Scenario.

On client term will be displayed the action to do in order to start the Client
Process.

After client activation, follow the steps indicated on main windows in order
to simulate the crash of Primary.

After fault, the client receives info from new Primary, and on client 
term "Current Time" continue to be displayed.


CONTENT:
=======
Makefile      : Global makefile
README.txt    : This file
config.mk     : CDMW configuration file for use by demo makefiles
tao13.mk      : TAO 1.3.x configuration file
site.mk       : Configuration file used by demo makefiles
data/         : Contains XML data files for starting demo programs
idl/          : Contains 'user-provided' IDL files
include/      : Contains 'user-provided' header files
run/          : Contains scripts for starting demo programs
src/          : Contains 'user-provided' source files
xml/          : Contains 'user-provided' XML data files for CDMW codegenration


The following directories are created by the global makefile
generated/    : Contains CDMW generated files
<host-type>/  : Build directory


