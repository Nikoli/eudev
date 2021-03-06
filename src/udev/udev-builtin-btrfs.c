/***
  This file is part of systemd.

  Copyright 2012 Kay Sievers <kay@vrfy.org>

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "udev.h"

#define BTRFS_PATH_NAME_MAX 4087
struct btrfs_ioctl_vol_args {
        int64_t fd;
        char name[BTRFS_PATH_NAME_MAX + 1];
};
#define BTRFS_IOCTL_MAGIC 0x94
#define BTRFS_IOC_DEVICES_READY _IOR(BTRFS_IOCTL_MAGIC, 39, struct btrfs_ioctl_vol_args)

static int builtin_btrfs(struct udev_device *dev, int argc, char *argv[], bool test)
{
        struct  btrfs_ioctl_vol_args args;
        int fd;
        int err;

        if (argc != 3 || !streq(argv[1], "ready"))
                return EXIT_FAILURE;

        fd = open("/dev/btrfs-control", O_RDWR);
        if (fd < 0)
                return EXIT_FAILURE;

        util_strscpy(args.name, sizeof(args.name), argv[2]);
        err = ioctl(fd, BTRFS_IOC_DEVICES_READY, &args);
        close(fd);
        if (err < 0)
                return EXIT_FAILURE;

        udev_builtin_add_property(dev, test, "ID_BTRFS_READY", err == 0 ? "1" : "0");
        return EXIT_SUCCESS;
}

const struct udev_builtin udev_builtin_btrfs = {
        .name = "btrfs",
        .cmd = builtin_btrfs,
        .help = "btrfs volume management",
};
