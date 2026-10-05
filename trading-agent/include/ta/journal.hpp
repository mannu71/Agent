#pragma once

#include <string>

#include "ta/kv.hpp"

namespace ta {

std::string sha256_hex(const std::string& data);

// Append-only, hash-chained decision log. Each line is
//   seq|utc_timestamp|prev_hash|payload|hash
// where hash = sha256(seq|utc_timestamp|prev_hash|payload). Editing or deleting any
// earlier line breaks every later hash, so a suggestion cannot be quietly rewritten
// after its outcome is known.
class Journal {
public:
    explicit Journal(std::string path);

    // Appends one record stamped with the current UTC wall-clock time.
    void append(const KvRecord& payload);

    struct Verification {
        bool ok = true;
        long lines = 0;
        long first_bad_line = 0;  // 1-based, 0 when ok
    };
    static Verification verify(const std::string& path);

    const std::string& path() const { return path_; }

private:
    std::string path_;
    long seq_ = 0;
    std::string last_hash_;
};

std::string utc_now_iso();

}  // namespace ta
