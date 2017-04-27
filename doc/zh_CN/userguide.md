**警告** 此文件的中文版本需要更新翻译

查看 [英文版本](../userguide.md)

### Ueld 用户手册 Wiki

Ueld 版本: 0.36

#### 程序和工具

>PREFIX 默认为 空白

>INSTALLDIR 默认为 /usr/lib/ueld/ueld-${UELD_VERSION}

${PRIFIX}${INSTALLDIR}/ueld

${PRIFIX}${INSTALLDIR}/ueldctl

符号链接:

${PRIFIX}/sbin/init -> ${PRIFIX}${INSTALLDIR}/ueld

$(PRIFIX}/sbin/ueldctl -> ${PRIFIX}${INSTALLDIR}/ueldctl

**ueld** Ueld 的 init 程序，用作系统启动后的 pid=1 的进程。

**ueldctl** 控制 Ueld 进程的实用工具，使用方法见 “ueldctl 实用工具” 一节。

#### 配置文件和脚本

>/etc/ueld/ueld.conf
>
>/etc/ueld/sysinit.sh
>
>/etc/ueld/sysloaded.sh
>
>/etc/ueld/syshalt.sh
>
>/etc/ueld/respawn.list
>
>/etc/ueld/ctrlaltdel.sh

**/etc/ueld/ueld.conf**

Ueld 的配置文件，格式：

```
name=value
```

'=' 两侧不允许空格，'value' 两侧不允许引号

Name|Default Value|Description
----|-------------|-----------
ueld_execfile|/sbin/init|Ueld 可执行文件路径，当使用 `ueldctl -l` 时 ueld 进程 exec 该程序，不允许相对路径
ueld_must_remount_before_poweroff|0|要求 Ueld 在切断机器电源之前必须卸载（或重载为只读）所有文件系统，否则停止关机过程并弹出 Shell
system_shell|/bin/sh|设置系统 Shell 程序，不允许相对路径
ueld_enable_muti_init|0|是否启用 Muti Init 功能
ueld_muti_init_other_init|空白|ueld_enable_muti_init 为 1 时（启用 Muti Init 功能时）有效，另一个 init 程序的绝对路径，例如 /bin/systemd
ueld_muti_init_other_init_telinit|空白|ueld_enable_muti_init 为 1 时（启用 Muti Init 功能时）有效，另一个 init 软件包的 telinit 程序的绝对路径，例如 /bin/systemd
ueld_muti_init_choose_time_out|3|ueld_enable_muti_init 为 1 时（启用 Muti Init 功能时）有效，开机时选择 init 程序时等待的最大秒数，超时则选择加载 ueld
ueld_muti_init_press_keys|Ss|ueld_enable_muti_init 为 1 时（启用 Muti Init 功能时）有效，在选择时，让 ueld 加载另一个 init 程序的热键，允许同时指定多个热键，区分大小写
ueld_path_var|blank|If it is not blank, ueld will set it to `$PATH` when `$PATH` is blank. If you want to force override `$PATH`, use ueld_override_path_var
ueld_override_path_var|0|When it is 1, ueld will force override `$PATH` to ueld_path_var
ueld_console_vt|CONFIG_CONSOLE_VT or 1|see [genconfig.md](../genconfig.md)

注：上表中的“默认”值是指如果 ueld.conf 文件中不存在这一项，ueld 采用的值，而不是指的安装 Ueld 时直接复制的 ueld.conf 文件中的配置！

**/etc/ueld/sysinit.sh**

系统初始化脚本，Ueld 在启动时直接运行的脚本

ueld 将等待该脚本执行完毕再继续运行，该脚本继承 ueld 的标准输入标准输出和标准错误

适合在此脚本做的工作：启动 `udevd(8)` 守护进程；使用 `mount(8)` 挂载文件系统；使用 `modprobe(8)` 加载内核模块；使用 `ifconfig(8)` 等配置网络接口；启动你需要的系统服务

在安装时会复制一份示例，该示例启动 `udevd(8)` 守护进程。

**/etc/ueld/sysloaded.sh**

在系统加载完毕后执行的脚本

ueld 不等待地将该脚本放置到后台运行，该脚本的标准输入标准输出和标准错误重定向到 /dev/null

适合在此脚本做的工作：启动 X Server（X Window System）；启动一些实用程序

在安装时会复制一份示例，该示例演示如何启动 X Server，但命令被注释，这是因为并非所有人都愿意开机时打开 X Server，如果需要，你可以简单修改并取消注释。

**/etc/ueld/syshalt.sh**

系统关机前运行的脚本

ueld 将等待该脚本执行完毕再继续运行，该脚本继承 ueld 的标准输入标准输出和标准错误

在安装时会复制一份示例，一般不需要再更改了，如果需要可阅读脚本中的注释

**/etc/ueld/respawn.list**

ueld 会在执行完 sysinit.sh 后解析该文件，格式：

```
vt编号:命令行
...
```

ueld 会执行每一行中的命令行，并将他创建在你指定的 virtual console 上（设置控制终端为该 vt），如果vt编号为0或空白，则不设置控制终端。在指定的命令行退出后，如果不是应用程序使用 _exit(EXIT_FAILURE) 退出（例如正常推出或接受到信号而推出），那么 ueld 再次重新启动该命令行。

If you define `CONFIG_NO_IGN_FAIL_PROC` (you can do it by mkconfig.sh), ueld will always respawn all process.

在安装时会复制一份示例，该示例在 tty1-5 上启动 `getty(8)`，如下

```
1:getty tty1
2:getty tty2
3:getty tty3
4:getty tty4
5:getty tty5
```

**/etc/ueld/ctrlaltdel.sh**

用户在 virtual console 上按下 Ctrl-Alt-Del 时运行的脚本

ueld 将等待该脚本执行完毕再继续运行，该脚本继承 ueld 的标准输入标准输出和标准错误

注：在 X Server 一类的应用程序运行时，键盘处于 Raw 模式，此时 Linux 内核的 vt 驱动不会处理此按键，因此 ueld 不会得到通知

#### ueldctl 实用工具

`ueldctl -p` 或 `ueldctl --poweroff` 关机

`ueldctl -r` 或 `ueldctl --reboot` 重新启动系统

`ueldctl -l` 或 `ueldctl --reload-init` 重新加载 init 程序

`ueldctl -h` 或 `ueldctl --help` 显示 usage

还有一种特殊用法，你可以创建指向 ueldctl 的符号链接，名字为 poweroff 则不带参数运行等价于 `ueldctl -p` ，reboot 同理

#### 卸载 Ueld

1. 删除安装 Ueld 时设置的安装目录(默认为 ${PREFIX}/usr/local/sbin/ueld/ueld-${UELD_VERSION})，一级配置文件 ${PREFIX}/etc/ueld/ 目录

	```
	# rm -r ${PREFIX}/etc/ueld
	# rm -r where-you-install-ueld
	```

2. 删除软链接 /sbin/init 和 /sbin/ueldctl，（如果有必要）并将 /sbin/init 设置为新 init 程序的软链接

	```
	# rm /sbin/init /sbin/ueldctl
	# ln -s the-path-of-new-init /sbin/init
	```

3. （如果是在本机上为当前正在运行的系统卸载 Ueld，那么）卸载 Ueld 会导致这一次关机遇到麻烦，但目前没有更好的选择，一种方法是直接关机

	```
	# kill -TERM 1
	```

	由于 ueldctl 实用程序已经被删除，因此我们得使用 `kill(1)` 命令给 ueld 发送信号。

	但这样会导致下一次开机时 `fsck(8)` 实用程序检查文件系统，因为删除 ueld 程序后，ueld 的进程 (pid 1) 无法退出或加载其他程序，使得 ueld 所在的文件系统无法卸载或重载为只读。（ueld 关机时会调用 `sync(2)` 系统调用同步文件系统，因此不会导致数据丢失）

	另一种方法是让 ueld 加载一个其他的 init 程序，由于配置文件已经被删除，因此 ueld 使用缺省配置 /sbin/init，确保他可用后，执行

	```
	# ls -l /sbin/init
	lrwxrwxrwx 1 root root 20 Jan 7 12:14 /sbin/init -> /lib/systemd/systemd
	# kill -USR1 1
	```

	但这种方法的代价是新的 init 程序很可能启动系统服务等，因此最好的方法是关闭一切应该关闭的东西，并在 tty8 等不太可能用到的虚拟控制台上执行。

	如果是为另一个系统卸载 Ueld，例如，情况是嵌入式设备准备的根文件系统，那么不需要此步。

