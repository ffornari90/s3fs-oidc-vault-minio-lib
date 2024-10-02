#include "main.hpp"

bool getOIDCToken(std::string accountname, std::string audience, struct agent_response& agent_res) {
  agent_res = getAgentTokenResponse(accountname.c_str(), 60, NULL, "s3fs-fuse", audience.c_str());
  auto result = true;
  if (agent_res.type == AGENT_RESPONSE_TYPE_ERROR) {
    oidcagent_printErrorResponse(agent_res.error_response);
    result = false;
  }
  return result;
}
