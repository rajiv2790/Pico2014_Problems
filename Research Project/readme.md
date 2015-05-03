# Install Instructions

Download Intel pin from this url.
http://software.intel.com/sites/landingpage/pintool/downloads/pin-2.14-71313-gcc.4.4.7-linux.tar.gz

Extract the pin as follows
`tar xvzf pin-2.14-71313-gcc.4.4.7-linux.tar.gz`

From here, the directory into which the source is extracted is referred to as <pin>.

Copy the source file `taint_tracker.cpp into `<pin>/source/tools/ManualExamples`.

Before compiling, make sure to run this command as root: `echo 0 > /proc/sys/kernel/yama/ptrace_scope` to enable pin to access dynamic binary instrumentation.

Export the path `<pin>` to the `$PATH` variable using `export PATH=$PATH:<pin>`

Compile the pintool using `make dir obj-intel64/taint_tracker.so`

Copy the test programs to desired location `<tests>`

Run the pintool against the test cases using `pin -t obj-intel64/taint_tracker.so -- <tests>/overflow2`
