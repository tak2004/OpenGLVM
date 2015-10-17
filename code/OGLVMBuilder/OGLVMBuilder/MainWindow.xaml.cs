using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
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
using Microsoft.Win32;

namespace OGLVMBuilder
{
    public class State
    {
        public string Name { get; private set; }
        public ObservableCollection<Command> Commands { get; private set; }
        public State(string name) { 
            this.Name = name;
            Commands = new ObservableCollection<Command>();
        }
    }    

    public class Command
    {
        public string Name { get; set; }
        public Command(string name)
        { 
            this.Name = name;
        }
    }    

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Point _dragStartPoint;
        private bool dataChanged = false;
        
        private T FindVisualParent<T>(DependencyObject child)
            where T:DependencyObject
        {
            var parentObject = VisualTreeHelper.GetParent(child);
            if (parentObject == null)
                return null;
            T parent = parentObject as T;
            if (parent != null)
                return parent;
            return FindVisualParent<T>(parentObject);
        }

        private ObservableCollection<State> code = new ObservableCollection<State>() { };

        public MainWindow()
        {
            InitializeComponent();

            code.Add(new State("initialize"));
            code.Add(new State("shutdown"));
            code[0].Commands.Add(new Command("glBegin"));
            code[0].Commands.Add(new Command("glEnd"));
            code[1].Commands.Add(new Command("glBegin"));
            code[1].Commands.Add(new Command("glEnd"));

            DataContext = new
            {
                Code = code
            };

            LoadSpecs();
        }

        private void ListBoxItem_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _dragStartPoint = e.GetPosition(null);
        }

        private void ListBoxItem_Drop(object sender, DragEventArgs e)
        {
             if (sender is TextBlock)
             {
                 var source = e.Data.GetData(typeof(Command)) as Command;
                 var target = ((TextBlock)(sender)).DataContext as Command;

//                  int sourceIndex = -1;
//                  foreach(var item in CodeTree.Items)
//                     item.IndexOf(source);
//                  int targetIndex = CodeTree.Items.IndexOf(target);
 
//                 if (sourceIndex == -1)
//                {
//                     if (target.AllowParent)
//                         _items.Insert(targetIndex + 1, new VMItem(e.Data.GetData(typeof(string)) as string, true));
//                     else
//                         _items.Insert(targetIndex + 1, new VMItem(e.Data.GetData(typeof(string)) as string,true));
/*                }*/
//                else
/*                {*/
               //     Move(source, sourceIndex, targetIndex);
//                 }
             }
        }

//         private void Move(VMItem source, int sourceIndex, int targetIndex)
//         {
//             if (sourceIndex < targetIndex)
//             {
//                 _items.Insert(targetIndex + 1, source);
//                 _items.RemoveAt(sourceIndex);
//             }
//             else
//             {
//                 int removeIndex = sourceIndex + 1;
//                 if (_items.Count + 1 > removeIndex)
//                 {
//                     _items.Insert(targetIndex, source);
//                     _items.RemoveAt(removeIndex);
//                 }
//             }
//         }

        private void LoadSpecs()
        {
            Hashtable knownCommands = null;
            if (!System.IO.File.Exists("dump.bin"))
            {

                var specs = GLCSGen.GLSpec.FromFile("gl.xml");

                knownCommands = new Hashtable();

                foreach (var version in specs.Versions)
                {
                    foreach (var command in version.Commands)
                    {
                        if (!knownCommands.ContainsKey(command.Name))
                        {
                            knownCommands.Add(command.Name, command.Name);
                        }
                    }
                }

                System.IO.FileStream fs = new System.IO.FileStream("dump.bin", System.IO.FileMode.Create);
                BinaryFormatter formatter = new BinaryFormatter();
                formatter.Serialize(fs, knownCommands);

            }
            else
            {
                System.IO.FileStream fs = new System.IO.FileStream("dump.bin", System.IO.FileMode.Open);
                BinaryFormatter formatter = new BinaryFormatter();
                knownCommands = (Hashtable)formatter.Deserialize(fs);
            }

            foreach(DictionaryEntry command in knownCommands)
            {
                CommandList.Items.Add(command.Value);
            }
        }

        private void AppExit_Click(object sender, RoutedEventArgs e)
        {
            DataCheck();
            Application.Current.Shutdown();
        }

        private void NewCode_Click(object sender, RoutedEventArgs e)
        {
            DataCheck();
            code.Clear();
        }

        protected void SaveData(string filename)
        {
            dataChanged = false;
        }

        protected void LoadData(string filename)
        {
            dataChanged = false;
        }

        protected void DataCheck()
        {
            if (dataChanged == true)
            {
                MessageBoxButton btnMessageBox = MessageBoxButton.YesNo;
                MessageBoxImage icnMessageBox = MessageBoxImage.Warning;

                MessageBoxResult rsltMessageBox = MessageBox.Show("Do you want to save the current data ?",
                    "Possible data loss", btnMessageBox, icnMessageBox);

                if(rsltMessageBox ==MessageBoxResult.Yes)
                {
                    SaveFileDialog saveFileDialog = new SaveFileDialog();
                    if (saveFileDialog.ShowDialog() == true)
                        SaveData(saveFileDialog.FileName);
                }
            }
        }

        private void OpenCode_Click(object sender, RoutedEventArgs e)
        {
            DataCheck();

            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
                LoadData(openFileDialog.FileName);
        }

        private void SaveCode_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            if (saveFileDialog.ShowDialog() == true)
                SaveData(saveFileDialog.FileName);
        }

        private void CommandList_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            ListBox parent = (ListBox)sender;
            TextBlock draggedItem = e.OriginalSource as TextBlock;

            if (draggedItem != null)
            {
                DragDrop.DoDragDrop(parent, draggedItem.Text, DragDropEffects.Move);
            }
        }

        private void TreeView_Drop(object sender, DragEventArgs e)
        {
            TextBlock parent = (TextBlock)sender;
            object data = e.Data.GetData(typeof(string));
            if (data != null && parent.DataContext is State)
            {
                State state = (State)parent.DataContext;
                state.Commands.Add(new Command(data as string));
                dataChanged = true;
            }
        }

        private void TreeView_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            Point point = e.GetPosition(null);
            Vector diff = _dragStartPoint - point;
            if (e.LeftButton == MouseButtonState.Pressed &&
                (Math.Abs(diff.X) > SystemParameters.MinimumHorizontalDragDistance ||
                    Math.Abs(diff.Y) > SystemParameters.MinimumVerticalDragDistance))
            {
                TextBlock lbi = (TextBlock)e.OriginalSource;
                if (lbi != null)
                {
                    DragDrop.DoDragDrop(lbi, lbi.DataContext, DragDropEffects.Move);
                }
            }
        }
        
        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            SectionDialog dlg = new SectionDialog();
            if (dlg.ShowDialog() == true)
            {
                code.Add(new State(dlg.Statename));
                dataChanged = true;
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            DataCheck();
        }

        private void DeleteCommand_Click(object sender, RoutedEventArgs e)
        {
            MenuItem item = sender as MenuItem;
            ContextMenu menu = item.CommandParameter as ContextMenu;
            if (menu.PlacementTarget is TextBlock)
            {
                TextBlock cmdComponent = menu.PlacementTarget as TextBlock;
                Command cmd = cmdComponent.DataContext as Command;
                foreach(var state in code)
                {
                    if (state.Commands.Remove(cmd))
                        break;
                }
                dataChanged = true;
            }
        }

        private void DeleteState_Click(object sender, RoutedEventArgs e)
        {
            MenuItem item = sender as MenuItem;
            ContextMenu menu = item.CommandParameter as ContextMenu;
            if (menu.PlacementTarget is TextBlock)
            {
                TextBlock stateComponent = menu.PlacementTarget as TextBlock;
                State state = stateComponent.DataContext as State;
                code.Remove(state);
                dataChanged = true;
            }
        }
    }
}
