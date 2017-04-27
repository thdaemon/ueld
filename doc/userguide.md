### Ueld User Manual Wiki

Switch language: [简体中文](zh_CN/userguide.md)

Ueld Version: 0.36

#### programs and tools

> PREFIX defaults to blank
>
> INSTALLDIR defaults to /usr/lib/ueld/ueld-${UELD_VERSION}

${PRIFIX}${INSTALLDIR}/ueld

${PRIFIX}${INSTALLDIR}/ueldctl

symlinks:

${PRIFIX}/sbin/init -> ${PRIFIX}${INSTALLDIR}/ueld

$(PRIFIX}/sbin/ueldctl -> ${PRIFIX}${INSTALLDIR}/ueldctl

**ueld** Ueld init program, used as the pid = 1 process after a system start.

**ueldctl** Utilitie that control the Ueld process are described in the section "Ueldctl Utilities".

#### Configuration files and scripts

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

Ueld configuration file, format:

```
name=value
```

'=' On both sides does not allow spaces, 'value' on both sides does not allow quotes

Name|Default Value|Description
----|-------------|-----------
ueld_execfile|/sbin/init|Ueld executable file path, when using `ueldctl -l` ueld process exec the program, does not allow relative path
ueld_must_remount_before_poweroff|0|Require Ueld to unmount (or re-mount as read-only) all file systems before disconnecting the machine from power, otherwise stop the shutdown process and drop to a shell
system_shell|/bin/sh|Set the system shell program, do not allow relative path
ueld_enable_muti_init|0|Whether to enable the Muti Init feature
ueld_muti_init_other_init|blank|When ueld_enable_muti_init is 1 (when Muti Init is enabled), it is the absolute path of another init program, such as /bin/systemd
ueld_muti_init_other_init_telinit|blank|When ueld_enable_muti_init is 1 (when Muti Init is enabled), it is the absolute path of the telinit program of another init package, such as /bin/systemd
ueld_muti_init_choose_time_out|3|When ueld_enable_muti_init is 1 (when Muti Init is enabled), the maximum number of seconds to wait for the init program to be selected at boot time, and select to load ueld when it times out
ueld_muti_init_press_keys|Ss|When ueld_enable_muti_init is 1 (when Muti Init is enabled), in the choice, the hotkey to let ueld load another init program, allowing the same time specify multiple hotkeys, case-sensitive
ueld_path_var|blank|If it is not blank, ueld will set it to `$PATH` when `$PATH` is blank. If you want to force override `$PATH`, use ueld_override_path_var
ueld_override_path_var|0|When it is 1, ueld will force override `$PATH` to ueld_path_var
ueld_console_vt|CONFIG_CONSOLE_VT or 1|see [genconfig.md](genconfig.md)

Note: The "default" value in the table above means that if the ueld.conf file does not include this item, ueld uses the value, rather than the direct copy of the ueld.conf file configuration when install Ueld!

**/etc/ueld/sysinit.sh**

System initialization script, Ueld runs directly the script at startup

Ueld will wait for the script to finish executing and then continue running, the script will get ueld standard input standard output and standard error

Suitable for the work done in this script: Start the `udevd (8)` daemon; Use `mount (8)` mount the file system; use `modprobe (8)` to load the kernel module; use `ifconfig (8)` etc to configure the network interface; start the system services which you need.

An example is copied at the time of installation, which starts the `udevd (8)` daemon.

**/etc/ueld/sysloaded.sh**

The script will be executed after the system was loaded

Ueld will run the script  in the background and does not wait it, the script's standard input standard output and standard error redirect to /dev/null

Suitable for the work done in this script: Start X Server (X Window System); start some utilities

An example will be copied at the time of installation, which demonstrates how to start the X Server, but the command is commented because not everyone is willing to open the X Server as soon as the system starts, and you can simply modify and cancel the comment if you need it.

**/etc/ueld/syshalt.sh**

Scripts that are run before the system shuts down

Ueld will wait for the script to finish executing and continue running, the script inherits ueld standard input standard output and standard error

In the installation will copy a example, generally do not need to change, if you need, tou can read the comments in the script

**/etc/ueld/respawn.list**

Ueld will analysis the file after completed to run the sysinit.sh, format:

```
Vt number:command line
...
```

Ueld will execute the command line in each row and create it on the virtual console you specify (set the control terminal to that vt). If the vt number is 0 or blank, the control terminal is not set. After the specified command line is exited, ueld will respawn the command line if it is not a process that exits with  _exit (EXIT_FAILURE), such as normal exited or received a signal.

If you define `CONFIG_NO_IGN_FAIL_PROC` (you can do it by mkconfig.sh), ueld will always respawn all process.

An example is copied at the time of installation, which starts `getty (8)` on tty1-5, as follows

```
1:getty tty1
2:getty tty2
3:getty tty3
4:getty tty4
5:getty tty5
```

**/etc/ueld/ctrlaltdel.sh**

The script that is run when the user presses Ctrl-Alt-Del on the virtual console

Ueld will wait for the script to finish executing and continue running, the script inherits ueld standard input standard output and standard error

Note: When some application like X Server is running, the keyboard is in Raw mode, so the Linux kernel's vt driver does not handle the key, so ueld will not be notified

#### ueldctl utility

`ueldctl -p` or `ueldctl --poweroff` shutdown

`ueldctl -r` or `ueldctl --reboot` reboot the system

`ueldctl -l` or `ueldctl --reload-init` reload the init program

`ueldctl -h` or `ueldctl --help` show usage

There is also a special use, you can create a symbolic link to ueldctl, the name is poweroff without parameters run equivalent to `ueldctl -p`, reboot same

#### Uninstall Ueld

1. Delete the installation directory set by installing Ueld (default is ${PREFIX}/usr/local/sbin/ueld/ueld-${UELD_VERSION}), and the configuration files in ${PREFIX}/etc/ueld/ directory

```
# rm -r $ {PREFIX}/etc/ueld
# rm -r where-you-install-ueld
```

2. Delete the soft link /sbin/init and /sbin/ueldctl, if necessary, and set /sbin/init as a soft link to the new init program

```
# rm /sbin/init/sbin/ueldctl
# ln -s the-path-of-new-init /sbin/init
```

3. (If you are uninstalling Ueld on the machine for the currently running system, then uninstalling Ueld will cause this time to shut down difficult, but there is no better option, one way is to shut down directly

```
# kill -TERM 1
```

Since the ueldctl utility has been removed, we have to use the `kill(1)` command to send the signal to ueld.

But this will lead to the next boot time `fsck(8)` utility check the file system, because delete ueld in filesystem, ueld process (pid 1) can not exit or load other programs, making ueld where the file system can not be unloaded or re-mounted to read only. (Ueld will call `sync(2)` system call to synchronize the file system when poweroff, it will not lead to data loss)

Another way is to let ueld load an other init program, because the configuration file has been deleted, so ueld use the default configuration /sbin/init, to ensure that he is available after the implementation of

```
# ls -l /sbin/init
lrwxrwxrwx 1 root root 20 Jan 7 12:14 /sbin/init -> /lib/systemd/systemd
# kill -USR1 1
```

But the cost of this approach is that the new init program will start the system service, so the best way is to turn off everything that should be closed and execute on a virtual console that is unlikely to be used like tty8.

If you are uninstalling Ueld for another system, for example, the situation is the root file system prepared by the embedded device, then this step is not required.
