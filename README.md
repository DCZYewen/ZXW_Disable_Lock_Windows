# ZXW_Disable_Lock_Windows Tool readme file .

## 免责声明

本软件仅供交流学习之用，请于下载24小时后自行删除。由于使用本程序带来的任何后果，我们不负任何责任。

本软件可以解除ZXWXStudentClient程序的锁屏功能，消除它对计算机的影响。如果使用者的行为对ZXWXStudentClient的开发公司造成了任何影响和损失，责任由使用者承担。

程序基于GPL V3开放源代码。

## 编译方法

在`http://www.mingw.org/`下载Mingw，将`%Mingw%\bin`加入path后，运行指令`g++ .\main.cpp -fexec-charset=gbk -O ZXW_Disable_Lock_Windows.exe`编译程序。正常情况下，应该可以得到`ZXW_Disable_Lock_Windows.exe`的x86可执行程序。对于Windows10 For ARM，请在X86下对应地交叉编译。

## 运行方法

您可以下载本程序的二进制程序直接双击运行，或者使用命令行调起程序。

## 原理

```滞学网最近搞了一个“锁屏功能”搞心态。
这波来一手反击。先说，这个办法学会了也不要乱用，就也是探讨一波原理。哪个同学要是真的翻车了，我这也是没有办法。把这点东西发上来也不是要大家水课，课该上还是要上的奥！（保命声明）
打开我们的x学x校的安装目录：C:\Program Files (x86)\ZXWXStudentClient.赫然看见，图1的文件。这给了我们很多信息。这个x学x校是用C#编写的，主要原理是ChromiumWebBrowser内嵌到WPF（Windows Presentation Foundation）中。我们知道， C# 语言在 Windows 下，是通过微软的 C# 编译器，生成了 IL（相当于一种指令） 代码，运行在 CLR（相当于指令的解释器） 中。这时候我们直接打开dnSpy进行一波反编译。
翻啊翻啊翻啊翻，我们找到了图2的代码。芜湖起飞，这就是调起LockWindow（图3）的代码了。掏出我们的Visual Studio Blender，大概瞜一眼窗体的信息。结合代码我们知道。在partial class MainWindow中，继续定义了MainWindow，很奇怪的写法。只要当窗口不是最大化，或者x学网的窗口不是当前选中的窗口，就会触发相当于一个事件“拉起LockWindow，倒计时21秒后ZXWXStudentBrowser.Back()调起”。这个时候我大概心里有数了。只需要更改.net Framework，让System.Windows.Windows的IsActive无论如何都返回1（欺骗x学网让它以为自己是最前的窗口，就相当于他是第二层，把我想成了第一层，而我在第五层），或者，直接更改x学网校的代码，让我画第二个横线的地方的条件永远为假就不会触发下边的事件了（相当于我给它玩一手卡视野）。
但是很遗憾啊，两种方法都没成功，他们的项目太大了，依赖太多，我懒得一个一个找dll链接程序。
这个时候我灵光一现！如果在检测到你溜号的时候触发了退出事件，我们将其阻塞一手怎么样。这时候我兴奋的打开了OD，追踪一下栈（图4）。失败了，这是一个类似于即时编译的程序，它的内部变量在程序内存中到处乱飘，事件装载也只能看见dll加载。
不不不！我又有了主意，这时候就要涉及Windows的一个特性咯，程序每一秒以一个Timer递减更改LockWindow里的文字，如果我们把句柄消除，让更改SetText方法阻塞不就可以实现阻塞整体的返回程序么！说干就干，使用Cygwin我们有了posix兼容的进程管理模型，掏出我们的C语言，写下（图5）的弱鸡代码。找到ZXW的进程，调用.\sabeezxw.exe 124223（进程号）。ohhhhhhhhhhhhhhhhhhhhhhhhhhhhh，LockWindow顺利被结束而主窗口没有返回！成功！然后我就开始深刻的思考这个问题，找到LockWindow的进程号并命令行调用我的小程序发送SIGUSR1信号是不是对一般人来说太难了，有没有什么好的方法呐。芜湖，有的，只需要你在Windows10中按下Windows键+Tab键，找到LockWindow右上角的叉叉（类似图6），给它点了就完成了类似我的sabeezxw.exe完成的功能。或者找到任务栏的图标右键关闭窗口，（并未测试嗷），类似图7，应该也能实现类似的功能。```
