using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
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

namespace Virtion.Knife.Modules.PeEditor.Views
{
    /// <summary>
    /// AdbView.xaml 的交互逻辑
    /// </summary>
    public partial class AdbView : UserControl
    {
        private StreamWriter cmdInputStream;
        public AdbView()
        {
            InitializeComponent();
        }

        void Start()
        {
            Process process = new Process();
            process.StartInfo.FileName = "cmd.exe";
            process.StartInfo.UseShellExecute = false;   // 是否使用外壳程序 
            process.StartInfo.CreateNoWindow = true;   //是否在新窗口中启动该进程的值 
            process.StartInfo.RedirectStandardInput = true;  // 重定向输入流 
            process.StartInfo.RedirectStandardOutput = true;  //重定向输出流 
            process.StartInfo.RedirectStandardError = true;  //重定向错误流 
            process.OutputDataReceived += Process_OutputDataReceived;
            string strCmd = "ping www.163.com /r/n";

            process.Start();
            process.StandardInput.WriteLine(strCmd);
            cmdInputStream = process.StandardInput;

            //string output = process.StandardOutput.ReadToEnd();//获取输出信息 
            //process.WaitForExit();
            //int n = process.ExitCode;  // n 为进程执行返回值 
            //process.Close();

        }

        private void Process_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            OutputTextBox.Text += e.Data;
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            cmdInputStream.WriteLine(InputTextBox.Text);
            InputTextBox.Text = "";
        }
    }
}
