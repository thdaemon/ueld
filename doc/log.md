## Ueld log system

### compile options

`--enable-log` - Whether to compile the log system

`--enable-log-file` - Whether to compile the log-to-file type

`--enable-log-syslog` - Whether to compile the log-to-syslog type

`--enable-os-klog` - Whether to compile the log-to-kernel-message type and the os support

### configure

In `/etc/ueld/ueld.conf`, you can set `ueld_log_target` to config log.

The format is `type:target`

`type` is the target type, ueld have these types:

1. none

	`none:discard` - discard all logs.

2. file

	`file:/path/to/file` - write log to a file. for example, `file:/var/log/ueld`

3. klog

	`klog:linux` - write log to kernel log buffer

4. syslog

	`syslog:ueld` - write log to syslog

	> You need start rsyslogd in `sysinit.sh`
