#include <cstring>
#include <sstream>

#include <http_exceptions.h>
#include <algorithm>
#include "http_request.h"

std::string &HttpRequest::get_method() {
    return method_;
}

std::string HttpRequest::get_method() const {
    return std::string(method_);
}

void HttpRequest::set_method(const std::string &method) {
    method_ = method;
}

HttpRequest::HttpRequest(const int in_fd) {
    char buffer[buf_size_];
    int buffer_len = read_line(in_fd, buffer);
    if (buffer_len == -1) {
        throw ReadException("Error while reading from file descriptor");
    }

    char *method_begin = buffer;
    char *method_end = strchr(method_begin, ' ');
    if (!method_end) {
        throw DelimException("Space not found");
    }

    method_ = std::string(method_begin, method_end - method_begin);

    char *url_begin = method_end + 1;
    while (isspace(*url_begin))
        url_begin++;
    char *url_end = strchr(url_begin, ' ');
    if (!url_end) {
        throw DelimException("Space not found");
    }

    url_ = std::string(url_begin, url_end - url_begin);

    char *protocol_begin = url_end + 1;

    if (sscanf(protocol_begin, "HTTP/%d.%d", &version_major_, &version_minor_) != 2) {
        throw ReadException("Error while reading from file descriptor");
    }

    while ((buffer_len = read_line(in_fd, buffer)) > 0) {
        char *header_name_begin = buffer;
        char *header_name_end = strchr(header_name_begin, ':');
        if (!header_name_end)
            continue;

        char *header_value_begin = header_name_end + 1;
        while (isspace(*header_value_begin))
            header_value_begin++;
        char *header_value_end = buffer + buffer_len;

        std::string header_name = std::string(header_name_begin, header_name_end - header_name_begin);
        std::transform(header_name.begin(),
                       header_name.end(),
                       header_name.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        std::string header_value = std::string(header_value_begin, header_value_end - header_value_begin);
        std::transform(header_value.begin(),
                       header_value.end(),
                       header_value.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        headers_[header_name] = header_value;
    }
    if (buffer_len < 0) {
        throw ReadException("Error while reading body from file descriptor");
    }
}
std::string &HttpRequest::get_url() {
    return url_;
}
