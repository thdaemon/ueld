# Example: Install Ueld on Ubuntu 16.04.1

Hardware is a x86 PC

> Install need some software, but ueld do not need them, ueld only require glibc.
>
> git - fetch ueld source   
> make gcc - compile ueld   
> basic Unix program like cat, expr, cp, ln, etc.  

## Get source, compile and install

```
$ git clone https://github.com/thdaemon/ueld.git --depth=1
$ cd ueld
$ ./mkconfig.sh
$ make && sudo make install
```

> On Ubuntu, compiler may give two stupid warning. It is no problem.  
> But on Debian it should not have any warning!

## Change config file

The default config is not fully fit Ubuntu, we need fix it.

### Muti-Init ?

Do you need muti-init feature? If you worry that system can not start after replace systemd by ueld, you can enable muti init. It will ask you which init will be loaded when starting system.

If you do not need, ignore this!

```
# cd /etc/ueld
# vi ueld.conf
```

> My editor is vi, you can use the editor you like, such as nano

Change 

```
ueld_enable_muti_init=0
ueld_muti_init_other_init=
ueld_muti_init_other_init_telinit=
```

to

```
ueld_enable_muti_init=1
ueld_muti_init_other_init=/lib/systemd/systemd
ueld_muti_init_other_init_telinit=/lib/systemd/systemd
```

### Fixup udevd name

The name of udevd on Ubuntu is /lib/systemd/systemd-udevd, we need use the name to run udev daemon

**Method 1**

Change the sysinit.sh file.

```
# cd /etc/ueld
# vi sysinit.sh
```

Change `udevd --daemon` to `/lib/systemd/systemd-udevd --daemon`

**Method 2**

Create a symlink `/sbin/udevd` to `/lib/systemd/systemd-udevd`

```
# ls -l /sbin/udevd
ls: cannot access /sbin/udevd: No such file or directory
# ln -s /lib/systemd/systemd-udevd /sbin/udevd
# ls -l /sbin/udevd
lrwxrwxrwx 1 root root 26 XXX X XX:XX /sbin/udevd -> /lib/systemd/systemd-udevd
```

### Remount root file system to read-write, and hostname problem

Ubuntu left a read-only root file system (because the 'ro' in the kernel command line), we need re-mount it.

We will create a `ubuntu_init.sh` file to avoid edit `sysinit.sh` directly.

```
# cd /etc/ueld
# vi sysinit.sh
```

Add these in sysinit.sh before `exit 0`

```
[ -x /etc/ueld/ubuntu_init.sh ] && /etc/ueld/ubuntu_init.sh
```

And create it. Edit it.

```
# cd /etc/ueld
# touch ubuntu_init.sh
# chmod +x ubuntu_init.sh
# vi ubuntu_init.sh
```

Add these

```
#!/bin/sh

mount -o rw,remount /
hostname `cat /etc/hostname`
```

hostname command will set local hostname to the content in /etc/hostname, if you won't run it, hostname will keep null

### Network Interface

Ubuntu's eth0 is renamed to other, you can get it by `ifconfig -a` to get the name, and Add it in /etc/ueld/sysloaded.sh

The example will enable the interface and try to get ip, getway, netmask, etc

Must after prepare resolv!

```
/sbin/ifconfig XXX up
dhclient XXX &
echo 'nameserver 8.8.8.8' > /etc/resolv.conf
```

### The X Window

on Ubuntu 16.04, startx worked badly (a user who isn't root use startx will catch a error which may is threw by OpenGL), so I write a simple display manager to start X.

```
$ git clone https://github.com/thdaemon/mydm --depth=1
$ cd mydm
$ make
$ sudo make install
```

Add these to /etc/ueld/sysloaded.sh before `exit 0`

```
/opt/mydm -u user_name -c unity &
```

user_name is your user's name, unity is your de, other DEs and WMs like:

DE/WM|Start Command
-----|-------------
XFCE4|startxfce4
LXDE|startlxde
Gnome3|gnome-session
KDE|startkde4, etc.
Unity|unity
Openbox|openbox
i3|i3wm

For more usages, run `/opt/mydm -h`, for example, if you want to run fcitx and some programs after start x client, you can

Firstly, create a script

```
# cat > /opt/myde_autostart.sh << EOF
#!/bin/sh

fcitx &
other-programs &
exit 0
EOF
# chmod +x /opt/myde_autostart.sh
```
Afert doing these, we can run mydm like this

```
# /opt/mydm -u user_name -c XXXX -r /opt/myde_autostart.sh
```

And you can also use it to run your own DE, for example, you can run a pancel and run a wm (metacity is a wm)

```
# /opt/mydm -u user_name -c pancel_name_or_start_script -r metacity
```

> Warning: mydm don't support `xauth` now, so it only fit personal user like Desktop PC. If it is required that many users login your system in a same time, do not use it!

**NOTE**

fonts may be very bad, but you can configure your fonts by fontconfig

It is a good webpage which can solve all problems: 

[Font configuration - Arch Wiki](https://wiki.archlinux.org/index.php/font_configuration)

### The others

If you need, you can start any program in ueld's script, such as loading some drivers, syslogd, sshd, etc.
