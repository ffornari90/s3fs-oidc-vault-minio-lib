# s3fs-fuse-oidc-vault-minio-lib

Authentication module using indigo-dc/oidc-agent and Hashicorp Vault for s3fs-fuse to locally mount MinIO buckets.

## Overview
`s3fs-fuse-oidc-vault-minio-lib` is a shared library that performs credential processing of [s3fs-fuse](https://github.com/s3fs-fuse/s3fs-fuse/).
This shared library can be specified with the option(`credlib` and `credlib_opts`) of [ggtakec/s3fs-fuse](https://github.com/ggtakec/s3fs-fuse/tree/extcred_mod) and works by replacing the built-in credential processing of s3fs-fuse.
This shared library makes use of `oidc-agent` C++ API in order to get an OIDC access token from IAM and get the client authenticated and authorized by `vault`; then, using `vault` C++ API it can obtain S3 temporary credentials fetched from MinIO to mount buckets with s3fs-fuse. 
