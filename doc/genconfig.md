## Generate config header

Run `mkconfig.sh` without args will generate a default config

```
$ cd path-to-ueld-src
$ ./mkconfig.sh
```

Now, ueld support these compiletime-config

Name|Description
----|-----------
--no-build-in-minit|Do not add build-in Muti-Init feature.
--term-waittime=value|The time (in seconds) to wait after send SIGTERM to all processes when poweroffing. After this time, ueld will send SIGKILL to all processes. (Default 2)
--manu-get-mntinfo|(Only support Linux system) Manu get the mount infomation, not use the getmntent function.

For example, if you need not Build-in Muti-Init, and want to wait 5 seconds for the exiting process, you can

```
$ ./mkconfig.sh --no-build-in-minit --term-waittime=5
```

