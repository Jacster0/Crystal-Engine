using CrystalEditor.Utils.Extensions;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CrystalEditor.Networking
{
    public class NamedPipeServer<T> where T : struct
    {
        bool running;
        EventWaitHandle terminateHandle = new EventWaitHandle(false, EventResetMode.AutoReset);

        public Queue<T> data { get; private set; } = new Queue<T>();
        public string PipeName { get; set; }
        public int NumberOfServerInstances { get; set; }

        public void Run()
        {
            running = true;
            new Thread(ProcessClients).Start();
        }

        public void Stop()
        {
            running = false;
            terminateHandle.WaitOne();
        }
        private void ProcessClients()
        {
            while (running)
            {
                ProcessNextClient();
            }
            terminateHandle.Set();
        }

        private void ProcessNextClient()
        {
            var pipeStream = new NamedPipeServerStream(PipeName, PipeDirection.InOut, NumberOfServerInstances, PipeTransmissionMode.Byte);
            pipeStream.WaitForConnection();

            new Thread(() => ProcessClientThread(pipeStream)).Start();
        }

        private void ProcessClientThread(NamedPipeServerStream pipeStream)
        {
            using var streamReader = new StreamReader(pipeStream);
            data.Enqueue(streamReader.BaseStream.ReadStruct<T>());

            pipeStream.Close();
            pipeStream.Dispose();
        }

    }
}
