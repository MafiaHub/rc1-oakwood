#pragma once
#define EXPAND_VEC(VEC) {VEC.x, VEC.y, VEC.z}

inline auto mod_log(const char* msg) -> void {
	printf("[Oakwood MP] %s\n", msg);
}

inline auto mod_debug(const char* msg) -> void {
	printf("[DEBUG] %s\n", msg);
}

inline auto mod_get_file_content(std::string file_name) {
	std::ifstream ifs(file_name);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));

	return content;
}

inline auto mod_file_exist(std::string file_name) {
	std::ifstream infile(file_name);
	return infile.good();
}

