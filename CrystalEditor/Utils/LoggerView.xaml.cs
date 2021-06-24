using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CrystalEditor.Utils
{
    /// <summary>
    /// Interaction logic for LoggerView.xaml
    /// </summary>
    public partial class LoggerView : UserControl
    {
        public LoggerView()
        {
            InitializeComponent();
        }

        private void OnClearButtonClick(object sender, RoutedEventArgs e) => Logger.Clear();

        private void OnMessageFilterButtonClick(object sender, RoutedEventArgs e)
        {
            LogLevel filter = 0x0;

            if (toggleInfo.IsChecked.Value)
            {
                filter |= LogLevel.Info;
            }

            if (toggleWarnings.IsChecked.Value)
            {
                filter |= LogLevel.Warning;
            }

            if (toggleErrors.IsChecked.Value) 
            {
                filter |= LogLevel.Error;
            }
           
            Logger.SetMessageFilter(filter);
        }
    }
}
