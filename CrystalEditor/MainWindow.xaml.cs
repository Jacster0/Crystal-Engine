using CrystalEditor.Managers;
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
            manager.StartUp();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
        }
    }
}
