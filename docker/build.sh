#!/bin/bash
docker build -t ffornari/s3fs-ovm-dependencies -f dependencies.Dockerfile .
docker build -t ffornari/s3fs-oidc-vault-minio -f Dockerfile .
