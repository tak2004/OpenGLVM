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
using PropertyTools.Wpf;

namespace OGLVMBuilder
{
    using System.ComponentModel;
    using PropertyTools;

    public class State
    {
        public string Name { get; private set; }
        public ObservableCollection<Command> Commands { get; private set; }
        public State(string name) { 
            this.Name = name;
            Commands = new ObservableCollection<Command>();
        }
    }

    [Serializable]
    public class Command
    {
        [Serializable]
        public class Parameter
        {
            public Parameter(string NewName, string NewType)
            {
                Name = NewName;
                Type = NewType;
            }
            public string Name { get; private set; }
            public string Type { get; private set; }
            public string Value { get; set; }
        }

        public string Name { get; private set; }

        public ObservableCollection<Parameter> Parameters { get; private set; }

        public Command(string NewName, ObservableCollection<Parameter> NewParameters)
        { 
            Name = NewName;
            Parameters = NewParameters;
        }
    }

    public class Variable
    {
        public Variable(string NewName, string NewType)
        {
            Name = NewName;
            Type = NewType;
        }
        public string Name { get; set; }
        public string Type { get; set; }
        public string Value { get; set; }
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

        private ObservableCollection<State> states = new ObservableCollection<State>() { };
        private ObservableCollection<Variable> variables = new ObservableCollection<Variable>() { };

        public MainWindow()
        {
            InitializeComponent();

            states.Add(new State("initialize"));
            states.Add(new State("shutdown"));
            states[0].Commands.Add(new Command("glBegin", new ObservableCollection<Command.Parameter>()));
            states[0].Commands.Add(new Command("glEnd", new ObservableCollection<Command.Parameter>()));
            states[1].Commands.Add(new Command("glBegin", new ObservableCollection<Command.Parameter>()));
            states[1].Commands.Add(new Command("glEnd", new ObservableCollection<Command.Parameter>()));

            variables.Add(new Variable("vertexdata", "void*"));
            variables.Add(new Variable("vertexdatasize", "GLuint"));

            DataContext = new
            {
                States = states,
                Variables = variables
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
                            ObservableCollection<Command.Parameter> parameters = new ObservableCollection<Command.Parameter>();
                            foreach (var parameter in command.Parameters)
                            {
                                parameters.Add(new Command.Parameter(parameter.Name, parameter.Type));
                            }
                            Command cmd = new Command(command.Name, parameters);
                            knownCommands.Add(command.Name, cmd);
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

            List<Command> commands = new List<Command>();
            foreach(DictionaryEntry command in knownCommands)
            {
                 commands.Add(command.Value as Command);
            }
            CommandList.ItemsSource = commands;
            CollectionView view = (CollectionView)CollectionViewSource.GetDefaultView(CommandList.ItemsSource);
            view.Filter = UserFilter;
        }

        private bool UserFilter(object item)
        {
            if (String.IsNullOrEmpty(txtFilter.Text))
                return true;
            else
                return ((item as Command).Name.IndexOf(txtFilter.Text, StringComparison.OrdinalIgnoreCase) >= 0);
        }

        private void AppExit_Click(object sender, RoutedEventArgs e)
        {
            DataCheck();
            Application.Current.Shutdown();
        }

        private void NewCode_Click(object sender, RoutedEventArgs e)
        {
            DataCheck();
            states.Clear();
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
                DragDrop.DoDragDrop(parent, draggedItem.DataContext, DragDropEffects.Copy);
            }
        }

        private void TreeView_Drop(object sender, DragEventArgs e)
        {
            TextBlock parent = (TextBlock)sender;
            object data = e.Data.GetData(typeof(Command));
            if (data != null && parent.DataContext is State)
            {
                State state = (State)parent.DataContext;
                state.Commands.Add(data as Command);
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
                    DragDrop.DoDragDrop(lbi, lbi.DataContext, DragDropEffects.Copy);
                }
            }
        }
        
        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            SectionDialog dlg = new SectionDialog();
            if (dlg.ShowDialog() == true)
            {
                states.Add(new State(dlg.Statename));
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
                foreach(var state in states)
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
                states.Remove(state);
                dataChanged = true;
            }
        }

        private void CodeTree_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
//             if (e.NewValue is Command)
//             {
//                 selectedFunction = (Observable)e.NewValue;
// 
//             }
//             else
//             {
//                 selectedFunction = null;
//             }
        }

        private void txtFilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            CollectionViewSource.GetDefaultView(CommandList.ItemsSource).Refresh();
        }
    }
}
