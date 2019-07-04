#pragma once

#define EXPAND_VEC(VEC) {VEC.x, VEC.y, VEC.z}
#define EXPLODE_VEC(VEC) VEC.x, VEC.y, VEC.z

inline auto split(std::string str, std::string token) {
    std::vector<std::string> result;
    while (str.size()) {
        int index = str.find(token);
        if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

#include <ostream>
#include <sstream>
#include <iterator>

static std::string implode(const std::vector<std::string>& parts, int index=0, const char *sep=" ") {
    switch (parts.size())
    {
    case 0:
        return "";
    
    case 1:
        return parts[0];
    
    default:
        std::ostringstream os;
        std::copy(parts.begin()+index, parts.end()-1, std::ostream_iterator<std::string>(os, sep));
        os << *parts.rbegin();
        return os.str();
    }
    
    std::ostringstream imploded;
    
    return imploded.str();
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

static int StringToInteger(std::string text) {
    try {
        return std::stoi(text);
    }
    catch (...) {
        return -1;
    }

    return -1;
}

static u64 StringToLong(std::string text) {
    try {
        return std::stoll(text);
    }
    catch (...) {
        return -1;
    }

    return -1;
}

static float StringToReal(std::string text)
{
    try
    {
        return std::stof(text);
    }
    catch (...)
    {
        return -1;
    }

    return -1;
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

            while (zpl_char_is_hex_digit(*e) && hex_col[5] == 0) {
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

static std::vector<std::string> SplitStringByWhitespace(const std::string &subject)
{
    std::istringstream ss{subject};
    using StrIt = std::istream_iterator<std::string>;
    std::vector<std::string> container{StrIt{ss}, StrIt{}};
    return container;
}


inline auto mod_log(const char* msg) -> void {
#ifdef OAKWOOD_SERVER
    console::printf("[Oakwood MP] %s\n", msg);
#else
    printf("[Oakwood MP] %s\n", msg);
#endif
}

inline auto mod_log(std::string msg) -> void {
#ifdef OAKWOOD_SERVER
    console::printf("[Oakwood MP] %s\n", msg.c_str());
#else
    printf("[Oakwood MP] %s\n", msg.c_str());
#endif
}

inline auto mod_debug(const char* msg) -> void {
#ifdef OAKWOOD_SERVER
    console::printf("[DEBUG] %s\n", msg);
#else
    printf("[DEBUG] %s\n", msg);
#endif
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
