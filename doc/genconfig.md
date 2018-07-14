## Generate config header

Before you compile ueld, you need do 'compiletime config' by `./mkconfig.sh`, it will generate a config header which includes config macros. Run `mkconfig.sh` without args will make a config header which includes blank config. Run `make defconfig` will make a default config header.

```
$ make defconfig       # default config
```

```
$ ./mkconfig           # blank config
```

Now, ueld support these compiletime-config

Name|Rollback Value|Description
----|--------------|-----------
--no-build-in-minit|not define|Do not add build-in Multi-Init feature.
--term-waittime=value|2|The time (in seconds) to wait after send SIGTERM to all processes when poweroffing. After this time, ueld will send SIGKILL to all processes.
--manu-get-mntinfo|not define|(Only support Linux system) Manu get the mount infomation, not use the getmntent function. (This function may have a problem but is portable, see reboot.c)
--console-vt=value|1|When system poweroffing, let ueld change to this vt.(It is usually the vt number of /dev/console) **This config can be overrided by `ueld_console_vt` in ueld.conf**
--respawn-no-ign-fail-proc|not define|When ueld respawn a exited process, ueld will ignore the processes which exited by `_exit(EXIT_FAILURE)` if you do not define the config.
--fbsd-unmountfs|not define|(Only support FreeBSD system) If you don't define it, ueld will not unmount filesystems before poweroff. Because FreeBSD may do it by kernel.
--enable-log|not define|Enable logging.
--enable-log-file|not define|Depends on `--enable-log`, enable log to file feature
--enable-log-syslog|not define|Depends on `--enable-log`, enable log to syslog feature
--enable-os-klog|not define|Depends on `--enable-log`, enable log to kernel log feature

For example, if you need not Build-in Multi-Init, and want to wait 5 seconds for the exiting process, and only need to log to file, you can

```
$ ./mkconfig.sh --no-build-in-minit --term-waittime=5 --enable-log --enable-log-file
```

