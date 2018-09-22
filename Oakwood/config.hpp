#pragma once
struct _GlobalConfig {
	std::string server_address;
	std::string username;
} GlobalConfig;

constexpr const char* config_file_name = "multiplayer.json";
inline auto config_get_default() ->nlohmann::json {
	return R"({
		"username": "Unknown",
		"ip": "localhost"
	})"_json;
}

inline auto config_get() -> void {

	nlohmann::json json_config;
	if (mod_file_exist(config_file_name)) {
		json_config = nlohmann::json::parse(mod_get_file_content(config_file_name));
		mod_log("Loading config");
	}
	else {
		json_config = config_get_default();
		std::ofstream o(config_file_name);
		o << std::setw(4) << json_config << std::endl;
		mod_log("Config doesent exists ! using default !");
	}

	GlobalConfig.server_address = json_config["ip"].get<std::string>();
	GlobalConfig.username = json_config["username"].get<std::string>();
}