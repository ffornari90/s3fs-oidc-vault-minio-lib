FROM ffornari/s3fs-ovm-dependencies:latest
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN git clone https://baltig.infn.it/fornari/s3fs-fuse-oidc-vault-minio-lib.git && \
    cd s3fs-fuse-oidc-vault-minio-lib && cmake -S . -B build && \
    cd build && make install && cd ../.. && \
    rm -rf s3fs-fuse-oidc-vault-minio-lib
RUN adduser --disabled-password --gecos '' docker
USER docker
RUN mkdir -p $HOME/mnt/minio && \
    echo "access:secret" > $HOME/.passwd-s3fs && \
    chmod 600 $HOME/.passwd-s3fs
CMD eval `oidc-agent` && \
  oidc-gen && \
  s3fs bucket $HOME/mnt/minio \
  -o use_path_request_style \
  -o url=https://$MINIO_HOST \
  -o no_check_certificate \
  -o credlib=liboidc-vault-minio.so \
  -o credlib_opts=Info -f -d
