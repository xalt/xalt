Testing XALT to produce \*.json output
--------------------------------------

The generation of \*.json files part of XALT can be tested in a user
account without any making any changes to the system level.  These
techniques can be use when trying to debug how XALT is working (or not
working).  Setting the environment variable **XALT_TRACING** will give
verbose output.


**DO NOT USE THIS INSTALLED VERSION OF XALT** for your system. The
**Config/rtm_config.py** file is designed for testing but not
production. It allows too many executables to be tracked.

Step 1: Configure and build XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Lets assume that **~/xalt** is your XALT source directory and you want to
install it under the **~/pkg/**.  Then you can do the following::

   $ cd ~/xalt;
   $ unset XALT_EXECUTABLE_TRACKING
   $ ./configure --prefix=$HOME/pkg --with-syshostConfig=hardcode:local --with-transmission=file --with-MySQL=no --with-config=Config/rtm_config.py
   $ make install
   $ export PATH=$HOME/pkg/xalt/xalt/bin:$PATH
   $ export COMPILER_PATH=$HOME/pkg/xalt/xalt/bin
   

Step 2: Set XALT_EXECUTABLE_TRACKING to yes.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

XALT will not track anything unless this environment variable is set to
**yes**::

   $ export XALT_EXECUTABLE_TRACKING=yes
   
Step 3: $HOME and $USER must have values
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

XALT will not produce anything \*.json without $HOME and $USER having
a value. This might be true when run under a container.

Step 4: Test xalt_configuration_report
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You should run **xalt_configuration_report** to see how xalt is
currently configured.  Make sure that the transmission style is
**file** and that you are happy with the path patterns::

   $ xalt_configuration_report

Step 5: Test linking a program
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create a **try.c** that has::

   #include <stdio.h>
   int main()
   { printf("Hello World!\n"); return 0;}

Then compile and link this with::

   $ XALT_TRACING=link gcc -o try try.c

You should see something like the following::

   16:37:15 XALT Tracing Activated for version: 2.6.11

   16:37:15 Starting ld

   16:37:15 find_real_command: Searching for the real: ld

   16:37:15 find_real_command: found /opt/apps/gcc/6.3.0/bin/ld

   16:37:15 COMP_T: {"compiler":"gcc","compilerPath":"/opt/apps/gcc/6.3.0/bin/gcc","link_line":["gcc","-o","try","try.c"]}

   16:37:15 Using ld to find functions:  /opt/apps/gcc/6.3.0/bin/ld -plugin /opt/apps/gcc/6.3.0/libexec/gcc/x86_64-pc-linux-gnu/6.3.0/liblto_plugin.so -plugin-opt=/opt/apps/gcc/6.3.0/libexec/gcc/x86_64-pc-linux-gnu/6.3.0/lto-wrapper -plugin-opt=-fresolution=/tmp/ccF6X3sh.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s --eh-frame-hdr -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o try /lib/../lib64/crt1.o /lib/../lib64/crti.o /opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/crtbegin.o -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0 -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/opt/intel/compilers_and_libraries_2018.2.199/linux/daal/../tbb/lib/intel64_lin/gcc4.4 -L/opt/intel/compilers_and_libraries_2018.2.199/linux/daal/lib/intel64_lin -L/opt/intel/compilers_and_libraries_2018.2.199/linux/tbb/lib/intel64/gcc4.7 -L/opt/intel/compilers_and_libraries_2018.2.199/linux/mkl/lib/intel64_lin -L/opt/intel/compilers_and_libraries_2018.2.199/linux/compiler/lib/intel64_lin -L/opt/intel/compilers_and_libraries_2018.2.199/linux/ipp/lib/intel64 -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/../../../../x86_64-pc-linux-gnu/lib -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/../../.. /tmp/ccMlm4OD.o -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/crtend.o /lib/../lib64/crtn.o --unresolved-symbols=report-all -o /dev/null

   16:37:15 Not adding XALT initialize routines to user code

   16:37:15 /opt/apps/gcc/6.3.0/bin/ld -plugin /opt/apps/gcc/6.3.0/libexec/gcc/x86_64-pc-linux-gnu/6.3.0/liblto_plugin.so -plugin-opt=/opt/apps/gcc/6.3.0/libexec/gcc/x86_64-pc-linux-gnu/6.3.0/lto-wrapper -plugin-opt=-fresolution=/tmp/ccF6X3sh.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s --eh-frame-hdr -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o try /lib/../lib64/crt1.o /lib/../lib64/crti.o /opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/crtbegin.o -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0 -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/opt/intel/compilers_and_libraries_2018.2.199/linux/daal/../tbb/lib/intel64_lin/gcc4.4 -L/opt/intel/compilers_and_libraries_2018.2.199/linux/daal/lib/intel64_lin -L/opt/intel/compilers_and_libraries_2018.2.199/linux/tbb/lib/intel64/gcc4.7 -L/opt/intel/compilers_and_libraries_2018.2.199/linux/mkl/lib/intel64_lin -L/opt/intel/compilers_and_libraries_2018.2.199/linux/compiler/lib/intel64_lin -L/opt/intel/compilers_and_libraries_2018.2.199/linux/ipp/lib/intel64 -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/../../../../x86_64-pc-linux-gnu/lib -L/opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/../../.. /tmp/ccMlm4OD.o -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /opt/apps/gcc/6.3.0/lib/gcc/x86_64-pc-linux-gnu/6.3.0/crtend.o /lib/../lib64/crtn.o /tmp/mclay_08c8ba01-689e-456e-b6e9-e618c594c327_u0yMaa/xalt.o    -t > /tmp/mclay_08c8ba01-689e-456e-b6e9-e618c594c327_u0yMaa/link.txt  2>&1

   16:37:15 XALT_TRANSMISSION_STYLE file
     Wrote json link file : /home/user/.xalt.d/link.stampede2.2019_05_08_16_37_15_3155.08c8ba01-689e-456e-b6e9-e618c594c327.json


You should also try running the link without XALT tracing::

   $ gcc -o try try.c

And check that there is a new \*.json file in ~/.xalt.d


Step 6: run a program with XALT tracing turned on 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Normally XALT will produce no output but for testing you can run a program under XALT by doing the following::

    $ export LD_PRELOAD=$HOME/pkg/xalt/xalt/lib64/libxalt_init.so
    $ XALT_TRACING=yes XALT_SAMPLING=no ./try

You should see something that looks like::

    myinit(LD_PRELOAD,/home/user/t/xalt/try){
      Test for __XALT_INITIAL_STATE__: "LD_PRELOAD", STATE: "LD_PRELOAD"
      Test for XALT_EXECUTABLE_TRACKING: yes
      Test for rank == 0, rank: 0
        -> XALT is build to track all programs, Current program is a scalar program -> Not producing a start record
    }

    Hello World!

    myfini(LD_PRELOAD){
      Recording State at end of scalar user program:
        LD_LIBRARY_PATH= PATH=/usr/bin:/bin /home/user/l/pkg/xalt/xalt//libexec/xalt_run_submission --interfaceV 4 --pid 10 --ppid 1--syshost "..." --start "..." --end "..." --exec "..." --ntasks 1 --kind "scalar" --uuid "..." --prob 1 --ngpus 0 --watermark "..." --path "..." --ld_libpath "..." -- ["./try"]
    }


    xalt_run_submission(zzz) {
      Built envT
      Extracted recordT from executable
      Built userT, userDT, scheduler: SLURM
      Filter envT
      Parsed ProcMaps
      Using XALT_TRANSMISSION_STYLE: file
      cmdlineA: ["./try"]
      Built json string
      Wrote json run file : /home/user/.xalt.d/run.stampede2.2019_05_08_16_39_12_3657.zzz.cbaec70f-d9e8-41e1-a76b-4db0729ddb06.json
    }   

Also please try running the program again to see if there is a new
\*.json file generated in the ~/.xalt.d directory::

    $ ./try
