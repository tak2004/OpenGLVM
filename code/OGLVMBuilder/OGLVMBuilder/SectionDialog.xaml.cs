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
using System.Windows.Shapes;

namespace OGLVMBuilder
{
    /// <summary>
    /// Interaction logic for SectionDialog.xaml
    /// </summary>
    public partial class SectionDialog : Window
    {
        public string Statename { get; private set; }

        public SectionDialog()
        {
            InitializeComponent();
            Text.Focus();
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void Ok_Click(object sender, RoutedEventArgs e)
        {
            Statename = Text.Text;
            DialogResult = true;
        }

        private void Text_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                if (Text.Text.Length > 0)
                {
                    Statename = Text.Text;
                    DialogResult = true;
                }
                e.Handled = true;
            }
        }
    }
}
