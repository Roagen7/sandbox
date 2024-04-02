CONTAINER_DIR=containers
IMAGE=ubuntu
mkdir -p $CONTAINER_DIR
cd $CONTAINER_DIR
mkdir $IMAGE
docker export $(docker create $IMAGE) --output="image.tar"
tar -xf image.tar -C $IMAGE
mount --bind $IMAGE $IMAGE
unshare -m
#cd $IMAGE
#mkdir oldroot
#pivot_root . oldroot/
#cd /
#umount -l oldroot

