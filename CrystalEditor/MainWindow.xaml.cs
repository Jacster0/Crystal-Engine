using CrystalEditor.API;
using CrystalEditor.Managers;
using System.Threading;
using System.Windows;

namespace CrystalEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            LogManager manager = new LogManager();
            new Thread(() =>
            {
                Thread.CurrentThread.IsBackground = true;
                manager.StartUp();
            }).Start();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
        }
    }
}
