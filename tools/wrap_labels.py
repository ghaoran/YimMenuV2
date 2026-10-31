#!/usr/bin/env python3
"""Wrap user-visible C++ string literals in Localization::Tr()/TrStr().

Rules:
  * ImGui label arguments are wrapped with LBL("English"), which yields
    "Chinese##English" so widget IDs stay unique and stable.
  * Plain display calls (ImGui::Text / SetTooltip) are wrapped with Tr().
  * Format strings (containing % specifiers), "##" ids and pure technical
    literals are skipped.
"""
import os
import re
import sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")

# ImGui functions whose first string argument is a label/ID.
LABEL_FN = re.compile(
    r'(ImGui::(?:Text|Button|SmallButton|SeparatorText|TextDisabled|MenuItem|Selectable|'
    r'BeginChild|BeginTabItem|TextLink|BulletText|Checkbox|RadioButton|CollapsingHeader|'
    r'TreeNode|TreeNodeEx)\()' 
)
FORMAT_SPEC = re.compile(r'%(?!%)(?:[-+ #0]*\d*(?:\.\d+)?(?:lld|llu|zu|zd|[diouxXeEfgGcs]|\.\d+f))')

# Literal already wrapped?
WRAPPED = re.compile(r'(?:LBL|Tr|TrStr)\(\s*"')


def skip(lit: str) -> bool:
    if "##" in lit:
        return True
    if FORMAT_SPEC.search(lit):
        return True
    if not re.search(r"[A-Za-z]", lit):
        return True
    return False


def wrap_line(line: str) -> str:
    out = line
    # ImGui label arguments: ImGui::Xxx("Literal" -> ImGui::Xxx(LBL("Literal"
    def label_sub(m):
        start = m.end()
        rest = out[start:]
        if WRAPPED.match(rest):
            return m.group(1)
        mm = re.match(r'"((?:[^"\\]|\\.)*)"', rest)
        if not mm:
            return m.group(1)
        lit = mm.group(1)
        if skip(lit):
            return m.group(1)
        return m.group(1) + 'LBL("'

    return out


def process(path: str) -> bool:
    with open(path, encoding="utf-8") as fh:
        text = fh.read()
    orig = text
    lines = text.split("\n")
    changed = False
    for i, line in enumerate(lines):
        stripped = line.lstrip()
        if stripped.startswith("//"):
            continue
        new = line
        # handle every ImGui label call occurrence on the line
        pos = 0
        while True:
            m = LABEL_FN.search(new, pos)
            if not m:
                break
            after = new[m.end():]
            if WRAPPED.match(after):
                pos = m.end()
                continue
            mm = re.match(r'\s*"((?:[^"\\]|\\.)*)"', after)
            if not mm or skip(mm.group(1)):
                pos = m.end()
                continue
            lit = mm.group(1)
            left = new[:m.end()]
            right = after[mm.end():]
            new = left + ' LBL("' + lit + '")' + right
            pos = m.end()
        if new != line:
            lines[i] = new
            changed = True
    text = "\n".join(lines)
    if changed and text != orig:
        with open(path, "w", encoding="utf-8") as fh:
            fh.write(text)
    return changed


def main():
    files = []
    for root, _, fs in os.walk(ROOT):
        for f in fs:
            if f.endswith((".cpp", ".hpp")):
                p = os.path.join(root, f)
                rel = p.replace(os.sep, "/")
                if "/gta/" in rel or "/types/" in rel or "/fonts/" in rel or "/pointers/" in rel:
                    continue
                if f in ("Natives.hpp", "NativeDefs.cpp", "Crossmap.hpp", "MainFont.cpp", "IconFont.cpp", "LocalizationData.hpp", "Localization.cpp"):
                    continue
                files.append(p)
    n = 0
    for p in files:
        if process(p):
            n += 1
    print("files changed:", n)


main()
