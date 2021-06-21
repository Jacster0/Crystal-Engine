using System.IO;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using System.Windows;
using CrystalEditor.Utils.Extensions;

namespace CrystalEditor.Utils
{
    public enum LogLevel
    {
        NORMAL,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    public class Logger
    {
        public static void Log(string message, LogLevel lvl = LogLevel.NORMAL)
        {
            MessageBox.Show(message + lvl.ToString());
        }
    }
}
