using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.IO;

namespace CrystalEditor.Utils.Extensions
{
    public static class StreamExtensions
    {
        private const int MAX_STACKALLOC_SIZE = 512;

        public static T ReadStruct<T>(this Stream stream) where T : struct
        {
            var length = Marshal.SizeOf<T>();
            Span<byte> buffer = (length <= MAX_STACKALLOC_SIZE) ? stackalloc byte[length] : new byte[length];

            stream.Read(buffer);
            return MemoryMarshal.Read<T>(buffer);
        }

        public static void WriteStruct<T>(this Stream stream, ref T structure) where T : struct
        {
            var length = Marshal.SizeOf<T>();
            Span<byte> buffer = (length <= MAX_STACKALLOC_SIZE) ? stackalloc byte[length] : new byte[length];

            MemoryMarshal.Write(buffer, ref structure);
            stream.Write(buffer);
        }
    }
}
