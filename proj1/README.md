
# Project 1

### DUE: Monday, 4/4/2016 at 8:59pm

In this project, we learn about basic LLVM usage, from installing LLVM to writing a simple pass. The recommended environment for your project is Ubuntu 12.04. 

### What is LLVM & Clang?

You can think of LLVM as a (open-source) compiler, similar to GCC. Today, LLVM is gaining more and more popularity. It is widely used in industries and academies. The major differences from GCC are (in my opinion), highly modular design that comes from the power of C++ language (GCC is written in C), and the BSD-style license. The modular design enables developers to easily use LLVM’s components as a library to extend LLVM’s functionality or to use LLVM for other projects. The BSD-style license allows developers to freely use LLVM’s code without responsibility for disclosing the modified code.

Clang is a C-family frontend for LLVM. It takes the source code written in C/C++/Objective-C and translate it into LLVM IR.

### Install LLVM & Clang

First, we will install LLVM & Clang. Since we are going to modify LLVM in our project, we will download the source code. LLVM project is based on SVN version control system, but we will use Git. Execute the following commands.

```{.bash}
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install GCC g++ gv graphviz subversion git
git clone -b release_34 https://github.com/llvm-mirror/llvm.git
cd llvm/tools
git clone -b release_34 https://github.com/llvm-mirror/clang.git
cd ../..
mkdir build
cd build
../llvm/configure --disable-optimized --enable-targets=host
make -j8 clang-only BUILD_CLANG_ONLY=YES
```

`--disable-optimized` is used for debug build instead of release build. Debug macros, debug symbols, assertions and other facilities helpful for debugging will be included. As the result, binary size will be very big (around 10GB). We need debug build because we will modify and debug LLVM. 

`clang-only` and `BUILD_CLANG_ONLY=YES` options are used to avoid building a bunch of LLVM tools including `llc`, `opt`, etc. Most of the use of LLVM functionality can be done with `clang` command so we can use those options to save compilation time.

cf)
`-j<n>` option determines how many threads will be used for compilation. It may be changed depending on the number of cores in CPU.

Recommended RAM memory is 3GB or more.

If the build is successful, the binaries will be created in `build/Debug+Asserts/bin`. For easy access to the `clang` binary, you should add this folder to the `PATH` environment variable. You can add it to the `.profile` or its equivalent to set it up automatically every time you connect to the shell. Instead, you can create a file that can be fed to the `source` bash command and only `source` it when it’s needed.

### Using `clang`
<!-- LLVM 기본 사용법 -Ox, -emit-llvm, -c, -S, -o -->

After installing LLVM & clang, you can use `clang` command to compile a source file. `-Ox, -o, -c, -S` options can be used in the same manner as in the gcc. `-emit-llvm` option can be used to generate LLVM IR instead of machine instructions. Following are the example uses of `-emit-llvm` option.

```
clang -emit-llvm -c test.c     // generate LLVM IR bitcode (binary)
clang -emit-llvm -S test.c    // generate human readable LLVM IR
```
`.bc` file extension is used for LLVM IR bitcode and `.ll` is used for human readable LLVM IR.

<!-- clang driver에 대한 설명 --> 
`clang` is not a binary just for Clang. As explained above, Clang is a C-language family frontend for LLVM. On the other hand, `clang` command not only can be used to generate LLVM IR out of C, but also can be used for whole compilation process. In this context, `clang` is actually a compiler driver. It is called the Clang Driver. The clang Driver’s role is to examine the type of input file (source file, assembly file, object file, IR file, …) and compile flags (`-c`, `-S`, `-emit-llvm`) to decide how the input file should be processed. It then forks suitable program with suitable arguments. For example, `clang test.c` first forks off `clang -cc1` to compile `test.c` into an object file and then forks off `/bin/ld` to link the object file with other object files and libraries to create an executable file. Note that GNU linker (`/bin/ld`) is the default linker for LLVM 3.4. `-v` option can be used to see what program is forked by the Clang Driver.

### What is an LLVM Pass?

Compilers first translate the source code into an IR, optimize IR, and generate code from IR. Optimizations and code generation in LLVM (and probably in every other compilers) are done by applying multiple passes on the IR and the machine code. (The machine code is different from the IR in that its instructions correspond to a machine instruction.) Each pass in LLVM either apply a specific optimization to the IR (or the machine code) and thus transform it (Transform Pass) or provide analysis for the transform pass (Analysis Pass). Each pass only care about its job and rarely consider its impact on other passes. This greatly eases the development of each pass---It would be impossible to create a single pass that does all the analyses and transformations.

`-mllvm -debug-pass=<Arguments|Structure|Executions|Details>` option can be used to print information about the passes that are run. `-mllvm` option must be prepended because `-debug-pass` option is not available in the Clang Driver. `-mllvm` option just passes the next argument to LLVM. `-debug-pass=Structure` can be used to see what passes are run.  `-debug-pass=Execution` additionally prints out when the pass is executed. It is very helpful in debugging LLVM with gdb.

`-print-after-all` and `-print-before-all` options can be used to print LLVM IR after/before each pass.

A LLVM pass can be classified according to what unit of IR it operates on. A module pass operates on a module which represents a translation unit (containing all the functions and global variables, etc. in a file being compiled). A function pass operates on each function in the translation unit. Refer to [this page](http://llvm.org/docs/WritingAnLLVMPass.html#introduction-what-is-a-pass) for more info on LLVM Passes.


### How to Debug an LLVM Pass

Before learning how to write an LLVM pass, we will see how to debug an LLVM pass. This part will tell you where to look at when debugging an LLVM pass. Although you are free to use any debugging tool you are familiar with, we will explain here using emacs’s gdb plugin. 

First, install and execute emacs
```{.bash}
$ sudo apt-get install emacs
$ cd (LLVM_OBJ_DIR)
$ emacs -nw
```
cf) LLVM_OBJ_DIR = build directory.

cf2) LLVM_SRC_DIR = llvm directory.

`-nw` option can be used to avoid GUI emacs. Since GUI involves a lot of traffic, sometimes you will prefer to use this option. You can omit it if the connection to the server is fast enough.

Inside emacs, do the following.
```
M-x gdb               cf) M-x = alt+x, C-x = Ctrl+x
Run gdb (like this): gdb --annotate=3 clang
```

This will open `clang` binary file and load debug symbols to get ready for debugging session. We will now set a breakpoint at the entry point of a pass.

```
C-x 3                    // split the current window and open new window on the right half
C-x C-f                 // open a file
Find file: (LLVM_SRC_DIR)/lib/IR/LegacyPassManager.cpp
M-g M-g 1538      // goto line 1538
C-x C-a C-b         // set breakpoint at the current line
```

cf) LegacyPassManager is the code for managing passes. It is called **Legacy** because LLVM developers are building a brand new pass manager. All the backends must eventually be migrated to the new pass manager but this is not something that can be done easily so the legacy pass manager will stay in the code base for a while (if not forever).

Now we will actually run the program. Move to the gdb window and run the program. Type `pwd` to check the present working directory and make sure you are at the right location for specifying the input source code. (You can use `cd` to change directory)

```
C-x o                    // move to the next window (gdb window)
(gdb) run -mllvm -debug-pass=Executions test.c -v
```

`gdb` won’t stop at the breakpoint. Why? Recall that `clang` is actually the Clang driver. The actual compilation is done by `clang -cc1`. `gdb` just followed the parent thread, so the breakpoint in the child thread was ignored.

There can be two ways to solve this problem. First is to use `set follow-fork-mode child` to make gdb track the child process upon fork. Another way, which I prefer, is to copy all the argument from the output of `-v` option. From the output of `-v` option, find and copy all the arguments from `clang -cc1` invocation (from `-cc1` to end of line). Paste it after the `run` command as follows.

```
gdb) run <copied arguments>
```

Then, it will stop at the breakpoint. This is the main entry for the function passes. Since we used `-debug-pass=Executions` option, we can see what pass is about to run. You can see what this pass does by using various gdb commands. We haven’t looked at the major classes in LLVM yet, but for now just know that the major LLVM classes provide class methods for printing out useful information. For example, at the breakpoint we just set, we can enter `call F.dump()` to see how the function F looks like before running the pass.

In the appendix at the end of this document, the major emacs commands and emacs gdb plugin shortcuts are described for non-emacs users.

### Make A Test Pass

Here, we are going to write a new pass. This section is based on [this page](http://llvm.org/docs/WritingAnLLVMPass.html#basic-code-required), but slightly different since we are going to insert the pass into the regular optimization sequence.

1. Download “TestPass.cpp” from ETL.
1. Move it to (LLVM_SRC_DIR)/lib/Analysis.
1. Add following to ~/.profile
```{.bash}
export PATH=”$PATH:(LLVM_OBJ_DIR)/Debug+Asserts/bin”
```
1. terminal$ . ~/.profile
1. Add “(void) llvm::createTestPass();” to (LLVM_SRC_DIR)/include/llvm/LinkAllPasses.h
1. Add “FunctionPass *createTestPass();” to (LLVM_SRC_DIR)/include/llvm/Analysis/Passes.h
1. Add “void initializeTestPassPass(PassRegistry&);” to (LLVM_SRC_DIR)/include/llvm/InitializePasses.h
1. terminal$ cd (LLVM_OBJ_DIR)
1. Make arbitrary test.c of which CFG will be analyzed by your pass.
1. terminal$ make -j8 clang-only BUILD_CLANG_ONLY=YES &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; cf) Everytime you change TestPass.cpp or test.c or all_file_you_ve_made, you need to do this again.
1. terminal$ clang -mllvm -testpass -mllvm -time-passes -mllvm -debug-only=”testpass” test.c -O1

cf)
If you don’t want to make PATH permanently, make shell script and run it instead of modifying “.profile”.

cf2)
To use emacs comfortably, download _emacs from ETL. Then, “mv _emacs ~/.emacs”.

cf3)
In terminal, if you enter “Ctrl+r wanted_command”, you can find what you’ve entered.

### LLVM Core Classes

Go to [this page](http://llvm.org/docs/ProgrammersManual.html#the-core-llvm-class-hierarchy-reference) and read about LLVM’s core classes such as Function, BasicBlock, Instruction, etc.

### Basic traversal routines

Learn how to traverse the basic blocks in a function and instructions in a basic block.
Go to [this page](http://llvm.org/docs/ProgrammersManual.html#basic-inspection-and-traversal-routines)

### LLVM RTTI (Run-Time Type Identification)

Learn how to figure out how to figure out a type of a virtual class by using LLVM’s custom RTTI. Goto [this page](http://llvm.org/docs/ProgrammersManual.html#the-isa-cast-and-dyn-cast-templates) 

### LLVM API for debuging

<!-- CFG, DT 관련은 project 2로 -->

Read [this page](http://llvm.org/docs/ProgrammersManual.html#the-debug-macro-and-debug-option), to learn about `DEBUG`, `DEBUG_TYPE` macro and `-debug`, `-debug-only` option.

Normally, `-debug` or `-debug-only` option should be given as the arguments to `clang` if you want to see the debug outputs. However, you can use a little trick to turn debug outputs on/off while debugging with gdb. `set DebugFlag=1/0` does the trick. Note that this only works when `DebugFlag` global variable is visible in the current scope.

Use `dbgs()` to print debug outputs. It can be used similar to cout, like this:
```
dbgs() << “Printing debug outputs…\n”;
```

call F.dump() &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; cf) F is function type
 * display functions in IR

call I->dump() &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; cf) I is BasicBlock type
 * show which basic block I is


### Adding a command line option

Learn how to add a command line option. Read [this page](http://llvm.org/releases/3.4.2/docs/CommandLine.html#quick-start-guide)
(Just read the quick start guide)

### Things you need to do in this project
* Add a new function pass at the last stage of the optimization sequence (O1-3 optimization level)
* Enable a command line option that enable your pass. (default is disabled)
* Traverse basic blocks in each function and instructions in each basic block and use LLVM’s RTTI to count BinaryOperator, CallInst, CmpInst, GetElementPtrInst, PHINode, StoreInst, TerminatorInst, and LoadInst.
* Add `README.md` file to the project root directory that describes briefly what kind of instructions the above classes represent.
* `git add` your files and use `git diff --cached` to print the diffs. Generate the diff file by using I/O redirection. e.g.) `git diff --cached > proj1.diff` Make sure you stage all the changed files so that the diff file contains all the changes.

Submit the diff file to ETL by the deadline.

## APPENDIX

### Emacs Commands

| Key | Description |
| ------------- | ------------- |
| C-x C-c  | Exit  |
| C-g | Cancel command  |
| C-x 2/3 | Split window horizontally/vertically|
| C-x C-f | Open file |
| C-x k | kill buffer |
| C-x o | Move to the next window |
| C-s/C-r | Forward/Reverse incremental search, press repeatedly for next match |
| C-x a | Next buffer |
| C-x b | Switch to buffer (default: previous buffer) |
| C-x space | Set mark (move cursor and C-x space again to select region |
| M-w | Copy selected region |
| C-y | Paste |
| C-x 1 | Close all window except the current window |
| C-x + | Resize all window to the same size |

Note that GNU Emacs and GNU bash are the same GNU project and share many shortcut keys, including `C-g`, `C-r`, `C-y`. `C-a`, `C-e`, `C-k`.

Also, `C-z`, `fg` commands can be of great help.

### Emacs-Gdb Commands

In gdb window, C-c + followings. In other windows, C-x + C-a + followings.

| Key | Description |
| ------------- | ------------- |
| C-n | Next |
| C-s | Step |
| C-f | Finish |
| C-l | Current line |
| C-b | Set breakpoint |
| C-u | Until |
| C-r | Continue |
| C-< | Up stack |
| C-> | Down stack |

