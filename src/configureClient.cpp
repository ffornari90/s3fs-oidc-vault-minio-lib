#include "main.hpp"

Vault::Client configureClient(Vault::Host host, Vault::Port port,
                              bool enableTLS, bool enableTLSverification, bool debug,
                              std::string role, std::string accountname,
                              struct agent_response& agent_res) {
  Vault::Config vaultConfig = Vault::ConfigBuilder().withHost(host).withPort(port).withTlsEnabled(enableTLS).withTlsVerification(enableTLSverification).withDebug(debug).build();
  Vault::HttpErrorCallback httpErrorCallback = [&](std::string err) {
    std::cout << err << std::endl;
  };
  Vault::ResponseErrorCallback responseCallback = [&](Vault::HttpResponse err) {
    std::cout << err.statusCode << " : " << err.body.value() << std::endl;
  };
  getOIDCToken(accountname, agent_res);
  Vault::JwtStrategy auth{Vault::RoleId{role}, Vault::Jwt{agent_res.token_response.token}};
  return Vault::Client{vaultConfig, auth, httpErrorCallback, responseCallback};
}
