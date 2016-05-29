#ucore在v9-CPU上的移植
**韩旭 徐磊**

## 概述
将ucore移植到v9-CPU上是我们组课程设计的核心内容。根据我们的设想，我们将完成ucore代码（包含了lab1~lab8）在v9-CPU上的移植工作，我们进一步的希望能实现更多的操作系统内存管理、进程管理的策略算法，并能够提供更好的实验是指导书。

由于对于代码移植部分的困难估计不足，因此目前只完成了实验的主体部分，即代码移植。目前我们已经将ucore移植到v9-CPU上，相关代码可以较为稳定的运行。我们将8个实验进行了一定的合并，合并之后的包括了6个实验。目前移植的实验和ucore保持功能一致，但在某些硬件相关的部分没有完全按照ucore的抽象层次实现，这样可以保持程序的简单。

## 相关工作介绍

**[swieros](https://github.com/rswier/swieros)** 是v9-CPU的原型，作者设计了一个兼具RISC和SISC特性的指令集，并为指令集实现了一个简化的C编译器以及相应的模拟器。作者在这套框架上移植了xv6教学操作系统，但是受限于编译器和模拟器，xv6被较多的修改，而且在实现上也有一些不太合理的地方。

**[ucore](https://github.com/chyyuu/ucore_os_lab)** 是清华大学操作系统课程采用的教学操作系统，这套操作系统具有较为完善的抽象层次和代码结构，操作系统被拆分成了8个lab，每个lab都在前一个lab上进行了一些修改和加入了新的功能。比较适合增量开发，但是这样的开发也可能造成一些麻烦。

**[v9.js](https://github.com/JianxinMa/v9.js)** 是今年课程设计另一个小组的工作，它把v9的编译器和调试器都移植到javascript上，这样就可以有一个在浏览器上的运行环境。我们希望我们移植之后的操作系统可以在他们提供的平台上运行。

**[alex-machine](https://github.com/paulzfm/alex-machine)** 参考v9定义的新指令集和模拟器。

**[alex-llvm](https://github.com/a1exwang/llvm)** 采用LLVM作为C语言的编译器，这样就可以使用标准的C进行操作系统的编程而不必考虑编译器带来的各种局限。因为alex与v9非常类似，将来可能可以把v9-ucore通过比较少的修改移植到alex-machine上。

## 实现方案

## 主要难点

## 开发过程

## 实验总结