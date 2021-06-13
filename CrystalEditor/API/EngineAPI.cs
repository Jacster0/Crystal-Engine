using System;
using System.Runtime.InteropServices;

namespace CrystalEditor.API
{
    public static class EngineApi
    {
        private const string path = @"CrystalDll.dll";

        #region public_API_interface
        public static IntPtr WindowHandle { get { return get_window_handle(); } }

        public static void CreateRenderSurface(IntPtr parent, uint width, uint height) => create_render_surface(parent, width, height);
        public static void DestroyRenderSurface() => destroy_render_surface();
        #endregion

        #region private_API_interface
        [DllImport(path)]
        private static extern void create_render_surface(IntPtr parent, uint width = 1300, uint height = 800);

        [DllImport(path)]
        private static extern void destroy_render_surface();

        [DllImport(path)]
        private static extern IntPtr get_window_handle();

        [DllImport(path)]
        private static extern IntPtr get_cpu_information();
        #endregion
    }
}
