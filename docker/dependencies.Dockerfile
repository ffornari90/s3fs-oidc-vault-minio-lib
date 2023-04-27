FROM ubuntu:22.04
RUN DEBIAN_FRONTEND=noninteractive apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    liboidc-agent-dev oidc-agent git cmake curl wget ca-certificates \
    libcurl4-openssl-dev autotools-dev automake build-essential gnupg \
    libxml2-dev pkg-config libssl-dev libfuse-dev fuse fio && \
    DEBIAN_FRONTEND=noninteractive apt-get clean
RUN curl "https://crt.sh/?d=2475254782" -o /usr/local/share/ca-certificates/geant-ov-rsa-ca.crt && \
    wget -q -O - "https://dist.eugridpma.info/distribution/igtf/current/GPG-KEY-EUGridPMA-RPM-3" | apt-key add - && \
    echo "deb http://repository.egi.eu/sw/production/cas/1/current egi-igtf core" > /etc/apt/sources.list.d/ca-repo.list && \
    DEBIAN_FRONTEND=noninteractive apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y ca-policy-egi-core && \
    DEBIAN_FRONTEND=noninteractive apt-get clean && \
    for f in `find /etc/grid-security/certificates -type f -name '*.pem'`; \
    do filename="${f##*/}"; cp $f /usr/local/share/ca-certificates/"${filename%.*}.crt"; done && \
    update-ca-certificates
RUN git clone https://github.com/abedra/libvault.git && \
    cd libvault && git checkout tags/0.51.0 && \
    cmake -S . -B build && cmake --build build && \
    cd build && make install && cd ../.. && \
    git clone https://github.com/nlohmann/json.git && \
    cd json && cmake -S . -B build && cmake --build build && \
    cd build && make install && cd ../.. && \
    rm -rf libvault json
