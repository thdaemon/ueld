# Example: Install Ueld on FreeBSD

Hardware is a x86 PC

FreeBSD is 11.0, other versions should be ok.

> Install need some software, but ueld do not need them, ueld only require libc.
>
> git - fetch ueld source   
> GNU Make - compile ueld   
> basic Unix program like cat, expr, cp, ln, etc.  

## Get source, compile and install

```
$ git clone https://github.com/thdaemon/ueld.git --depth=1
$ cd ueld
$ ./mkconfig.sh --no-build-in-minit
$ gmake
$ su
Password:
# gmake install
```
Multi-init feature can not work on FreeBSD, but you can build-in it, it won't cause ueld crash, but it will improve the size of the binary.

> On FreeBSD, compiler may give some warning now.  
> Because ueld use some gnu ext like strchrnul

## Change config file

The default config is not fully fit FreeBSD, because sysinit.sh will run /etc/rc. If you want to customize the details manually, you can see

[this Wiki](userguide.md)

and

[Install on Ubuntu 16.04.1](install_on_ubuntu.md)
