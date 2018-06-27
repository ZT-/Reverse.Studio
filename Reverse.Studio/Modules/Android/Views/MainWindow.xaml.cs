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
using ConsoleControlAPI;

namespace WpfApp1
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        /// <summary>
        ///     The internal process interface used to interface with the process.
        /// </summary>
        private readonly ProcessInterface processInterace = new ProcessInterface();

        /// <summary>
        ///     Occurs when console input is produced.
        /// </summary>
        public event ConsoleEventHanlder OnConsoleInput;

        /// <summary>
        ///     The console event handler is used for console events.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="args">The <see cref="ConsoleEventArgs" /> instance containing the event data.</param>
        public delegate void ConsoleEventHanlder(object sender, ConsoleEventArgs args);

        public MainWindow()
        {
            InitializeComponent();

            //  Handle process events.
            processInterace.OnProcessOutput += processInterace_OnProcessOutput;
            processInterace.OnProcessError += processInterace_OnProcessError;
            processInterace.OnProcessInput += processInterace_OnProcessInput;
            processInterace.OnProcessExit += processInterace_OnProcessExit;
        }

        /// <summary>
        ///     Handles the OnProcessError event of the processInterace control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="args">The <see cref="ProcessEventArgs" /> instance containing the event data.</param>
        private void processInterace_OnProcessError(object sender, ProcessEventArgs args)
        {
            //  Write the output, in red
            WriteOutput(args.Content, Colors.Red);

            //  Fire the output event.
            //FireConsoleOutputEvent(args.Content);
        }

        /// <summary>
        ///     Handles the OnProcessOutput event of the processInterace control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="args">The <see cref="ProcessEventArgs" /> instance containing the event data.</param>
        private void processInterace_OnProcessOutput(object sender, ProcessEventArgs args)
        {
            //  Write the output, in white
            WriteOutput(args.Content, Colors.White);

            //  Fire the output event.
            //FireConsoleOutputEvent(args.Content);
        }

        /// <summary>
        ///     Handles the OnProcessInput event of the processInterace control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="args">The <see cref="ProcessEventArgs" /> instance containing the event data.</param>
        private void processInterace_OnProcessInput(object sender, ProcessEventArgs args)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        ///     Handles the OnProcessExit event of the processInterace control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="args">The <see cref="ProcessEventArgs" /> instance containing the event data.</param>
        private void processInterace_OnProcessExit(object sender, ProcessEventArgs args)
        {
            //  Are we showing diagnostics?
            //if (ShowDiagnostics)
            {
                WriteOutput(Environment.NewLine + processInterace.ProcessFileName + " exited.",
                    Color.FromArgb(255, 0, 255, 0));
            }

            //if (!IsHandleCreated)
            //    return;
            ////  Read only again.
            //Invoke((Action)(() => { richTextBoxConsole.ReadOnly = true; }));
        }

        /// <summary>
        ///     Writes the input to the console control.
        /// </summary>
        /// <param name="input">The input.</param>
        /// <param name="color">The color.</param>
        /// <param name="echo">if set to <c>true</c> echo the input.</param>
        public void WriteInput(string input, Color color, bool echo)
        {
            processInterace.WriteInput(input);

            //Invoke((Action)(() =>
            //{
            //    //  Are we echoing?
            //    if (echo)
            //    {
            //        richTextBoxConsole.SelectionColor = color;
            //        richTextBoxConsole.SelectedText += input;
            //        inputStart = richTextBoxConsole.SelectionStart;
            //    }

            //    lastInput = input;

            //    //  Write the input.
            //    processInterace.WriteInput(input);

            //    //  Fire the event.
            //    FireConsoleInputEvent(input);
            //}));
        }

        /// <summary>
        ///     Writes the output to the console control.
        /// </summary>
        /// <param name="output">The output.</param>
        /// <param name="color">The color.</param>
        public void WriteOutput(string output, Color color)
        {
            //if (string.IsNullOrEmpty(lastInput) == false &&
            //    (output == lastInput || output.Replace("\r\n", "") == lastInput))
            //    return;

            //if (!IsHandleCreated)
            //    return;

            Dispatcher.Invoke((Action)(() =>
            {
                this.OutputTextBox.Text += output;
                //  Write the output.
                // richTextBoxConsole.SelectionColor = color;
                //richTextBoxConsole.SelectedText += output;
                //inputStart = richTextBoxConsole.SelectionStart;
            }));

        }

        /// <summary>
        ///     Fires the console input event.
        /// </summary>
        /// <param name="content">The content.</param>
        private void FireConsoleInputEvent(string content)
        {
            //  Get the event.
            var theEvent = OnConsoleInput;
            if (theEvent != null)
                theEvent(this, new ConsoleEventArgs(content));
        }

        private void Button_OnClick(object sender, RoutedEventArgs e)
        {
            WriteInput(this.InputTextBox.Text, Colors.AliceBlue, true);
        }

        private void Button1_OnClick(object sender, RoutedEventArgs e)
        {
            processInterace.StartProcess("cmd","");
        }
    }
}
