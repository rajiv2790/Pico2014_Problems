# Install Instructions

Download Intel pin from this url. <br>
http://software.intel.com/sites/landingpage/pintool/downloads/pin-2.14-71313-gcc.4.4.7-linux.tar.gz

Extract the pin as follows <br>
`tar xvzf pin-2.14-71313-gcc.4.4.7-linux.tar.gz`

From here, the directory into which the source is extracted is referred to as `<pin>`.

Copy the source file `taint_tracker.cpp` into <br>
`<pin>/source/tools/ManualExamples`.

Before compiling, make sure to run this command as root: <br>
`echo 0 > /proc/sys/kernel/yama/ptrace_scope` <br>
to enable pin to access dynamic binary instrumentation.

Export the path `<pin>` to the `$PATH` variable using <br>
`export PATH=$PATH:<pin>`

The intel pin binary is a 32 bit binary. Hence, we have to install gcc-multilib and g++-multilib.
````
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install gcc-multilib
sudo apt-get install g++-multilib
````

Compile the pintool using <br>
`make dir obj-intel64/taint_tracker.so`

Copy the test programs to desired location `<tests>` <br>
Compile the test programs as follows: <br>
`gcc -o test1 test1.c`
`gcc -o overflow2 overflow2.c`

Run the pintool against the test cases using <br>
`pin -t obj-intel64/taint_tracker.so -- <tests>/overflow2`
