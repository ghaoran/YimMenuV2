#!/usr/bin/env python3
import json, os, re
SRC = "src"
SKIP_DIRS = ("gta/natives","gta/data","types","pointers","fonts","scripting/libraries")
SKIP_FILES = ("Natives.hpp","NativeDefs.cpp","Crossmap.hpp","MainFont.cpp","IconFont.cpp")
LIT = re.compile(r'"((?:[^"\\]|\\.){2,120})"')
HAS_WORD = re.compile(r"[A-Za-z]{2,}")
TECH = re.compile(r"^[a-z0-9_]+$|^[A-Z0-9_]+$|\.(cpp|hpp|h|so|dll|json|ttf|ttc|png|bin)$|^[%a-z0-9\.\-\\/]+$|\\x[0-9a-f]{2}|^#|%[sdふ]|^\d")
def wanted(rel):
    for d in SKIP_DIRS:
        if "/" + d + "/" in rel: return False
    return not any(f in rel for f in SKIP_FILES)
found = {}
for root, dirs, files in os.walk(SRC):
    for f in files:
        if not f.endswith((".cpp",".hpp")): continue
        p = os.path.join(root,f); rel = os.path.relpath(p, SRC).replace(os.sep,"/")
        if not wanted(rel): continue
        for line in open(p, encoding="utf-8", errors="replace"):
            if line.strip().startswith("//"): continue
            for m in LIT.finditer(line):
                lit = m.group(1)
                if not HAS_WORD.search(lit): continue
                if TECH.match(lit.strip()): continue
                if lit.count(" ") == 0 and len(lit) > 28: continue
                found.setdefault(lit, set()).add(rel)
out = {k: sorted(v)[:3] for k,v in sorted(found.items())}
json.dump(out, open("tools/strings.json","w",encoding="utf-8"), ensure_ascii=False, indent=0)
print("unique:", len(out))
for k in list(out)[:20]: print("  ", repr(k))
