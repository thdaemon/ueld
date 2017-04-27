## Generate config header

Run `mkconfig.sh` without args will generate a default config

```
$ cd path-to-ueld-src
$ ./mkconfig.sh
```

Now, ueld support these compiletime-config

Name|Default Value|Description
----|-------------|-----------
--no-build-in-minit|not define|Do not add build-in Muti-Init feature.
--term-waittime=value|2|The time (in seconds) to wait after send SIGTERM to all processes when poweroffing. After this time, ueld will send SIGKILL to all processes.
--manu-get-mntinfo|not define|(Only support Linux system) Manu get the mount infomation, not use the getmntent function. (This function may have a problem but is portable, see reboot.c)
--console-vt=value|1|When system poweroffing, let ueld change to this vt.(It is usually the vt number of /dev/console) **This config can be overrided by `ueld_console_vt` in ueld.conf**
--respawn-no-ign-fail-proc|not define|When ueld respawn a exited process, ueld will ignore the processes which exited by `_exit(EXIT_FAILURE)` if you do not define the config.

For example, if you need not Build-in Muti-Init, and want to wait 5 seconds for the exiting process, you can

```
$ ./mkconfig.sh --no-build-in-minit --term-waittime=5
```

