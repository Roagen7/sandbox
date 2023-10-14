steps to make a container 
1. unshare --pid -mount-proc --mount --fork --user --map-root-user bash
2. mount -t proc proc /proc
3. mount --rbind dockerfs/ dockerfs/
5. mount -t proc proc /proc
6. mkdir oldroot
7. pivot_root . oldroot
8. mount -t proc proc /proc
9. umount -l oldroot
voilla
