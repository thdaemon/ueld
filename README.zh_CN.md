**警告** 中文版本需要更新翻译

查看 [英文版本](README.md)

## UELD ---- Usermode Environment Loader Daemon

Ueld (用户态环境加载器守护进程) 是一个轻量级的 Unix/Linux 环境下的 init 程序，可用于替换 SysVInit 和 Systemd

### 编译与安装

```
$ cd path-to-ueld-src
$ ./mkconfig.sh
$ make
$ sudo make install
```

On FreeBSD, you need use `gmake` instead of `make`. Compiling ueld need GNU Make.

安装需要 Root 用户，因此本例中使用了 `sudo(8)`，如果你的系统中没有 `sudo(8)`，请使用其他方法切换到 Root。

可使用 `PREFIX` 变量设置根文件系统，可使用 `INSTALLDIR` 变量设置安装目录。

> Note: When you use both `PREFIX` and `INSTALLDIR`, the install path is `${PREFIX}/${INSTALLDIR}`, in other word, `INSTALLDIR` should not include the path prefix.

**编译时配置**

Before you compile ueld, you need do 'compiletime config' by `./mkconfig.sh`, it will generate a config.h header which includes config macros. Run mkconfig.sh without args will make a header which includes default config.

For example, if you need not Muti-Init feature, you can

```
$ ./mkconfig.sh --no-build-in-minit
```

For more infomation, please see [this Wiki](doc/genconfig.md)

**交叉编译**

如果你希望交叉编译 Ueld，那么你可以使用 `CROSS` 变量设置交叉编译器，例如交叉编译器为 arm-linux-gnueabihf-，你可以

```
$ make CROSS=arm-linux-gnueabihf-
```

**Install for a linux system which does not use initramfs (or initrd).**

Some systems do not use initramfs. That means ueld should mount some filesystems by itself. To install ueld for these system, you need

```
$ sudo make install_no_initramfs
```

### 配置和使用 Ueld

在安装后建议立刻配置 Ueld，参考 [此 Wiki](doc/zh_CN/userguide.md)

**Ueld 使用手册也包含在这篇 Wiki 中；**卸载说明也包含在此 Wiki 中。

If you first install Ueld on Linux, then the Ueld configuration file is set to create `getty(8)` on tty1-5 to wait for login, which may not be suitable for your situation, so do not forget to configure it. Noteworthy, when you install Ueld, if the configuration file in the corresponding directory is already exists, it will not overwrite the previous configuration file, even if the new version of the default configuration may be changed.

But on FreeBSD, the Ueld configuration file will run /etc/rc and then ctreate `getty`, so you may have no request to modify them.

### Install Examples

Now, we have some examples to show how to install ueld on your system!

[Install on Ubuntu 16.04.1](doc/install_on_ubuntu.md)

[Install on FreeBSD](doc/install_on_freebsd.md)

### TODO

- Ueld currently only supports Linux systems and FreeBSD. It is interesting to port it to other Unix systems. (Platform related code is placed in the os/ subdirectory. The init program itself determines that the characteristics of the different platforms have to make a lot of differences in the code.)

- Ueld 目前不支持 ueld.conf 和 respawn.list 中的转义字符和引号

- Ueld 目前不支持带参数重启

- Ueld 目前不能优雅得与现有桌面环境进行整合. That need use dbus to get messages. But do not add the support into ueld, let strategy and mechanism separate.

- See the 'TODO' and 'FIXME' marks in code.
