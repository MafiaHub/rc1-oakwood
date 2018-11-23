#pragma once
#ifdef _WIN32
class KeyToggle {
public:
	KeyToggle(int key) :mKey(key), mActive(false) {}
	operator bool() {
		if (GetAsyncKeyState(mKey)) {
			if (!mActive) {
				mActive = true;
				return true;
			}
		}
		else
			mActive = false;
		return false;
	}
private:
	int mKey;
	bool mActive;
};

inline auto alloc_console() {
	AllocConsole();
	std::setlocale(LC_ALL, "C");
	SetConsoleOutputCP(CP_UTF8);
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}
#endif

inline auto split(std::string s, std::string delimiter){
    std::vector<std::string> list;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return list;
}

static zpl_vec3 ComputeDirVector(float angle) {
	zpl_vec3 dir = { 0 };
	dir.x = (float)(::sin(zpl_to_radians(angle)));
	dir.z = (float)(::cos(zpl_to_radians(angle)));
	return dir;
}

static float DirToRotation180(zpl_vec3 dir) {
	return zpl_to_degrees(::atan2f(dir.x, dir.z));
}

static float DirToRotation360(zpl_vec3 dir) {
	auto val = zpl_to_degrees(::atan2f(dir.x, dir.z));

	if (val < 0) {
		val += 360.0f;
	}

	return val;
}