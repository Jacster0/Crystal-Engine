using CrystalEditor.Utils;
using CrystalEditor.Utils.Extensions;
using System;
using System.Collections.Concurrent;
using System.IO.Pipes;

namespace CrystalEditor.Networking
{
    public class NamedPipeServer<T> where T : struct
    {
        public ConcurrentQueue<T> Data { get; } = new ConcurrentQueue<T>();
        public string PipeName { get; set; }
        public bool Running { get; private set; } = false;
        public void Run()
        {
            Running = true;
            ProcessClients();
        }

        public void Stop() => Running = false;

        private void ProcessClients()
        {
            var pipeStream = new NamedPipeServerStream(
                PipeName,
                PipeDirection.In,
                NamedPipeServerStream.MaxAllowedServerInstances,
                PipeTransmissionMode.Byte,
                PipeOptions.Asynchronous);

            pipeStream.BeginWaitForConnection(HandleConnection, pipeStream);
        }

        private void HandleConnection(IAsyncResult result)
        {
            var pipeStream = result.AsyncState as NamedPipeServerStream;

            try
            {
                pipeStream.EndWaitForConnection(result);

                if (!Running)
                {
                    pipeStream.Close();
                    pipeStream.Dispose();
                    return;
                }

                ProcessClients();

                Data.Enqueue(pipeStream.ReadStruct<T>());
            }
            catch (Exception e)
            {
                Logger.Log(e.Message, LogLevel.Error);
            }
            finally
            {
                pipeStream.Close();
                pipeStream.Dispose();
            }
        }
    }
}
