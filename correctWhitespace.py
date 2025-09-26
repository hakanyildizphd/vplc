#!/usr/bin/python3 -B

# Author: Hakan Yıldız
# Shared under MIT License. See the file LICENSE for more info.

# A tool to correct input files so that they contain space-separated non-empty
# lines ending with a final newline character. Uses UTF-8 encoding.
# [WARNING] Overwrites the given file in place.

# TODO: Improve this tool.

from sys import argv
from typing import List

def correctedContent(inputFile : str) -> str:
    correctedLines : List[str] = []
    with open(inputFile, "r", encoding = "utf-8") as inputStream:
        for line in inputStream.readlines():
            line = " ".join(line.split())
            if line != "":
                correctedLines.append(line + "\n")
    return "".join(correctedLines)

if len(argv) < 2:
    print()
    print("    correctWhitespace.py <inputFile>")
    print()
    print("    Corrects (overwrites) <inputFile> so that it contains")
    print("    space-separated non-empty lines ending with a final")
    print("    newline character. Uses UTF-8 encoding.")
    print()
else:
    newContent = correctedContent(argv[1])
    with open(argv[1], "w", encoding = "utf-8") as outputStream:
        print(newContent, end = "", file = outputStream)
