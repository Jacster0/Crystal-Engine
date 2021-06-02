using System;
using System.Runtime.InteropServices;
using System.Windows.Interop;

namespace CrystalEditor.API
{
    class RenderSurface : HwndHost
    {
        public uint SurfaceWidth { get; private set; } = 1300;
        public uint SurfaceHeight { get; private set; } = 800;

        private IntPtr windowHandle = IntPtr.Zero;

        public RenderSurface() { }
        public RenderSurface(uint width, uint height) => (SurfaceWidth, SurfaceHeight) = (width, height);

        protected override HandleRef BuildWindowCore(HandleRef parentHwnd)
        {
            EngineApi.CreateRenderSurface(parentHwnd.Handle, SurfaceWidth, SurfaceHeight);

            return new HandleRef(this, EngineApi.WindowHandle);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            EngineApi.DestroyRenderSurface();
            windowHandle = IntPtr.Zero;
        }
    }
}
