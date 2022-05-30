//
// Created by antares on 5/30/22.
//

#ifndef MIRAICP_PRO_LOADERMAIN_H
#define MIRAICP_PRO_LOADERMAIN_H


namespace LibLoader {
    class LoaderMain {
        volatile static bool loader_exit;

        static bool is_loader_exited() { return loader_exit; }

        static void mainloop();
    public:
        static void loaderExit() {
            loader_exit = true;
        }

        static void loaderMain();
    };
} // namespace LibLoader
#endif //MIRAICP_PRO_LOADERMAIN_H
