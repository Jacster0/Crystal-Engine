using CrystalEditor.Networking;
using CrystalEditor.Utils;
using CrystalEditor.Utils.Extensions;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace CrystalEditor.Managers
{
    class LogManager
    {
        private const int MaxLogSize = 512;
        private const int MaxFileNameLength = 260;
        private const int MaxFunctionNameLength = 64;

        private MessageInfo messageInfo;
        private NamedPipeServer<MessageInfo> server;

        [StructLayout(LayoutKind.Sequential)]
        private readonly struct MessageInfo
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MaxLogSize)]
            public readonly string Message;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MaxFileNameLength)]
            public readonly string FileName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MaxFunctionNameLength)]
            public readonly string FunctionName;
            public readonly int Line;
            public readonly LogLevel Level;
        }

        public async void StartUp() => await Task.Run(() => ListenForUnmanagedLogCalls());

        public void ShutDown()
        {
            if (server.Running)
            {
                server.Stop();
            }
        }

        private void ListenForUnmanagedLogCalls()
        {
            server = new NamedPipeServer<MessageInfo>()
            {
                PipeName = "ManagedLogger"
            };

            server.Run();

            while (true)
            {
                if (!server.Data.IsEmpty)
                {
                    if (server.Data.TryDequeue(out messageInfo))
                    {
                        Logger.Log(
                            messageInfo.Message,
                            messageInfo.Level,
                            messageInfo.FileName,
                            messageInfo.FunctionName,
                            messageInfo.Line);
                    }
                }
            }
        }
    }
}
