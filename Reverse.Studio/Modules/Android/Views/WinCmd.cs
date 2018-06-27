using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Virtion.Knife.Modules.PeEditor.Views
{
     class WinCmd
    {
        string _workDirectory;//工作文件夹，应该指向 你要执行的exe文件的所在路径

        public WinCmd()
        {

        }

        public WinCmd(string workDirectory)
        {
            _workDirectory = workDirectory;
        }
        //comamndString是要执行的文件名，argment是执行参数，output是执行的输出结果，errout是当错误时返回的结果。
        //返回值是 是否成功执行，如果失败了，就查看下errout内的信息
        public bool RunCommand(string comamndString, string argment, out string output, out string errout)
        {
            StringBuilder _result = null;
            StringBuilder _error = null;

            _result = new StringBuilder();
            _error = new StringBuilder();

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = comamndString;// "adb devices";
            startInfo.Arguments = argment;
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = false;
            //startInfo.WindowStyle = ProcessWindowStyle.Normal;
            startInfo.RedirectStandardInput = true;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;
            startInfo.WorkingDirectory = _workDirectory;

            Process process1 = null;
            try
            {
                process1 = Process.Start(startInfo);

                //接收错误的事件
                process1.ErrorDataReceived += (object sender, DataReceivedEventArgs e) =>
                {
                    if (!string.IsNullOrEmpty(e.Data))
                    {
                        _result.AppendLine(e.Data);
                    }
                };
                //接收数据的事件
                process1.OutputDataReceived += (object sender, DataReceivedEventArgs e) =>
                {
                    if (!string.IsNullOrEmpty(e.Data))
                    {
                        _error.AppendLine(e.Data);
                    }
                };
                process1.BeginErrorReadLine();
                process1.BeginOutputReadLine();

                //result = sr2.ReadToEnd();
                //err = sr1.ReadToEnd();
                process1.WaitForExit();
            }
            catch (Exception)
            {
                throw;
            }
            finally
            {
                if (process1 != null && !process1.HasExited)
                {
                    process1.Kill();
                }
            }

            output = _result.ToString();
            errout = _error.ToString();

            _error = null;
            _result = null;

            if (!string.IsNullOrEmpty(errout))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }

}
