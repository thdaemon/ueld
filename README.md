## UELD ---- Usermode Environment Loader Daemon

Switch language: [简体中文](README.zh_CN.md)

Ueld (Usermode Environment Loader Daemon) is a lightweight init program under Unix/Linux environment, it can be used to replace SysVInit and Systemd

#### Compile and install

```
$ cd path-to-ueld-src
$ ./mkconfig.sh
$ make
$ sudo make install
```
The installation requires root user, so `sudo(8)` is used in this example. If you do not have `sudo(8)` in your system, use other methods to switch to Root.

You can use the `PREFIX` variable to set the root file system. You can use the` INSTALLDIR` variable to set the installation directory.

> Note: When you use both `PREFIX` and `INSTALLDIR`, the install path is `${PREFIX}/${INSTALLDIR}`, in other word, `INSTALLDIR` should not include the path prefix.

**Compiletime config**

Before you compile ueld, you need do 'compiletime config' by `./mkconfig.sh`, it will generate a config.h header which includes config macros. Run mkconfig.sh without args will make a header which includes default config.

For example, if you need not Muti-Init feature, you can

```
$ ./mkconfig.sh --no-build-in-minit
```

For more infomation, please see [this Wiki](doc/genconfig.md)

**Cross compile**

If you want to cross-compile Ueld, you can use the `CROSS` variable to set the cross compiler name, for example, if you want to use 'arm-linux-gnueabihf-' cross compiler, you can

```
$ make CROSS=arm-linux-gnueabihf-
```

**Install for a linux system which does not use initramfs (or initrd).**

Some systems do not use initramfs. That means ueld should mount some filesystems by itself. To install ueld for these system, you need

```
$ sudo make install_no_initramfs
```

**On FreeBSD, you need use `gmake` instead of `make`**

Compiling ueld need GNU Make.

#### Configure and use Ueld

After the installation, i suggest you configure Ueld immediately, refer to [this Wiki](doc/userguide.md)

**Ueld manual is also included in this wiki;** Uninstallation instructions are also included in this wiki.

If you first install Ueld, then the Ueld configuration file is set to create `getty(8)` on tty1-5 to wait for login, which may not be suitable for your situation, so do not forget to configure it. Noteworthy, when you install Ueld, if the configuration file in the corresponding directory is already exists, it will not overwrite the previous configuration file, even if the new version of the default configuration may be changed.

#### Install Examples

Now, we have some examles to show how to install ueld on your system!

[Install on Ubuntu 16.04.1](doc/install_on_ubuntu.md)

[Install on FreeBSD](doc/install_on_freebsd.md)

#### Advantages of Ueld

- Ueld is very lightweight and simple, "small is both beautiful". The huge software is always uncomfortable, so I try to avoid Ueld becoming a huge software.

- I try my best to let Ueld to only provide a series of primitive operations of init. and try my best to provide "the mechanism, not the strategy". Through scripts which are editable for users, The system start, shut down the specific strategy to complete the different software, The final logic of all actions can be formulate by users themselves to meet their own needs, so it has a strong flexibility.

- Ueld particularly suitable for devices such as Raspberry Pi, or a people who care about speed. In fact, I develop ueld in a "class Raspberry Pi" board ---- NanoPi 2Fire. The device use the Debian 8 system, with ueld replace systemd, the boot speed increased by 10-20 times! (Which made me surprised)

- The last one is a unique, in most cases no use, but sometimes may be quite useful small function, I call it "Muti Init". It allows you to configure Ueld as a "init loader" that you can choose to load which init program (ueld, sysvinit, or systemd, etc.) when the system is booting. (PC users may say that I can add some entries in grub, they use different init=xxx kernel boot parameters, but must mention that, some devices like NanoPi, it is a heavy work to brush u-boot to modify start parameter)

#### Ueld current question

- Ueld currently only supports Linux systems, however, most of the transplant layer code is placed in the os/ subdirectory, the other code also take into account the portability problem (such as BSD need to use `ioctl(2)` set the control terminal), it can be transplanted to other Unix systems, but the init program itself determines that the characteristics of the different platforms have to make a lot of differences in the code. Portability requires a certain amount of work.

- Ueld currently does not support escaped characters and quotation marks.

- Ueld currently uses many signal handler to handle ueldctl command, there is not a telinit fifo.

- Ueld currently dose not support reboot with parameter.

- Ueld could not integrate with the existing Desktop environment gracefully now.
 