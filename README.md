## UELD ---- Usermode Environment Loader Daemon

Switch language: [简体中文](README.zh_CN.md)

Ueld (Usermode Environment Loader Daemon) is a lightweight init program under Unix/Linux environment, it can be used to replace SysVInit and Systemd

### Compile and install

```
$ cd path-to-ueld-src
$ ./mkconfig.sh
$ make
$ sudo make install
```

On FreeBSD, you need use `gmake` instead of `make`. Compiling ueld need GNU Make.

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

### Configure and use Ueld

After the installation, I suggest you configure Ueld immediately, refer to [this Wiki](doc/userguide.md)

**Ueld manual is also included in this wiki;** Uninstallation instructions are also included in this wiki.

If you first install Ueld on Linux, then the Ueld configuration file is set to create `getty(8)` on tty1-5 to wait for login, which may not be suitable for your situation, so do not forget to configure it. Noteworthy, when you install Ueld, if the configuration file in the corresponding directory is already exists, it will not overwrite the previous configuration file, even if the new version of the default configuration may be changed.

But on FreeBSD, the Ueld configuration file will run /etc/rc and then ctreate `getty`, so you may have no request to modify them.

### Install Examples

Now, we have some examples to show how to install ueld on your system!

[Install on Ubuntu 16.04.1](doc/install_on_ubuntu.md)

[Install on FreeBSD](doc/install_on_freebsd.md)

### TODO

- Ueld currently only supports Linux systems and FreeBSD. It is interesting to port it to other Unix systems. (Platform related code is placed in the os/ subdirectory. The init program itself determines that the characteristics of the different platforms have to make a lot of differences in the code.)

- Ueld currently does not support escaped characters and quotation marks in ueld.conf and respawn.list.

- Ueld currently dose not support reboot with parameter.

- Ueld could not integrate with the existing Desktop environment gracefully now. That need use dbus to get messages. But do not add the support into ueld, let strategy and mechanism separate.

- See the 'TODO' and 'FIXME' marks in code.
