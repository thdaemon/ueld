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
ueld_muti_init_other_init=/lib/system/systemd
ueld_muti_init_other_init_telinit=/lib/system/systemd
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

on Ubuntu, startx worked badly, so I write a simple display manager.

```
$ git clone https://gist.github.com/7db0df66171aae5855f9b84c97a4a3b7.git --depth=1
$ cd 7db0df66171aae5855f9b84c97a4a3b7
$ vi mydm.c

	change 'user' to your user name
	and use 'unity' replace 'startxfce4' if you use unity :-)
	if you use other DE, please change it!
 
$ gcc -Wall -O2 -std=c99 -D_POSIX_C_SOURCE=200819L -o mydm mydm.c -lX11
$ sudo su
[sudo] password for xxx: 
# mv mydm /opt/mydm
```

Add these to /etc/ueld/sysloaded.sh before `exit 0`

```
/opt/mydm &
```

**NOTE**

fonts may be very bad, but you can configure your fonts by fontconfig

It is a good webpage which can solve all problems: 

[Font configuration - Arch Wiki](https://wiki.archlinux.org/index.php/font_configuration)

### The others

If you need, you can start any program in ueld's script, such as loading some drivers, syslogd, sshd, etc.
