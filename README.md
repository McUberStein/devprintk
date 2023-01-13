# devprintk
Print to the kernel buffer from userspace
# Usage
Inserting the module creates a file at `/dev/printk`. Writing to it will write to the kernel buffer.   
A simple program, `printk.c`, is included to simplify things.
# Module parameters
* `public=1/0` - Decides whether the device file will be accessible to non-root users.



Licensed under GPLv3
