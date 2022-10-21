# s3fs-fuse-oidc-vault-minio-lib

Authentication module using [indigo-dc/oidc-agent](https://github.com/indigo-dc/oidc-agent) and [Hashicorp Vault](https://github.com/hashicorp/vault) for [s3fs-fuse](https://github.com/s3fs-fuse/s3fs-fuse/) to locally mount [MinIO](https://github.com/minio/minio) buckets.

## Overview
`s3fs-fuse-oidc-vault-minio-lib` is a shared library that performs credential processing of s3fs-fuse.
This shared library can be specified with the option (`credlib` and `credlib_opts`) of [ggtakec/s3fs-fuse](https://github.com/ggtakec/s3fs-fuse/tree/extcred_mod) and works by replacing the built-in credential processing of s3fs-fuse.
It makes use of `oidc-agent` C++ API in order to get an OIDC access token from IAM and get the client authenticated and authorized by `vault`; then, using `vault` C++ API, it obtains S3 temporary credentials from MinIO to mount buckets with s3fs-fuse. 

## Usage
You can easily build and use `s3fs-fuse-oidc-vault-minio-lib` by following the steps below.

### Build

#### Install oidc-agent C++ API by apt-get on Ubuntu22.04
```
$ sudo apt-get install liboidc-agent-dev oidc-agent
```

#### Build and Install Hashicorp Vault C++ API on Ubuntu22.04
```
$ sudo apt-get install libcurl4-openssl-dev
$ git clone https://github.com/abedra/libvault.git
$ cd libvault
$ cmake -S . -B build
$ cmake --build build
$ cd build
$ make
$ sudo make install
```

#### Build and Install nlohmann/json on Ubuntu22.04
```
$ git clone https://github.com/nlohmann/json.git
$ cd json
$ cmake -S . -B build
$ cmake --build build
$ cd build
$ make
$ sudo make install
```

#### Build s3fs-fuse and s3fs-fuse-oidc-vault-minio-lib
```
$ sudo apt-get install autotools-dev automake build-essential libxml2-dev pkg-config libssl-dev libfuse-dev fuse
$ git clone git@baltig.infn.it:fornari/s3fs-fuse-oidc-vault-minio-lib.git
$ cd s3fs-fuse-oidc-vault-minio-lib
$ cmake -S . -B build
$ cd build
$ sudo make install
```
After that, you can find `liboidc-vault-minio.so` in `build` sub directory.

### Run s3fs-fuse
You can set a profile to be sourced before s3fs-fuse execution in order to configure your environment.
For example:
```
$ cat oidc-vault-minio-profile 
OIDC_CLIENT_NAME=<your_oidc_client_name>
VAULT_HOST=vault.example.com
VAULT_PORT=443
VAULT_ROLE=<your_organization_name>
VAULT_TLS_ENABLE=true
VAULT_TLS_VERIFY=true
MINIO_HOST=minio.example.com
```
Then, you can run s3fs-fuse in the following way:
```
$ source oidc-vault-minio-profile
$ s3fs <bucket> <mountpoint> <options...> -o credlib=liboidc-vault-minio.so -o credlib_opts=Off
```
To specify this `s3fs-fuse-oidc-vault-minio-lib` for s3fs, use the following options:

#### credlib
An option to specify the `s3fs-fuse-oidc-vault-minio-lib` library.
You can specify only the library name or the path to the library file.
The s3fs use `dlopen` to search for the specified `s3fs-fuse-oidc-vault-minio-lib` library and load it.

Example:
```
-o credlib=liboidc-vault-minio.so
```

#### credlib_opts
Specifies the options provided by `s3fs-fuse-oidc-vault-minio-lib`.
If you specify `s3fs-fuse-oidc-vault-minio-lib`, you can specify the output level of the debug message shown below for this option:
- Off
- Error
- Warn
- Info
- Debug

Example:
```
-o credlib_opts=Info
```
