FROM ubuntu:22.04
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN apt-get update \
 && apt-get install -y liboidc-agent-dev oidc-agent git cmake curl wget \
    libcurl4-openssl-dev autotools-dev automake build-essential \
    libxml2-dev pkg-config libssl-dev libfuse-dev fuse && \
    apt-get clean
RUN curl "https://ssl-tools.net/certificates/c2826e266d7405d34ef89762636ae4b36e86cb5e.pem" \
    -o /usr/local/share/ca-certificates/geant-ov-rsa-ca.crt && \
    wget -q -O - "https://dist.eugridpma.info/distribution/igtf/current/GPG-KEY-EUGridPMA-RPM-3" | apt-key add - && \
    echo "deb http://repository.egi.eu/sw/production/cas/1/current egi-igtf core" > /etc/apt/sources.list.d/ca-repo.list && \
    apt-get update && apt-get install -y ca-policy-egi-core && apt-get clean && \
    for f in `find /etc/grid-security/certificates -type f -name '*.pem'`; \
    do filename="${f##*/}"; cp $f /usr/local/share/ca-certificates/"${filename%.*}.crt"; done && \
    update-ca-certificates
RUN git clone https://github.com/abedra/libvault.git && \
    cd libvault && cmake -S . -B build && cmake --build build && \
    cd build && make install && cd ../.. && \
    git clone https://github.com/nlohmann/json.git && \
    cd json && cmake -S . -B build && cmake --build build && \
    cd build && make install && cd ../.. && \
    git clone https://baltig.infn.it/fornari/s3fs-fuse-oidc-vault-minio-lib.git && \
    cd s3fs-fuse-oidc-vault-minio-lib && cmake -S . -B build && \
    cd build && make install && cd ../.. && \
    rm -rf libvault json s3fs-fuse-oidc-vault-minio-lib
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
