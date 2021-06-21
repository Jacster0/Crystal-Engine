using CrystalEditor.Networking;
using CrystalEditor.Utils;
using CrystalEditor.Utils.Extensions;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CrystalEditor.Managers
{
    class LogManager
    {
        private const int MAX_LOG_SIZE = 1024;
        private MessageInfo messageInfo;
        NamedPipeServer<MessageInfo> server;

        [StructLayout(LayoutKind.Sequential)]
        struct MessageInfo
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_LOG_SIZE)]
            public string Message;
            public LogLevel Level;
        }

        public void StartUp()
        {
            ListenForUnmanagedLogCalls();
        }

        public void ShutDown()
        {
            server.Stop();
        }

        public void ListenForUnmanagedLogCalls()
        {
            server = new NamedPipeServer<MessageInfo>()
            {
                PipeName = "ManagedLogger",
                NumberOfServerInstances = -1
            };

            server.Run();

            while (true)
            {
                if (!server.data.Empty())
                {
                    messageInfo = server.data.Dequeue();

                    Logger.Log(messageInfo.Message, messageInfo.Level);
                }
            }
        }
    }
}
