#!/usr/bin/env python3
"""Merge zh_p*.json into zh_cn.json and report coverage against todo_strings.json."""
import glob
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
FIXES = {
    "Garage Innocence Blvd": "伊诺森斯大道车库",
    "Lombank West": "西伦班克大厦",
    "Refresh Current 载具": "刷新当前载具",
    "To Beat About Bush": "绕弯子",
}

merged = {}
for path in sorted(glob.glob(os.path.join(HERE, "zh_p*.json"))):
    with open(path, encoding="utf-8") as fh:
        merged.update(json.load(fh))

merged.update(FIXES)

# drop any value that is identical to its key (untranslated)
merged = {k: v for k, v in merged.items() if v and v != k}

with open(os.path.join(HERE, "zh_cn.json"), "w", encoding="utf-8") as fh:
    json.dump(dict(sorted(merged.items())), fh, ensure_ascii=False, indent=0)

todo = json.load(open(os.path.join(HERE, "todo_strings.json"), encoding="utf-8"))
missing = [s for s in todo if s not in merged]
print("dict entries:", len(merged))
print("todo:", len(todo), "translated:", len(todo) - len(missing), "missing:", len(missing))
for s in missing[:40]:
    print("  MISS", repr(s))
