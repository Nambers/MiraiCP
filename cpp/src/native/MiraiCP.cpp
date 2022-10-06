// Copyright (c) 2022. Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include <stdio.h>

extern "C" {
__declspec(dllexport) void
Verify(const char *a, const char *b, const char *(*oper)(const char *), void (*log)(const char *, int)) {
    printf("call Verify(%s, %s)aa\n", a, b);
    log("test", 1);
}

__declspec(dllexport) void Event(const char *a) {
    printf("call Event(%s)\n", a);
}

__declspec(dllexport) void PluginDisable() {
    printf("call disable\n");
}
}