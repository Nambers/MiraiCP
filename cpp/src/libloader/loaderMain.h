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
        volatile bool loader_exit = false;

    private:
        void mainloop() const noexcept;

        void shutdownLoader();

        void tick() const noexcept;

        void process_task_queue() const;

    public:
        static LoaderMain &get();

        [[nodiscard]] bool is_loader_exited() const { return loader_exit; }

        void loaderExit() { loader_exit = true; }

        void loaderMain();
    };
} // namespace LibLoader
#endif //MIRAICP_PRO_LOADERMAIN_H
