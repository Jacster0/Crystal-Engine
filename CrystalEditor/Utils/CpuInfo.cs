using CrystalEditor.API;
using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace CrystalEditor.Utils
{
    public class CpuInfo
    {
#pragma warning disable 0649
        private struct CpuInfo_t
        {
            public int NumLogicalProcessors;
            public int NumCores;
            public string BrandString;
            public string Vendor;
            public string Architecture;
        }
#pragma warning restore 0649

        private CpuInfo_t cpuInfo;

        public string BrandString => cpuInfo.BrandString;
        public string Vendor => cpuInfo.Vendor;
        public string Architecture => cpuInfo.Architecture;
        public int NumLogicalProcessors => cpuInfo.NumLogicalProcessors;
        public int NumCores => cpuInfo.NumCores;

        public CpuInfo()
        {
            const BindingFlags bindingFlags = BindingFlags.NonPublic | BindingFlags.Static;
            const string methodName         = "get_cpu_information";

            //I'm doing this reflection bullshit because I don't want to expose the "get_cpu_information" method as part of the public api
            //and the friend of class concept does not exist in C# AFAIK.
            //The only way to obtain cpu information should be to create an instance of this class. 
            var ptr = (IntPtr)typeof(EngineApi).GetMethod(methodName, bindingFlags).Invoke(null, null);
            cpuInfo = Marshal.PtrToStructure<CpuInfo_t>(ptr);
        }
    }
}
