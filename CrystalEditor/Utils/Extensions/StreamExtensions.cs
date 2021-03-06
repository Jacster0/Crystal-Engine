using System;
using System.IO;
using System.Runtime.InteropServices;

namespace CrystalEditor.Utils.Extensions
{
    public static class StreamExtensions
    {
        private const int MAX_STACKALLOC_SIZE = 512;

        public static unsafe T ReadStruct<T>(this Stream stream) where T : struct
        {
            var length = Marshal.SizeOf<T>();
            var buffer = (length <= MAX_STACKALLOC_SIZE) ? stackalloc byte[length] : new byte[length];

            stream.Read(buffer);

            fixed (byte* ptr = buffer)
            {
                return Marshal.PtrToStructure<T>((IntPtr)ptr);
            }
        }

        public static unsafe void WriteStruct<T>(this Stream stream, in T structure) where T : struct
        {
            var length = Marshal.SizeOf<T>();
            var buffer = (length <= MAX_STACKALLOC_SIZE) ? stackalloc byte[length] : new byte[length];

            fixed (byte* ptr = buffer)
            {
                Marshal.StructureToPtr(structure, (IntPtr)ptr, true);
            }
            stream.Write(buffer);
        }
    }
}
