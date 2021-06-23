using System.IO;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using System.Windows;
using CrystalEditor.Utils.Extensions;

namespace CrystalEditor.Utils
{
    public enum LogLevel
    {
        Info    = 0x01,
        Warning = 0x02,
        Error   = 0x04,
    };

    public class Logger
    {
        public static void Log(string message, LogLevel lvl = LogLevel.Info)
        {
            MessageBox.Show(message + lvl.ToString());
        }
    }
}
