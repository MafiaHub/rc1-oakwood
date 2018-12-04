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

class KeyHeld {
public:
    KeyHeld(int key) :mKey(key) {}
    operator bool() {
        return GetAsyncKeyState(mKey);
    }
private:
    int mKey;
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

#include <sstream>

static void html_encode_string(std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    data.swap(buffer);
}

static std::string ConvertColoredString(std::string text) {
    std::stringstream output;
    
    #ifndef OAK_CHAT_DISABLE_STYLING
    b32 following_style = true;
    static std::string begin_style = "<p style=\"color: #";
    static std::string enclose_style = "\">";
    static std::string end_style = "</p>";

    html_encode_string(text);
    #endif

    char *p = (char *)text.c_str();

    while (*p) {
        if (*p == '\\' && *(p+1) == '#') {
            output << "#";
            p += 2;
        }
        else if (*p != '#') {
            output << *p++;
        }
        else {
            // handle hex string
            char *b = p+1;
            char *e = b;
            char hex_col[7] = {0};
            char *hexp = hex_col;

            while (zpl_char_is_hex_digit(*e) && hex_col[5] == NULL) {
                *hexp++ = *e++;
            }

            #ifndef OAK_CHAT_DISABLE_STYLING
            if (following_style) {
                output << end_style;
            }
            
            following_style = true;
            output << begin_style << std::string(hex_col) << enclose_style;
            #endif

            p = e;
        }
    }
    return output.str();
}

#include <iterator>

static std::vector<std::string> SplitStringByNewline(const std::string &subject)
{
    std::istringstream ss{subject};
    using StrIt = std::istream_iterator<std::string>;
    std::vector<std::string> container{StrIt{ss}, StrIt{}};
    return container;
}
