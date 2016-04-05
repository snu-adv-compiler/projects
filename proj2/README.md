# Project 2

### DUE: Monday, May 2, 2016 at 11:59pm

In this project, we will make a pass for detecting natural loops. As you have learned in the class, you will need to create a depth first tree (DFT) of the control flow graph (CFG) and use the DFT and the dominator tree to identify natural loops. After identifying natural loops, you need to display the natural loop by highlighting the corresponding basic blocks in the CFG. 

Of course, LLVM provides a pass for detecting natural loop. **Do not** use any of the classes or functions that can make the project trivial, such as `LoopInfo` class or `FindFunctionBackedges` function.

For your convenience, we recommend fully using c++ standard library (especially container APIs such as std::vector, std::set, std::map, etc)  and llvm. `assert()` function can greatly improve the debugging process so learn how to use it. Also, we recommend using cscope and doxygen document for LLVM for learning how to use LLVM APIs. We will provide the script for generating cscope database files. Download it and locate in your project root directory, modify the `LLVM_DIR` and `LLVM_SRC_DIR` accordingly and run the script. For doxygen documents you can use online LLVM doxygen document. Just google `LLVM <class name>` then most likely you will get a link to the doxygen document for that class. However, the online doxygen document corresponds to up-to-date LLVM repository so there can be a mismatch between 3.4.2 version source code and the online document. If you want to avoid that, you can build doxygen document from the source file as follows.
```
cd (LLVM_OBJ_DIR)
make DIRS=docs        # It’s okay if you have errors here
cd docs
make doxygen             # It may take a long time
```

`doxygen` folder will be created in `docs` folder. Open `index.html` and start browsing!

See the appendix for more on Doxygen and Cscope.

### Using an Analysis Pass

First, we need to learn how to use the analysis passes. Since we are going to use the dominator tree, include the following header.

```{c++}
#include “llvm/Analysis/Dominators.h”
```

Then provide the following function in `TestPass`

```{c++}
virtual void getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<DominatorTree>();
  AU.setPreservesAll();
}
```

This function is used in the pass manager to decide which analysis pass needs to be run before running the pass. `setPreservesAll` means that the pass do not modify IR. If we don’t include it, the pass manager will assume that the pass will destroy the analysis information present up to that point, and if any analysis pass is needed by later passes, that analysis pass will need to be run again. Refer to [this page](http://llvm.org/releases/3.4.2/docs/WritingAnLLVMPass.html#specifying-interactions-between-passes) for more info.

Finally, you need to modify the following part
```{c++}
INITIALIZE_PASS(TestPass, "test-pass", "Test Pass", false, false)
```
into
```{c++}
INITIALIZE_PASS_BEGIN(TestPass, "test-pass", "Test Pass", false, false)
INITIALIZE_PASS_DEPENDENCY(DominatorTree)
INITIALIZE_PASS_END(TestPass, "test-pass", "Test Pass", false, false)
```

This set of macros will define proper initialization routine to be called when `TestPass` instance is created.

To use the analysis, include the following in `runOnXXX`

```{c++}
DominatorTree *DT = &getAnalysis<DominatorTree>();
```

Now you can use various public functions provided by `DominatorTree`.

### Finding the Natural Loops

In this project, we will not provide detailed instructions on how to use the APIs provided by the classes. It is your job to find example usage of the APIs and learn how to use it. We will just provide the directions and important classes you can use.

First you will need to create a DFT. Create a class representing DFT. Then, starting from the entry basic block, visit successors of the basic block and build up DFT. Use methods provided in `Function` and `BasicBlock` class. Refer to [this link](http://llvm.org/releases/3.4.2/docs/ProgrammersManual.html#iterating-over-predecessors-successors-of-blocks) for how to visit predecessors and successors of the basic block.

The purpose of DFT is to find the back-edges in the CFG. If you think you can find back-edges without constructing a DFT (e.g. using `df_iterator` defined in `llvm/ADT/DepthFirstIterator.h` or finding back-edges while traversing the CFG), you can do that, but be sure that your algorithm of finding a back-edge is correct.

Next, use the dominator tree analysis to identify natural loops associated with each of the back-edges. Use the following algorithm as a reference. (from Muchnick p192)


```
detect_nat_loop(in G, e; out L)  // e is a back-edge
  u = tail(e);
  v = head(e);
  L = empty;
  Stack = empty;
  if v does not dominate u then
    return;   	// multi-entry loop
  fi
  L = {v, u};
  if v not equal u then   // if not a self-loop
    Stack += u;
  fi
  while Stack is not empty do
    w = Stack.pop();
    for each predecessor p of w in G do
      if p is not in L then
        L = L + {p};
        Stack += p;
      fi
    od
  od
end
```

### Highlighting the Natural Loop on the CFG

Visualizing the analysis result or code structure can be very helpful when you implement some functionality or modules. So in this project we will visualize the detected natural loops by highlighting them in the CFG.

To highlight basic blocks belonging to a natural loop in the CFG, we can use GraphWriter and DOTGraphTraits classes. While preparing for the project we found that there is no clean way to enable highlighting basic blocks. Either it needs a lot of code duplication or changes to the existing LLVM classes. We thought that figuring out how to do it is not much meaningful for the purpose of this class, so we provide the patch file to enable highlight of nodes in the CFG.

Download the patch file and apply the patch as follows:
```
cd (LLVM_SRC_DIR)
patch -p1 < (patch_file)
```

If you see the patch file, we have added `NodeGraphAttrs` map in the `Function` class that holds the mapping between `BasicBlock*` and the string representing the attributes that should be applied to the node representing that basic block.

To highlight a node, write a code like the following.
```{c++}
F->NodeGraphAttrs[BB] = “color=red”;
```

Call `F.viewCFGOnly()` method for each detected natural loop.

Note that in order for the graph viewer to work, `gv` and `graphviz` tool must be installed *before* building LLVM. If you’re using local Ubuntu machine and have trouble viewing the graph, [this site](http://askubuntu.com/questions/438451/could-not-open-display-null-error-how-can-i-fix-this) might help.

### Example Output
Your pass should display the following highlighted graphs, one by one. (Not necessarily in the same order)

<img src="https://github.com/snu-adv-compiler/projects/blob/master/proj2/images/result2-1.png" width="200">
<img src="https://github.com/snu-adv-compiler/projects/blob/master/proj2/images/result2-2.png" width="200">
<img src="https://github.com/snu-adv-compiler/projects/blob/master/proj2/images/result2-3.png" width="200">
<img src="https://github.com/snu-adv-compiler/projects/blob/master/proj2/images/result2-4.png" width="200">


## APPENDIX

### Doxygen & Cscope

Doxygen is a tool that parses the project source codes and produces documents to help programmers to navigate the source code. Doxygen-generated HTML document for the latest repo is available online. You just google `llvm <name of the class>` or `llvm <keywords>` and you’ll see several links for doxygen-generated pages. This is very useful if you want to view the class hierarchy or view the list of public functions, etc. If you have built doxygen documents from the source code, open `index.html` and type the class name in the search box, or click `Classes` to view the list of all the classes in LLVM.

Cscope is another tool that can help you navigate the source code. Its main use is to find the locations where a certain symbol (e.g. a function or a class) is used. It will help you if you want to find the examples of how a class or a function is used in other files in the LLVM project. There are many references on the web for how to use cscope so find and learn how to use it. It will be a lot easier to use it inside `vim` or `emacs`. Download the latest cscope plugin file (for emacs, go [here](https://github.com/dkogan/xcscope.el)), since it will provide more useful features.

