# 更新日志 ChangeLog

20180909 23:50 by 王陆洋 Leon Wong

Version: 0.5.0.0

Branch: Console

Contents:

* Support system API of window management.
* Support ls, cat, mem, etc. commands.
* Add system protection.
* Allow third-party apps with compatible MewOS APIs.
* 加入系统窗口API。
* 加入ls、cat、mem等内置命令。
* 加入系统保护，避免第三方应用随意访问内存。
* 支持运行使用了兼容的MewOS API的第三方APP（详见API.h）

---

20180907 02:20 by 王陆洋 Leon Wong

Version: 0.3.0.0

Branch: Console

Contents:

* Add a console window.
* Add task switch.
* Add window switch.
* Allow keyboard input (only EN-US support).
* 加入命令行窗口。
* 加入任务切换功能。
* 加入前台窗口切换。
* 允许使用en-us布局的键盘进行输入。

---

20180906 00:02 by 王陆洋 Leon Wong

Version: 0.2.1.0 (For new functions are only added but not used.)

由于只引入了多任务进制，还未加入多任务的应用，所以没有更改版本号。

Branch: MultitaskManagement.

Contents:

* Introducing multitasking, which supports up to 10 * 100 (level * priority) tasks.
* 引入多任务管理，支持10 * 100 （大等级 * 小等级）个任务。

---

20180905 10:36 by 王陆洋 Leon Wong

Bugfix

Contents:

* Fix a bug that system gets stuck when coming into interrupts handling.
* 修复了系统在中断响应时的严重卡顿。

---

20180904 23:18 by 王陆洋 Leon Wong

Version: 0.2.1.0

Contents:

* Timer support.
* Keyboard input support.
* Add TextBox widget support.
* Allow mouse to move window with left button.
* 加入定时器功能。
* 支持使用键盘输入文字（仅限纯英文）。
* 加入文本区控件。
* 允许使用鼠标右键进行窗口拖拽。

---

20180831 04:31 by 王陆洋 Leon Wong

Version: 0.1.0.0

Contents:

* Finish sheet window management.
* Allow mouse to go around~

* 完善图层式窗口管理。
* 完善鼠标功能。

---

20180830 04:49 by 王陆洋 Leon Wong

Version: 0.0.3.1

Contents:

* Memory checking.
* 加入内存检查功能，能计算可使用的内存大小。

---

20180830 03:12 by 王陆洋 Leon Wong

Version: 0.0.3.0

Contents:

* Advanced Mouse & Keyboard interrupts handling.
* 对键盘和鼠标的中断处理，能够显示二者的状态和数据。

---

20180827 03:33 by 王陆洋 Leon Wong

Version: 0.0.2.2

Contents:

* Enable interrupts handling from keyboard.
* Add PIC Management.
* 允许处理来自键盘的中断。
* 加入PIC管理。

Know issues:

- Mouse cannot move.
- 由于尚未完全实现中断机制，鼠标不能移动。

---

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

