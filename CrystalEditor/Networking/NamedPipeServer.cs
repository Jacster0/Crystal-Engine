using CrystalEditor.Utils.Extensions;
using System;
using System.Collections.Concurrent;
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
        EventWaitHandle terminateHandle = new EventWaitHandle(false, EventResetMode.AutoReset);

        public ConcurrentQueue<T> data { get; private set; } = new ConcurrentQueue<T>();
        public string PipeName { get; set; }
        public int NumberOfServerInstances { get; set; }
        public bool Running { get; private set; }

        public void Run()
        {
            Running = true;

            new Thread(() => 
            { 
                Thread.CurrentThread.IsBackground = true; 
                ProcessClients(); 
            }).Start();
        }

        public void Stop()
        {
            Running = false;
            terminateHandle.WaitOne();
        }
        private void ProcessClients()
        {
            while (Running)
            {
                ProcessNextClient();
            }
            terminateHandle.Set();
        }

        private async void ProcessNextClient()
        {
            var pipeStream = new NamedPipeServerStream(PipeName, PipeDirection.InOut, NumberOfServerInstances, PipeTransmissionMode.Byte);
            pipeStream.WaitForConnection();

            await Task.Run(() =>
            {
                using var streamReader = new StreamReader(pipeStream);
                data.Enqueue(streamReader.BaseStream.ReadStruct<T>());

                pipeStream.Close();
                pipeStream.Dispose();
            });
        }
    }
}
