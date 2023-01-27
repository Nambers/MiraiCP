import json
import os
import re
from typing import Dict, List, Optional, Type, TypeVar

from amalgamation.cpp_process import FileProcessor
from amalgamation.header import HeaderProcessor
from amalgamation.source import SourceProcessor

_T = TypeVar("_T")


class JsonHooker(dict):
    def __getattr__(self, attr):
        return self.get(attr)

    def __setattr__(self, attr, value):
        self[attr] = value


class AmalgamationData(JsonHooker):
    class Extension(JsonHooker):
        source: List[str]
        header: List[str]
    baseDirectory: str
    sources: List[str]
    headers: List[str]
    sourceDest: str
    headerDest: str
    prologue: Optional[str]
    includeDirectory: List[str]
    removeTwoSlashComments: bool
    extension: Extension


class Amalgamation(object):
    def __init__(self, data: AmalgamationData) -> None:
        FileProcessor.registerBaseDirectory(data.baseDirectory)
        self.rawsources = data.sources
        self.rawheaders = data.headers
        self.sourceDest = data.sourceDest
        self.headerDest = data.headerDest
        self.prologue = data.prologue
        self.includeDirectory = data.includeDirectory
        self.removeTwoSlashComments = data.removeTwoSlashComments
        self.supportedExtensions = data.extension

        # pre-process
        # first, update include directory
        searchPaths: List[str] = []
        for _path in self.includeDirectory:
            if os.path.isabs(_path):
                searchPaths.append(_path)
            else:
                searchPaths.append(os.path.abspath(
                    os.path.join(FileProcessor.baseDir, _path)
                ))
        self.includeDirectory = searchPaths
        # update destination
        if not os.path.isabs(self.sourceDest):
            self.sourceDest = os.path.abspath(
                os.path.join(FileProcessor.baseDir, self.sourceDest)
            )
            self.headerDest = os.path.abspath(
                os.path.join(FileProcessor.baseDir, self.headerDest)
            )

        # construct sources and headers processor
        self.sources: List[SourceProcessor] = list()
        self.sourcesDict: Dict[str, SourceProcessor] = dict()
        self.headers: List[HeaderProcessor] = list()
        self.headersDict: Dict[str, HeaderProcessor] = dict()

        for s in self.rawsources:
            fullPath = os.path.abspath(os.path.join(FileProcessor.baseDir, s))
            if os.path.isdir(fullPath):
                for d, _, files in os.walk(fullPath):
                    for file in files:
                        self.addSource(os.path.join(
                            os.path.join(fullPath, d), file))
            else:
                self.addSource(fullPath)

        for s in self.rawheaders:
            fullPath = os.path.abspath(os.path.join(FileProcessor.baseDir, s))
            if os.path.isdir(fullPath):
                for d, _, files in os.walk(fullPath):
                    for file in files:
                        self.addHeader(os.path.join(
                            os.path.join(fullPath, d), file))
            else:
                self.addHeader(fullPath)

    def addSource(self, fullpath: str):
        if any(fullpath.endswith(x) for x in self.supportedExtensions.source) \
                or any(fullpath.endswith(x) for x in self.supportedExtensions.header):
            self.addObject(
                fullpath,
                SourceProcessor,
                self.sources,
                self.sourcesDict,
            )

    def addHeader(self, fullpath: str):
        if any(fullpath.endswith(x) for x in self.supportedExtensions.header):
            self.addObject(
                fullpath,
                HeaderProcessor,
                self.headers,
                self.headersDict,
            )

    @staticmethod
    def addObject(fullpath: str, _type: Type[_T], _list: List[_T], _dict: Dict[str, _T]):
        fullpath = os.path.abspath(fullpath)
        s = _type(fullpath)
        _list.append(s)
        _dict[fullpath] = s

    def analyze(self):
        for s in self.sources:
            s.analyze(self)
        for h in self.headers:
            h.analyze(self)

    def process(self):
        prologue = ""
        if self.prologue is not None:
            with open(os.path.join(FileProcessor.baseDir, self.prologue), 'r', encoding='utf-8') as f:
                prologue = f.read()
        sourcePrologue = prologue + \
            f"\n#include \"{os.path.basename(self.headerDest)}\"\n"
        self._process(self.headers, self.headerDest, prologue)
        print(f"Header file is generated to {self.headerDest}")
        self._process(self.sources, self.sourceDest, sourcePrologue)
        print(f"Source file is generated to {self.sourceDest}")

    @staticmethod
    def _process(filelist: List[FileProcessor], writeto: str, prologue: str):
        root: List[HeaderProcessor] = []
        for s in filelist:
            if s.refCount == 0:
                root.append(s)
        if len(root) == 0 and len(filelist) != 0:
            raise ValueError("Circular include detected")

        result = prologue
        for s in root:
            result += s.process()

        result = re.sub(re.compile("\n\n\n+"), "\n\n", result)

        d = os.path.dirname(writeto)
        if not os.path.exists(d):
            os.mkdir(d)

        with open(writeto, 'w', encoding='utf-8') as f:
            f.write(result)


def run(jsonfile: str):
    with open(jsonfile, 'r', encoding='utf-8') as f:
        j: AmalgamationData = json.load(f, object_hook=AmalgamationData)
    amalgamation = Amalgamation(j)
    amalgamation.analyze()
    amalgamation.process()
