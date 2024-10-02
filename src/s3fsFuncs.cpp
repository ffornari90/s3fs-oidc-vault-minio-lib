#include "main.hpp"
#include "Logger.hpp"
#include "config.h"
#include <fstream>
#include <sstream>
#include <string.h>
#include <nlohmann/json.hpp>

bool string2bool (const std::string &v)
{
    return !v.empty() && !(strcasecmp(v.c_str(), "false") == 0 || strcasecmp(v.c_str(), "0") == 0);
}

const char* VersionS3fsCredential(bool detail)
{
        const char short_version_form[]  = "s3fs-oidc-vault-minio-lib : Version %s (%s)";
        const char detail_version_form[] =
                "s3fs-oidc-vault-minio-lib : Version %s (%s)\n"
                "s3fs-fuse credential I/F library for MinIO using OIDC Auth against Vault\n"
                "Copyright 2022 Federico Fornari <federico.fornari@cnaf.infn.it>\n";

        static char short_version_string[128];
        static char detail_version_string[256];
        static bool is_init = false;

        if(!is_init){
                is_init = true;
                sprintf(short_version_string, short_version_form, product_version, commit_hash_version);
                sprintf(detail_version_string, detail_version_form, product_version, commit_hash_version);
        }
        if(detail){
                return detail_version_string;
        }else{
                return short_version_string;
        }
}

bool InitS3fsCredential(const char* popts, char** pperrstr)
{
        if(pperrstr){
                *pperrstr = NULL;
        }

        //
        // Check option arguments and set it
        //
        auto logger = Logger::GetInstance();
        auto logOutput = logger->GetLogOutput("CONSOLE");
        LogLevel logLevel;
        if(popts && 0 < strlen(popts)){
		if(0 == strcasecmp(popts, "Off")){
                        logLevel = logger->GetLogLevel("NONE");
		}else if(0 == strcasecmp(popts, "Error")){
                        logLevel = logger->GetLogLevel("ERROR");
		}else if(0 == strcasecmp(popts, "Warn")){
                        logLevel = logger->GetLogLevel("WARN");
		}else if(0 == strcasecmp(popts, "Info")){
                        logLevel = logger->GetLogLevel("INFO");
		}else if(0 == strcasecmp(popts, "Debug")){
                        logLevel = logger->GetLogLevel("DEBUG");
		}else{
			if(pperrstr){
				*pperrstr = strdup("Unknown LogLevel option is specified.");
			}
			return false;
		}
	}

        //
        // Initalize
        //
        logger->SetLogPreferences("", logLevel, logOutput);

        return true;
}

bool FreeS3fsCredential(char** pperrstr)
{
        if(pperrstr){
                *pperrstr = NULL;
        }

        //
        // Restore placeholder s3fs credentials
        //
        auto home = std::getenv("HOME") ? std::getenv("HOME") : "/tmp";
        std::string s3fs_credfile = "/.passwd-s3fs";
        std::ofstream ofs(home + s3fs_credfile, std::ofstream::trunc);
        ofs << "access:secret\n";
        ofs.close();

        return true;
}

bool UpdateS3fsCredential(char** ppaccess_key_id, char** ppserect_access_key, char** ppaccess_token, long long* ptoken_expire, char** pperrstr)
{
        auto logger = Logger::GetInstance();

        if(!ppaccess_key_id || !ppserect_access_key || !ppaccess_token || !ptoken_expire){
                if(pperrstr){
                        *pperrstr = strdup("Some parameters are wrong(NULL).");
                }
                return false;
        }
        if(pperrstr){
                *pperrstr = NULL;
        }

        struct agent_response agent_res = (struct agent_response){0,{(struct token_response){NULL, NULL, 0}}};

        auto vault_host = std::getenv("VAULT_HOST") ? std::getenv("VAULT_HOST") : "localhost";
        auto vault_port = std::getenv("VAULT_PORT") ? std::getenv("VAULT_PORT") : "8200";
        auto vault_role = std::getenv("VAULT_ROLE") ? std::getenv("VAULT_ROLE") : "";
        auto vault_tls_enable = std::getenv("VAULT_TLS_ENABLE") ? std::getenv("VAULT_TLS_ENABLE") : "true";
        auto vault_tls_verify = std::getenv("VAULT_TLS_VERIFY") ? std::getenv("VAULT_TLS_VERIFY") : "true";
        auto oidc_client_name = std::getenv("OIDC_CLIENT_NAME") ? std::getenv("OIDC_CLIENT_NAME") : "";
        auto audience = std::getenv("AUDIENCE") ? std::getenv("AUDIENCE") : "";

        auto logLevel = logger->ShowLogLevel();
        auto vault_debug = (logLevel == LogLevel::DEBUG) ? true : false;

        bool vault_tls_enable_bool = string2bool(vault_tls_enable);
        bool vault_tls_verify_bool = string2bool(vault_tls_verify);

        // Get credentials
        auto vaultClient = configureClient(Vault::Host{vault_host}, Vault::Port{vault_port},
                                           vault_tls_enable_bool, vault_tls_verify_bool,
                                           vault_debug, vault_role, oidc_client_name, audience, agent_res);
        auto minioCreds = readSecretValue(vaultClient, Vault::Path{vault_role});

        if(!minioCreds.value().empty()){
          nlohmann::json data = nlohmann::json::parse(minioCreds.value());
          auto accessKeyId = data["data"]["accessKeyId"].get<std::string>();
          auto secretKey = data["data"]["secretAccessKey"].get<std::string>();

          // Set result buffers
          *ppaccess_key_id        = strdup(accessKeyId.c_str());
          *ppserect_access_key    = strdup(secretKey.c_str());
          *ppaccess_token         = strdup(agent_res.token_response.token);
          *ptoken_expire          = agent_res.token_response.expires_at;

          std::stringstream stream;
          stream << *ppaccess_token;
          std::string token_to_string;
          token_to_string = stream.str();

          stream.str(std::string());
          stream << *ptoken_expire;
          std::string expiration_to_string;
          expiration_to_string = stream.str();

          // For debug
          logger->Log(__FILE__, __LINE__, "[s3fsoidcvaultminiocred] : Access Key Id = " + accessKeyId, LogLevel::DEBUG);
          logger->Log(__FILE__, __LINE__, "[s3fsoidcvaultminiocred] : Secret Key    = " + secretKey, LogLevel::DEBUG);
          logger->Log(__FILE__, __LINE__, "[s3fsoidcvaultminiocred] : Access Token  = " + token_to_string, LogLevel::DEBUG);
          logger->Log(__FILE__, __LINE__, "[s3fsoidcvaultminiocred] : expiration    = " + expiration_to_string, LogLevel::DEBUG);
        }

        auto result = true;

        if(!*ppaccess_key_id || !*ppserect_access_key || !*ppaccess_token){
                if(pperrstr){
                        *pperrstr = strdup("Could not allocate memory.");
                }
                if(!*ppaccess_key_id){
                        free(*ppaccess_key_id);
                        *ppaccess_key_id = NULL;
                }
                if(!*ppserect_access_key){
                        free(*ppserect_access_key);
                        *ppserect_access_key = NULL;
                }
                if(!*ppaccess_token){
                        free(*ppaccess_token);
                        *ppaccess_token = NULL;
                }
                *ptoken_expire = 0;

                result = false;
        }

        secFreeAgentResponse(agent_res);

        return result;
}
