import os
from typing import TYPE_CHECKING

from amalgamation.cpp_process import FileProcessor

if TYPE_CHECKING:
    from amalgamation.amalgamation import Amalgamation


class SourceProcessor(FileProcessor):
    def __init__(self, file: str) -> None:
        super().__init__(file)

    def analyze(self, amalgamation: "Amalgamation"):
        if amalgamation.removeTwoSlashComments:
            self.removeComments()
        self.readInclude(amalgamation)

    def readInclude(self, amalgamation: "Amalgamation"):
        self.findIncludes()
        searchPaths = amalgamation.includeDirectory

        for i in range(len(self.includes)):
            _match = self.includes[i]._match
            relpath = _match.group("path")
            self.includes[i].object = self.searchFile(
                searchPaths,
                relpath,
                [amalgamation.headersDict, amalgamation.sourcesDict]
            )

        l = []
        for inc in self.includes:
            if inc.object is not None:
                l.append(inc)
                if type(inc.object) is SourceProcessor:
                    inc.object.refCount += 1
        self.includes = l

    def process(self) -> str:
        if self.data == "":
            return ""
        ans = f"/// {os.path.basename(self.filename)} START\n"
        lastEndIndex = 0
        datacopy = self.data
        self.data = ""
        for inc in self.includes:
            if inc.object is None:
                raise RuntimeError("include object is None")
            ans += datacopy[lastEndIndex:inc._match.start()]
            inctarget = ""
            if isinstance(inc.object, SourceProcessor):
                inctarget = inc.object.process()
            if inctarget != "":
                ans += f"\n{inctarget}\n"
            lastEndIndex = inc._match.end()
        ans += datacopy[lastEndIndex:]
        ans += f"/// {os.path.basename(self.filename)} END\n"
        return ans
