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
        private const int MAX_LOG_SIZE = 512;
        private const int MAX_FILE_NAME_LENGTH = 260;
        private const int MAX_FUNCTION_NAME_LENGTH = 64;
     
        private MessageInfo messageInfo;
        NamedPipeServer<MessageInfo> server;

        [StructLayout(LayoutKind.Sequential)]
        struct MessageInfo
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_LOG_SIZE)]
            public string Message;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_FILE_NAME_LENGTH)]
            public string FileName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_FUNCTION_NAME_LENGTH)]
            public string FunctionName;
            public int Line;
            public LogLevel Level;
        }

        public async void StartUp()
        {
            await Task.Run(() => ListenForUnmanagedLogCalls());
        }

        public void ShutDown()
        {
            if (server.Running)
            {
                server.Stop();
            }
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
                    if(server.data.TryDequeue(out messageInfo))
                    {
                        Logger.Log(messageInfo.Message, messageInfo.Level, messageInfo.FileName, messageInfo.FunctionName, messageInfo.Line);
                    }
                }
            }
        }
    }
}
