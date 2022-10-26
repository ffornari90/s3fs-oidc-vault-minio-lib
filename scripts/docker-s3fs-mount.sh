#!/bin/bash
ROOTDIR=$(git rev-parse --show-toplevel)
FILE="${ROOTDIR}/oidc-vault-minio-profile"
if [ -f "$FILE" ]; then
  docker run --name=s3fs --rm -ti \
           --device /dev/fuse \
           --cap-add SYS_ADMIN \
           --privileged \
           --env-file $FILE \
           ffornari/s3fs-oidc-vault-minio
fi
