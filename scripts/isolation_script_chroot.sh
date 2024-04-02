ROOTFS_PATH=containers/ubuntu
unshare --mount --pid --user --map-root-user --fork \
--mount-proc chroot $ROOTFS_PATH /bin/sh -c "mount -t proc proc /proc && bash"
