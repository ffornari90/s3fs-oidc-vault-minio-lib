#pragma once
#include <iostream>
#include <libvault/VaultClient.h>
extern "C" {
#include "/usr/include/oidc-agent/api.h"
}

Vault::Client configureClient(Vault::Host host, Vault::Port port,
                              bool enableTLS, bool enableTLSverification, bool debug,
                              std::string role, std::string accountname, struct agent_response &agent_res);

std::optional<std::string> readSecretValue(const Vault::Client &vaultClient, const Vault::Path path,
                                           const std::string mount = "minio/keys",
                                           const Vault::KeyValue::Version version = Vault::KeyValue::Version::v1);

bool getOIDCToken(std::string accountname, struct agent_response &agent_res);

extern "C" {
extern const char* VersionS3fsCredential(bool detail);
extern bool InitS3fsCredential(const char* popts, char** pperrstr);
extern bool FreeS3fsCredential(char** pperrstr);
extern bool UpdateS3fsCredential(char** ppaccess_key_id, char** ppserect_access_key, char** ppaccess_token, long long* ptoken_expire, char** pperrstr);
}
