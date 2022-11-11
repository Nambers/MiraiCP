// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_LOADERMAIN_H
#define MIRAICP_PRO_LOADERMAIN_H


namespace LibLoader {
    class LoaderMain {
        volatile static bool loader_exit;

    private:
        static void tick() noexcept;

        static void mainloop() noexcept;

        static void shutdownLoader();

    public:
        static bool is_loader_exited() { return loader_exit; }

        static void loaderExit() { loader_exit = true; }

        static void loaderMain();
    };
} // namespace LibLoader
#endif //MIRAICP_PRO_LOADERMAIN_H
