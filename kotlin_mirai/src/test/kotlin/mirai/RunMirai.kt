package mirai

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.console.command.CommandManager
import net.mamoe.mirai.console.pure.MiraiConsolePureLoader

object RunMirai {

    // ִ�� gradle task: runMiraiConsole ���Զ�����, shadow, ����, ������ pure console.

    @JvmStatic
    fun main(args: Array<String>) {
        // Ĭ���� /test Ŀ¼������

        MiraiConsolePureLoader.load(args[0], args[1]) // ���� console

        runBlocking { CommandManager.join() } // ��ֹ���߳��˳�
    }
}