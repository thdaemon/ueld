## UELD ---- Usermode Environment Loader Daemon

Switch language: [简体中文](README.zh_CN.md)

> Note that this readme file has not yet completed the translation work

Ueld (Usermode Environment Loader Daemon) is a lightweight init program under Unix/Linux environment, it can be used to replace SysVInit and Systemd

#### Compile and install

```
$ cd path-to-ueld-src
$ make
$ sudo make install
```
The installation requires root user, so `sudo (8)` is used in this example. If you do not have `sudo (8)` in your system, use other methods to switch to Root.

You can use the `PREFIX` variable to set the root file system. You can use the` INSTALLDIR` variable to set the installation directory.

**Cross compile**

If you want to cross-compute Ueld, then you can use the `CROSS` variable to set the cross compiler, such as the arm-linux-gnueabihf- cross compiler, you can

```
$ make CROSS=arm-linux-gnueabihf-
```

#### Configure and use Ueld

After the installation, i suggest you configure Ueld immediately, refer to [this wiki](doc/zh_CN/userguide.md)

> There is currently no English version, but soon there will be.

** Ueld manual is also included in this wiki; ** Uninstallation instructions are also included in this wiki.

If you first install Ueld, then the Ueld configuration file is set to create `getty (8)` on tty1-5 to wait for login, which may not be suitable for your situation, so do not forget to configure it. Noteworthy, when you install Ueld, if the  configuration file in the corresponding directory is already exists, it will not overwrite the previous configuration file, even if the new version of the default configuration may be changed.

#### Advantages of Ueld

- Ueld is very lightweight and simple, "small is both beautiful". The huge software is always uncomfortable, so I try to avoid Ueld becoming a huge software.

- 我尽力使 Ueld 仅提供 init 该提供的一系列原语操作，尽力做到 “提供机制，而不是策略”。通过用户可编辑的脚本，将系统启动，关机时的具体策略交由不同的软件去完成，所有行为的最终逻辑可由用户根据他们自己的需要制定，使其具有很强的灵活性。

- Ueld 特别适用于像树莓派这样的设备，或者对开机速度特别在意的人。实际上，我就是在一种“类树莓派”板卡----NanoPi 2Fire 上开发 ueld 的。在该设备上使用 Debian 8 系统，用 ueld 替换 systemd 后，开机速度提升了 10-20 倍！！（这太令我感到惊讶了）

- 最后一个，也是一个 Ueld 独有的、在大多数情况下没什么用、但有时可能挺实用的小功能，我把它叫做”Muti Init“，他允许你将 Ueld 配置为开机时选择加载哪一个 init 程序，ueld，sysvinit，还是 systemd。（PC 的用户或许会嘲笑我可以在 grub 中添加几个条目，他们使用不同的 init=xxx 内核启动参数，但是必须提到的是，像 NanoPi 这种设备，通过重刷 u-boot 来修改一次修改启动参数是一件苦差事）

#### Ueld current question

- Ueld currently only supports Linux systems, however, most of the transplant layer code is placed in the os/ subdirectory, the other code also take into account the portability problem (such as BSD need to use ioctl (2) set the control terminal), it can be transplanted to other Unix systems, but the init program itself determines that the characteristics of the different platforms have to make a lot of differences in the code. Portability requires a certain amount of work.

- Ueld 目前不支持命令行和配置文件中转义字符和引号

- Ueld 目前使用了多个信号处理各种 ueldctl 命令，更好的方案是像 telinit 那样

- Ueld 目前不支持带参数重启

- Ueld 目前不能优雅得与现有桌面环境进行整合
