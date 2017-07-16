# About System V Init service compat (Beta)

> WARNING: This feature is currently in Beta stage, please use your own risk assessment before use

**The test system is Debian Stretch**

System V Init service compat mode starts the sysvinit-style service at boot time (/etc/init.d/xxx)

1. Install sysvinit first

	```
	$ sudo apt-get install sysvinit-core sysvinit-utils
	```

2. If you have previously installed ueld, you must delete the original configuration file

	```
	# mv /etc/ueld /etc/ueld.bak
	```

	or

	```
	# rm -r /etc/ueld
	```

3. Install ueld by 'make install_sysv_compat'

	```
	$ cd path-to-ueld-src
	$ ./mkconfig.sh
	$ make
	$ sudo make install_sysv_compat
	```

4. The runlevel

	in /etc/ueld/sysinit.sh, you can change runlevel.

	Currently using kernel parameters to set the runlevel is not supported, but later versions will have.

References:

http://without-systemd.org/wiki/index.php/How_to_remove_systemd_from_a_Debian_jessie/sid_installation (Retrieved at 2017.7.16)
