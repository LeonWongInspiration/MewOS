# 更新日志 ChangeLog

20180826 03:36 by 王陆洋 Leon Wong

Version: 0.0.2.1

Contents:

* Init mouse with a picture.
* Add GDT Management.
* Add IDT Management.
* Use modified C stdio.
* 初始化鼠标。
* 加入GDT管理。
* 加入IDT管理。
* 魔改了C的头文件stdio.h，允许使用特别的格式化输出。

Know issues:

* Mouse cannot move.
* 由于尚未完全实现中断机制，鼠标不能移动。

---

20180825 03:30 by 王陆洋 Leon Wong

Version: 0.0.2.0

Contents:

* Add fonts.
* Now we can show texts on the screen.
* Fix the bug when fix a box with wrong diagonal.
* Group codes into several files.
* 加入字体（目前使用hankaku免费字体，后期会进行调整）
* 现在可以在屏幕上显示文字了
* 修复了使用错误的对角线画方形时的bug
* 将代码分类

---

20180824 00:30 by 王陆洋 Leon Wong

Version: 0.0.1.0

Contents:

* Add asm functions for C to control devices.
* Add palette support.
* Add drawing interfaces.
* 加入汇编IO函数，以便在C语言中控制设备。
* 加入调色盘。
* 允许在系统界面上绘画。

---

20180823 04:50 by 王陆洋 Leon Wong

Version: 0.0.0.3 (not finished)

Contents:

* Introduce C Language
* Availability to modify VRAM
* 32 bit support
* This version has not been finished yet, and doesn't compile.
* 引入C语言
* 允许设定VRAM
* 加入32bit的模式
* 此版本还没有完工，并且暂时不能进行编译

---

20180821 03:30 by 王陆洋 Leon Wong

Version: 0.0.0.2

Contents:

* Add an asm source code that can make the image file.
* Add Initial Program Loader (IPL) source code.
* 添加镜像的x86汇编源码。
* 添加IPL的源码。

P.S.

* Nasm compiler was once used, but the grammar is finally in nask.
* 原先使用nasm编译器，但是最后决定使用nask语法。 

---

20180819 23:59 by 王陆洋 Leon Wong

Version: 0.0.0.1

Contents:

* Add a bootable image file.
* 加入可引导用镜像文件。

---

20180819 01:48 by 王陆洋 Leon Wong

Version: 0.0.0.1

Contents:

* Initializing the project with documentation
* 初始化项目，上传有关文档

