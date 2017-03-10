









##UELD ---- Usermode Environment Loader Daemon

Ueld (用户态环境加载器守护进程) 是一个轻量级的 Unix/Linux 环境下的 init 程序，可用于替换 SysVInit 和 Systemd

####编译与安装

```
$ cd path-to-ueld-src
$ make
$ sudo make install
```

*安装需要 Root 用户，因此本例中使用了 `sudo(8)`，如果你的系统中没有 `sudo(8)`，可以使用其他方法切换到 Root。

可使用 `PREFIX` 变量设置根文件系统，可使用 `INSTALLDIR` 变量设置安装目录。

####交叉编译

如果你希望交叉编译 Ueld，那么你可以使用 `CROSS` 变量设置交叉编译器，例如交叉编译器为 arm-linux-gnueabihf-，你可以

```
$ make CROSS=arm-linux-gnueabihf-
```

####配置和使用 Ueld

在安装后建议立刻配置 Ueld，参考 [此 Wiki]()

Ueld 使用手册也包含在这篇 Wiki 中。

如果你第一次安装 Ueld，那么 Ueld 配置文件被设置为在 tty1-6 上创建 `getty(8)` 等待登录，这可能不适合你的情况，因此，不要忘记配置它。值得注意的是，安装 Ueld 时，如果相应目录已经存在配置文件，也不会覆盖之前的配置文件，即使新版本的缺省配置可能发生变化。

####卸载 Ueld

1. 删除安装 Ueld 时设置的安装目录(默认为 ${PREFIX}/usr/local/sbin/ueld/ueld-${UELD_VERSION})，一级配置文件 ${PREFIX}/etc/ueld/ 目录

	```
	# rm -r ${PREFIX}/etc/ueld
	# rm -r where-you-install-ueld
	```

2. 删除软链接 /sbin/init 和 /sbin/ueldctl，（如果有必要）并将 /sbin/init 设置为新 init 程序的软链接

3. 卸载 Ueld 会导致这一次关机遇到麻烦，但目前没有更好的选择，一种方法是直接关机

	```
	# kill -TERM 1
	```

	由于 ueldctl 实用程序已经被删除，因此我们得使用 `kill(1)` 命令给 Ueld 发送信号。

	但这样会导致下一次开机时 `fsck(8)` 实用程序检查文件系统，因为删除 ueld 程序后，ueld 的进程(pid 1)无法退出或加载其他程序，使得 ueld 所在的文件系统无法卸载或重载为只读。（ueld 关机时会调用 `sync(2)` 系统调用同步文件系统，因此不会导致数据丢失）

	另一种方法是让 Ueld 加载一个其他的 init 程序，由于配置文件已经被删除，因此 Ueld 使用缺省配置 /sbin/init，确保他可用后，执行

	```
	# kill -USR1 1
	```

	但这种方法的代价是新的 init 程序很可能启动系统服务等，因此最好的方法是关闭一切应该关闭的东西，并在 tty8 等不太可能用到的虚拟控制台上执行。

####Ueld 的优点

- Ueld 非常轻量级，符合 Unix 哲学“小既是美”

- Ueld 仅提供 init 该提供的一系列原语操作，提供“机制，而不是策略”，行为的最终逻辑由用户制定，具有很强的灵活性。Ueld 不像 systemd 一样什么都大包大揽，而是使不同的功能分割给不同的软件包，他们之间使用用户编写的脚本和配置文件连接，从而给用户提供了无与伦比的自由。

	例如，ueld 绝对不会在加载系统时解析 `/etc/fstab` 配置文件，也绝对不会在启动时运行任何 `/etc/rcX.d/` 系统服务，而是在启动时运行一个 `/etc/ueld/sysinit.sh` 脚本，系统加载完毕后在后台不等待得运行 `/etc/ueld/sysloaded.sh` 脚本，并解析 `/etc/ueld/restarts.list` 文件以启动这些需要退出后重新启动的程序（通常是 `getty(8)`），系统关机前运行 `/etc/ueld/syshalt.sh` 脚本，没错，ueld 不给用户强加任何策略！让用户有能力自主决定开机时准备什么等等，例如，用户可以在 sysinit.sh 中启动 `udev(7)` 程序，可以通过脚本解析 `/etc/fstab` 并使用 `mount(8)` 挂载文件系统，可以使用 `modprobe(8)` 加载内核模块，可以使用 `ifconfig(8)` 等配置网卡...；可以在 sysloaded.sh 中打开一个 X Server，启动自己的实用程序...，当然也可以不这样或按照自己喜欢的方式编制了，一切的一切都可以由用户自己决定。

	这就是 Ueld 坚守的设计信念，当然不是 Ueld 自创的，而是大多数 Unix 黑客一贯的设计作风，最终用户永远比软件设计者更明白自己需要的究竟是什么。

- Ueld 特别适用于像树莓派这样的设备，或者对开机速度特别在意的人。实际上，我就是在一种“类树莓派”板卡----NanoPi 2Fire 上开发 ueld 的。在该设备上使用 Debian 8 系统，用 ueld 替换 systemd 后，开机速度提升了近20倍！！（这太令我感到惊讶了）

- 最后一个，也是一个 Ueld 独有的、在大多数情况下没什么用、但有时可能挺实用的小功能，我把它叫做”Muti Init“，他允许你将 Ueld 配置为开机时选择加载哪一个 init 程序，ueld，sysvinit，还是 systemd。（PC 的用户或许会嘲笑我可以在 grub 中添加几个条目，他们使用不同的 init=xxx 内核启动参数，但是必须提到的是，像 NanoPi 这种设备，没人愿意通过重刷 u-boot 来仅仅是修改一次修改启动参数）
