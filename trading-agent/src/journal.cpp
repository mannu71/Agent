#include "ta/journal.hpp"

#include <openssl/evp.h>

#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace ta {

namespace {

const std::string kGenesis(64, '0');

std::vector<std::string> split_bar(const std::string& line) {
    std::vector<std::string> out;
    std::size_t start = 0;
    for (std::size_t k = 0; k <= line.size(); ++k) {
        if (k == line.size() || line[k] == '|') {
            out.push_back(line.substr(start, k - start));
            start = k + 1;
        }
    }
    return out;
}

std::string chain_hash(const std::string& seq, const std::string& ts, const std::string& prev,
                       const std::string& payload) {
    return sha256_hex(seq + "|" + ts + "|" + prev + "|" + payload);
}

}  // namespace

std::string sha256_hex(const std::string& data) {
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    if (EVP_Digest(data.data(), data.size(), md, &len, EVP_sha256(), nullptr) != 1) {
        throw std::runtime_error("sha256 failed");
    }
    std::string hex;
    char buf[3];
    for (unsigned int k = 0; k < len; ++k) {
        std::snprintf(buf, sizeof buf, "%02x", md[k]);
        hex += buf;
    }
    return hex;
}

std::string utc_now_iso() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&t, &tm);
    char buf[32];
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", &tm);
    return buf;
}

Journal::Journal(std::string path) : path_(std::move(path)), last_hash_(kGenesis) {
    std::ifstream in(path_);
    std::string line, last;
    while (std::getline(in, line)) {
        if (!line.empty()) last = line;
    }
    if (!last.empty()) {
        const auto f = split_bar(last);
        if (f.size() != 5) throw std::runtime_error("corrupt journal tail: " + path_);
        seq_ = std::stol(f[0]);
        last_hash_ = f[4];
    }
}

void Journal::append(const KvRecord& payload) {
    const std::string seq = std::to_string(seq_ + 1);
    const std::string ts = utc_now_iso();
    const std::string body = payload.encode();
    const std::string hash = chain_hash(seq, ts, last_hash_, body);
    std::ofstream out(path_, std::ios::app);
    if (!out) throw std::runtime_error("cannot append to journal " + path_);
    out << seq << '|' << ts << '|' << last_hash_ << '|' << body << '|' << hash << '\n';
    out.flush();
    if (!out) throw std::runtime_error("journal write failed: " + path_);
    ++seq_;
    last_hash_ = hash;
}

Journal::Verification Journal::verify(const std::string& path) {
    Verification v;
    std::ifstream in(path);
    std::string line, prev = kGenesis;
    long expected_seq = 1;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        ++v.lines;
        const auto f = split_bar(line);
        const bool good = f.size() == 5 && f[0] == std::to_string(expected_seq) && f[2] == prev &&
                          chain_hash(f[0], f[1], f[2], f[3]) == f[4];
        if (!good) {
            v.ok = false;
            v.first_bad_line = v.lines;
            return v;
        }
        prev = f[4];
        ++expected_seq;
    }
    return v;
}

}  // namespace ta
