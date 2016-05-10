# Project 3

### DUE: Monday, June 13, 2016 at 11:59pm

In this project, we will make a pass that will actually change the IR. We would like to change the program so that, if there is a integer division in the program we want to make sure that no divide-by-zero will occur. We only want to change the division if there is probability that the denominator becomes zero at runtime. Thus the project consists of two part. First you need to analyze the data flow to find out if the denominator can become zero. Next, you need to change the IR (by adding some instructions and altering existing instructions… ) so that if the denominator becomes zero, instead of division instruction, the result will be set to the maximum integer value.

This will be something like a symbolic analysis, but it can become very difficult if we want to cover all the cases that can happen. This is just a project that is meant to help you learn the concepts, so you only need to cover the case in the provided test code.

```{c}
void foo(int a, int b) {
  c = a + b;
  d = c - 5;
  e = 100 / d;
}
```

Look at the above example. First you need to find an integer division instruction. Then find the denominator operand (using `getOperand`) and it will return the instruction defining the operand. (Since LLVM IR is in SSA form, there is only one definition for the operand. There is no separate use-def chain. Thus LLVM developers designed IR so that the operand is actually the instruction defining that operand!) (click [here](
http://llvm.org/releases/3.4.2/docs/ProgrammersManual.html#iterating-over-def-use-use-def-chains) for some related info)
The operand is an instance of an instruction that provides definition of that operand. In the example shown above, `d = c - 5` is the instruction defining the denominator `d`. Since it is not a constant, you need to examine its operands to know the possible values it can become. So you will visit `c = a + b`. In this instruction, both operands are variables so you need to iterate through both operands. If you visit `a` or `b`, it is not an instance of `Instruction` anymore, but just a function parameter. Analyzing beyond the procedure boundary is much difficult and we won’t do that in this project. So you just assume that `a` and `b` can be any value and thus, ‘Unknown’. At this point, you propagate the possible value information down to the division instruction. Since `a` and `b` are unknown, `c` is unknown, and `d` is unknown. Thus `d` can be any value, even zero. Now that you know that the divide-by-zero can happen, you should modify the division instruction. There can be situations where the denominator can become one of the several integer values except zero. In this case you don’t have to modify the division instruction.

Following are some rules to simplify the project:
- If you encounter another division while tracking the possible value of the denominator, for example, `e = 100 / d`, if the possible values of `d` contain zero, the maximum integer value should be added to the possible values of `e`. If `d` is unknown, then `e` is unknown.
- We won’t do the memory analysis. That is, value defined by `Load` instruction---if you ever see one while doing your project---is ‘Unknown’. 
- Control flow can sometimes provide information of the range of values of a variable at some point. For example, in the following code, the value of `b` in `c = b + 5`  must be less than 0. But we also ignore such information that we can get through the control flow.

```{c}
if (b < 0)
  c = b + 5;
```
- There are many types of IR but to simplify the problem, we constrain the types of IR only to those contained in the uploaded test.c. Your pass only need to handle the cases in `test.c` without error.

If you have found out that the denominator in the division instruction can become zero, you need to modify the IR so that the divide-by-zero exception is not triggered at runtime. Instead, if the value of the denominator is zero at runtime, it must be set to the highest integer value. In order to do this, you should create new basic blocks (or split existing basic block) and add new instructions.
(click [here](http://llvm.org/releases/3.4.2/docs/ProgrammersManual.html#making-simple-changes)) and for some helpful info.)


### Example output while running your pass

```
============= Found Division =============
Instruction : %div44 = sdiv i32 %div43, %c.2
Possible values for the denominator : 0 1 -77 99
Possible divide by zero!!! 
//print changed CFG (you should highlight changed basic block)
============= Found Division =============
Instruction : %div45 = sdiv i32 %add42, %d.1
Possible values for the denominator : unknown
Possible divide by zero!!! 
//print changed CFG
============= Found Division =============
Instruction : %div46 = sdiv i32 %add45, %f.1
Possible values for the denominator : 1, 66
```
