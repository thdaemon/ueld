









##UELD ---- Usermode Environment Loader Daemon

Ueld (用户态环境加载器守护进程) 是一个轻量级的 Unix/Linux 环境下的 init 程序，可用于替换 SysVInit 和 Systemd

####编译与安装

```
$ cd path-to-ueld-src
$ make
$ sudo make install
```

安装需要 Root 用户，因此本例中使用了 `sudo(8)`，如果你的系统中没有 `sudo(8)`，请使用其他方法切换到 Root。

可使用 `PREFIX` 变量设置根文件系统，可使用 `INSTALLDIR` 变量设置安装目录。

**交叉编译**

如果你希望交叉编译 Ueld，那么你可以使用 `CROSS` 变量设置交叉编译器，例如交叉编译器为 arm-linux-gnueabihf-，你可以

```
$ make CROSS=arm-linux-gnueabihf-
```

####配置和使用 Ueld

在安装后建议立刻配置 Ueld，参考 [此 Wiki](doc/zh_CN/userguide.md)

**Ueld 使用手册也包含在这篇 Wiki 中；**卸载说明也包含在此 Wiki 中。

如果你第一次安装 Ueld，那么 Ueld 配置文件被设置为在 tty1-5 上创建 `getty(8)` 等待登录，这可能不适合你的情况，因此，不要忘记配置它。值得注意的是，安装 Ueld 时，如果相应目录已经存在配置文件，也不会覆盖之前的配置文件，即使新版本的缺省配置可能发生变化。

####Ueld 的优点

- Ueld 非常轻量级和简洁，“小既是美”。庞大的东西总是让人很不舒服，因此我尽力避免 Ueld 成为庞大的软件。

- 我尽力使 Ueld 仅提供 init 该提供的一系列原语操作，尽力做到 “提供机制，而不是策略”，行为的最终逻辑由用户制定，使其具有很强的灵活性。

	例如，ueld 绝对不会在加载系统时解析 `/etc/fstab` 配置文件，也绝对不会在启动时运行任何 `/etc/rcX.d/` 系统服务，而是在启动时运行一个 `/etc/ueld/sysinit.sh` 脚本，系统加载完毕后在后台不等待得运行 `/etc/ueld/sysloaded.sh` 脚本，并解析 `/etc/ueld/restarts.list` 文件以启动这些需要退出后需要立刻重新启动的程序（通常是 `getty(8)`），系统关机前运行 `/etc/ueld/syshalt.sh` 脚本，没错，ueld 不给用户强加任何策略！让用户有能力自主决定开机时准备什么等等，例如，用户可以在 sysinit.sh 中启动 `udevd(8)` 守护进程，可以通过脚本解析 `/etc/fstab` 并使用 `mount(8)` 挂载文件系统（简单情况下其实直接 mount 就好），可以使用 `modprobe(8)` 加载内核模块，可以使用 `ifconfig(8)` 等配置网卡...；可以在 sysloaded.sh 中打开一个 X Server，启动自己的实用程序...，当然也可以不这样或按照自己喜欢的方式编制了，一切的一切都可以由用户自己决定。

- Ueld 特别适用于像树莓派这样的设备，或者对开机速度特别在意的人。实际上，我就是在一种“类树莓派”板卡----NanoPi 2Fire 上开发 ueld 的。在该设备上使用 Debian 8 系统，用 ueld 替换 systemd 后，开机速度提升了 10-20 倍！！（这太令我感到惊讶了）

- 最后一个，也是一个 Ueld 独有的、在大多数情况下没什么用、但有时可能挺实用的小功能，我把它叫做”Muti Init“，他允许你将 Ueld 配置为开机时选择加载哪一个 init 程序，ueld，sysvinit，还是 systemd。（PC 的用户或许会嘲笑我可以在 grub 中添加几个条目，他们使用不同的 init=xxx 内核启动参数，但是必须提到的是，像 NanoPi 这种设备，通过重刷 u-boot 来修改一次修改启动参数是一件苦差事）

####Ueld 目前的问题

- Ueld 目前仅支持 Linux 系统，不过，绝大部分移植层代码都放置在 os/ 子目录中，其他代码也考虑到了移植问题（例如 BSD 上需要用 ioctl(2) 设置控制终端），因此可以移植到其他 Unix 系统，但 init 程序本身的特性决定了不同平台的代码必须存在大量差异，移植需要一定的工作。

- Ueld 目前不支持命令行和配置文件中转义字符和引号

- Ueld 目前使用了多个信号处理各种 ueldctl 命令，更好的方案是像 telinit 那样

- Ueld 目前不支持带参数重启

- Ueld 目前不能优雅得与现有桌面环境进行整合
