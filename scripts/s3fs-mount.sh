#!/bin/bash
ROOTDIR=$(git rev-parse --show-toplevel)
FILE="${ROOTDIR}/oidc-vault-minio-profile"
if [ -f "$FILE" ]; then
  export $(grep -v '^#' $FILE | xargs -d '\n')
fi
echo 'access:secret' > $HOME/.passwd-s3fs
chmod 600 $HOME/.passwd-s3fs
mkdir -p $HOME/mnt/minio
s3fs bucket $HOME/mnt/minio \
  -o use_path_request_style \
  -o url=https://$MINIO_HOST \
  -o no_check_certificate \
  -o credlib=liboidc-vault-minio.so \
  -o credlib_opts=Info -f
