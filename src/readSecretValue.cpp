#include "main.hpp"
#include "Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

std::optional<std::string> readSecretValue(const Vault::Client &vaultClient, const Vault::Path path,
                                           const std::string mount, const Vault::KeyValue::Version version) {
  auto logger = Logger::GetInstance();
  if (vaultClient.is_authenticated()) {
    Vault::KeyValue keyValue{vaultClient, Vault::SecretMount{mount}, Vault::KeyValue::Version{version}};

    if (auto response = keyValue.read(path); response) {
      logger->Log(__FILE__, __LINE__, "Secrets read successfully at " + path, LogLevel::INFO);
      nlohmann::json data = nlohmann::json::parse(response.value());
      auto access_key = data["data"]["accessKeyId"].get<std::string>();
      auto secret_key = data["data"]["secretAccessKey"].get<std::string>();
      auto home = std::getenv("HOME") ? std::getenv("HOME") : "/tmp";
      std::string s3fs_credfile = "/.passwd-s3fs";
      std::ofstream ofs(home + s3fs_credfile, std::ofstream::trunc);
      ofs << access_key + ":" + secret_key + "\n";
      ofs.close();
      return response.value();
    }
    else
      logger->Log(__FILE__, __LINE__, "Unable to read secrets at " + path, LogLevel::INFO);
  }
  else
    logger->Log(__FILE__, __LINE__, "Unable to authenticate against Vault", LogLevel::INFO);

  return "";
}
