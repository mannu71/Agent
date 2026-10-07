"""Append-only, hash-chained journal in the C++ ta::Journal format.

Each line is  seq|utc_timestamp|prev_hash|payload|hash  with
hash = sha256("seq|utc_timestamp|prev_hash|payload") and payload a KvRecord encoding
("type key=value ...", with space = | % and line breaks percent-escaped). `ta_paper verify`
checks files written here.
"""
import datetime as dt
import hashlib
import os

GENESIS = "0" * 64
_ESC = set(" =|%\n\r\t")


def escape(s) -> str:
    return "".join(f"%{ord(ch):02X}" if ch in _ESC else ch for ch in str(s))


def unescape(s: str) -> str:
    out, k = [], 0
    while k < len(s):
        if s[k] == "%" and k + 2 < len(s):
            out.append(chr(int(s[k + 1:k + 3], 16)))
            k += 3
        else:
            out.append(s[k])
            k += 1
    return "".join(out)


def fmt(v) -> str:
    if isinstance(v, bool):
        return "1" if v else "0"
    if isinstance(v, float):
        return repr(v)
    return str(v)


def encode(rtype: str, fields: dict) -> str:
    return " ".join([escape(rtype)] + [f"{escape(k)}={escape(fmt(v))}" for k, v in fields.items()])


def decode(payload: str):
    toks = payload.split(" ")
    rec = {}
    for tok in toks[1:]:
        k, v = tok.split("=", 1)
        rec[unescape(k)] = unescape(v)
    return unescape(toks[0]), rec


class Journal:
    def __init__(self, path: str):
        self.path = path
        self.seq, self.last = 0, GENESIS
        if os.path.exists(path):
            last = ""
            with open(path, encoding="utf-8") as f:
                for line in f:
                    if line.strip():
                        last = line.rstrip("\n")
            if last:
                p = last.split("|")
                if len(p) != 5:
                    raise ValueError(f"corrupt journal tail: {path}")
                self.seq, self.last = int(p[0]), p[4]

    def append(self, rtype: str, **fields):
        seq = str(self.seq + 1)
        ts = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
        body = encode(rtype, fields)
        h = hashlib.sha256(f"{seq}|{ts}|{self.last}|{body}".encode()).hexdigest()
        os.makedirs(os.path.dirname(self.path) or ".", exist_ok=True)
        with open(self.path, "a", encoding="utf-8", newline="\n") as f:
            f.write(f"{seq}|{ts}|{self.last}|{body}|{h}\n")
        self.seq, self.last = int(seq), h

    @staticmethod
    def verify(path: str):
        """(ok, lines, first_bad_line)."""
        prev, n = GENESIS, 0
        if not os.path.exists(path):
            return True, 0, 0
        with open(path, encoding="utf-8") as f:
            for line in f:
                line = line.rstrip("\n")
                if not line:
                    continue
                n += 1
                p = line.split("|")
                if len(p) != 5 or p[0] != str(n) or p[2] != prev:
                    return False, n, n
                if hashlib.sha256(f"{p[0]}|{p[1]}|{p[2]}|{p[3]}".encode()).hexdigest() != p[4]:
                    return False, n, n
                prev = p[4]
        return True, n, 0
