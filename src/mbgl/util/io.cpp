#include <mbgl/util/io.hpp>

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef __GNUC__
#define MBGL_FOPEN_MODE_WBE "wbe"
#else
#define MBGL_FOPEN_MODE_WBE "wb"
#endif

namespace mbgl {
namespace util {

IOException::IOException(int err, const std::string& msg)
    : std::runtime_error(msg + ": " + std::strerror(errno)), code(err) {
}

void write_file(const std::string &filename, const std::string &data) {
    FILE *fd = fopen(filename.c_str(), MBGL_FOPEN_MODE_WBE);
    if (fd) {
        fwrite(data.data(), sizeof(std::string::value_type), data.size(), fd);
        fclose(fd);
    } else {
        throw std::runtime_error(std::string("Failed to open file ") + filename);
    }
}

std::string read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.good()) {
        std::stringstream data;
        data << file.rdbuf();
        return data.str();
    } else {
        throw std::runtime_error(std::string("Cannot read file ") + filename);
    }
}

std::optional<std::string> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.good()) {
        std::stringstream data;
        data << file.rdbuf();
        return data.str();
    }
    return {};
}

void deleteFile(const std::string& filename) {
    const int ret = std::remove(filename.c_str());
    if (ret != 0 && errno != ENOENT) {
        throw IOException(errno, "Could not delete file " + filename);
    }
}

void copyFile(const std::string& destination, const std::string& source) {
    std::ifstream src(source, std::ios::binary);
    if (!src.good()) {
        throw IOException(errno, "Cannot read file " + source);
    }
    std::ofstream dst(destination, std::ios::binary);
    if (!dst.good()) {
        throw IOException(errno, "Cannot write file " + destination);
    }
    dst << src.rdbuf();
}

} // namespace util
} // namespace mbgl
