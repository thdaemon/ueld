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
The installation requires root user, so `sudo(8)` is used in this example. If you do not have `sudo(8)` in your system, use other methods to switch to Root.

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

If you first install Ueld, then the Ueld configuration file is set to create `getty(8)` on tty1-5 to wait for login, which may not be suitable for your situation, so do not forget to configure it. Noteworthy, when you install Ueld, if the configuration file in the corresponding directory is already exists, it will not overwrite the previous configuration file, even if the new version of the default configuration may be changed.

#### Advantages of Ueld

- Ueld is very lightweight and simple, "small is both beautiful". The huge software is always uncomfortable, so I try to avoid Ueld becoming a huge software.

- 我尽力使 Ueld 仅提供 init 该提供的一系列原语操作，尽力做到 “提供机制，而不是策略”。通过用户可编辑的脚本，将系统启动，关机时的具体策略交由不同的软件去完成，所有行为的最终逻辑可由用户根据他们自己的需要制定，使其具有很强的灵活性。

- Ueld particularly suitable for devices such as Raspberry Pi, or a people who care about speed. In fact, I develop ueld in a "class Raspberry Pi" board ---- NanoPi 2Fire. The device use the Debian 8 system, with ueld replace systemd, the boot speed increased by 10-20 times! (Which made me surprised)

- The last one is a unique, in most cases no use, but sometimes may be quite useful small function, I call it "Muti Init". It allows you to configure Ueld as a "init loader" that you can choose to load which init program (ueld, sysvinit, or systemd, etc.) when the system is booting. (PC users may say that I can add some entries in grub, they use different init=xxx kernel boot parameters, but must mention that, some devices like NanoPi, it is a heavy work to brush u-boot to modify start parameter)

#### Ueld current question

- Ueld currently only supports Linux systems, however, most of the transplant layer code is placed in the os/ subdirectory, the other code also take into account the portability problem (such as BSD need to use `ioctl(2)` set the control terminal), it can be transplanted to other Unix systems, but the init program itself determines that the characteristics of the different platforms have to make a lot of differences in the code. Portability requires a certain amount of work.

- Ueld 目前不支持命令行和配置文件中转义字符和引号

- Ueld 目前使用了多个信号处理各种 ueldctl 命令，更好的方案是像 telinit 那样

- Ueld 目前不支持带参数重启

- Ueld 目前不能优雅得与现有桌面环境进行整合
