using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Data;

namespace CrystalEditor.Utils
{
    [Flags]
    public enum LogLevel
    {
        Info    = 0x01,
        Warning = 0x02,
        Error   = 0x04,
    };

    class LogMessage
    {
        public DateTime Time { get; private set; }
        public LogLevel Level { get; private set; }
        public string Message { get; private set; }

        public string File { get; private set; }
        public string Method { get; private set; }
        public int Line { get; private set; }
        public string MetaData => $"{File}: {Method} ({Line})";
        public LogMessage(string message, LogLevel level, string file, string method, int line)
        {
            Time = DateTime.Now;
            Level = level;
            Message = message;
            File = Path.GetFileName(file);
            Method = method;
            Line = line;
        }
    }
    
    static class Logger
    {
        private static LogLevel messageFilter = LogLevel.Info | LogLevel.Warning | LogLevel.Error;
        private static readonly ObservableCollection<LogMessage> messages = new ObservableCollection<LogMessage>();
        public static ReadOnlyObservableCollection<LogMessage> Messages 
        {
            get; 
            private set;
        } = new ReadOnlyObservableCollection<LogMessage>(messages);

        public static CollectionViewSource FilteredMessages { get; set; } = new CollectionViewSource() { Source = Messages };
        
        static Logger()
        {
            FilteredMessages.Filter += (s, e) =>
            {
                var type = (e.Item as LogMessage).Level;
                e.Accepted = (type & messageFilter) != 0;
            };
        }

        public static async void Log(
            string message, 
            LogLevel lvl = LogLevel.Info,
            [CallerFilePath] string file = "",
            [CallerMemberName] string method = "",
            [CallerLineNumber] int line = 0)
        {
            await Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                messages.Add(new LogMessage(message, lvl,file,method,line));
            }));
        }

        public static async void Clear()
        {
            await Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                messages.Clear();
            }));
        }

        public static void SetMessageFilter(LogLevel filter)
        {
            messageFilter = filter;

            FilteredMessages.View.Refresh();
        }
    }
}
