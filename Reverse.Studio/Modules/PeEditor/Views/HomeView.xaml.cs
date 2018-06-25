using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using Caliburn.Micro;
using Gemini.Modules.Output.ViewModels;
using Newtonsoft.Json;
using Virtion.Knife.Modules.PeEditor.ViewModels;

namespace Virtion.Knife.Modules.PeEditor.Views
{
    /// <summary>
    /// Interaction logic for HomeView.xaml
    /// </summary>
    public partial class HomeView : UserControl
    {
        public HomeView()
        {
            InitializeComponent();
            DataContext = this;
            Loaded += new RoutedEventHandler(MainWindow_Loaded);
        }

        private Frida.DeviceManager deviceManager;
        private Frida.Session session;
        private Frida.Script script;
        private bool scriptLoaded;

        private void RefreshAllowedActions()
        {
            deviceList.IsEnabled = session == null;
            refreshButton.IsEnabled = session == null && deviceList.SelectedItem != null;

            processList.IsEnabled = session == null;
            spawnButton.IsEnabled = processList.SelectedItem != null;
            resumeButton.IsEnabled = processList.SelectedItem != null;
            attachButton.IsEnabled = processList.SelectedItem != null && session == null;
            detachButton.IsEnabled = session != null;

            scriptSource.IsEnabled = session != null && script == null;
            createScriptButton.IsEnabled = session != null && script == null;
            loadScriptButton.IsEnabled = script != null && !scriptLoaded;
            unloadScriptButton.IsEnabled = script != null;
            postToScriptButton.IsEnabled = script != null && scriptLoaded;
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            deviceManager = new Frida.DeviceManager(Dispatcher);
            deviceManager.Changed += new EventHandler(deviceManager_Changed);
            RefreshDeviceList();
            RefreshAllowedActions();
        }

        private void RefreshDeviceList()
        {
            var devices = deviceManager.EnumerateDevices();
            Module.Output.AppendLine(String.Format("Got {0} devices", devices.Length));
            Array.Sort(devices, delegate (Frida.Device a, Frida.Device b)
            {
                var aHasIcon = a.Icon != null;
                var bHasIcon = b.Icon != null;
                if (aHasIcon == bHasIcon)
                    return a.Id.CompareTo(b.Id);
                else
                    return bHasIcon.CompareTo(aHasIcon);
            });

            var model = IoC.Get<HomeViewModel>();
            model.Devices.Clear();
            foreach (var device in devices)
                model.Devices.Add(device);
        }

        private void RefreshProcessList()
        {
            var model = IoC.Get<HomeViewModel>();
            var device = deviceList.SelectedItem as Frida.Device;
            if (device == null)
            {
                model.Processes.Clear();
                return;
            }

            try
            {
                var processes = device.EnumerateProcesses();
                Array.Sort(processes, delegate (Frida.Process a, Frida.Process b)
                {
                    var aHasIcon = a.LargeIcon != null;
                    var bHasIcon = b.LargeIcon != null;
                    if (aHasIcon == bHasIcon)
                        return a.Name.CompareTo(b.Name);
                    else
                        return bHasIcon.CompareTo(aHasIcon);
                });
                model.Processes.Clear();
                foreach (var process in processes)
                    model.Processes.Add(process);
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("EnumerateProcesses failed: " + ex.Message);
                model.Processes.Clear();
            }
        }

        private void deviceManager_Changed(object sender, EventArgs e)
        {
            Module.Output.AppendLine("DeviceManager Changed");
            RefreshDeviceList();
        }

        private void deviceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            RefreshAllowedActions();
            RefreshProcessList();
        }

        private void refreshButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshProcessList();
        }

        private void processList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            RefreshAllowedActions();
        }

        private void spawnButton_Click(object sender, RoutedEventArgs e)
        {
            var device = deviceList.SelectedItem as Frida.Device;
            try
            {
                //device.Spawn("C:\\Windows\\notepad.exe", new string[] { "C:\\Windows\\notepad.exe", "C:\\document.txt" }, new string[] { });
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("Spawn failed: " + ex.Message);
            }
        }

        private void resumeButton_Click(object sender, RoutedEventArgs e)
        {
            var device = deviceList.SelectedItem as Frida.Device;
            try
            {
                device.Resume(1337);
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("Resume failed: " + ex.Message);
            }
        }

        private void attachButton_Click(object sender, RoutedEventArgs e)
        {
            var device = deviceList.SelectedItem as Frida.Device;
            var process = processList.SelectedItem as Frida.Process;

            try
            {
                session = device.Attach(process.Pid);
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("Attach failed: " + ex.Message);
                return;
            }
            //session.Detached += new EventHandler(session_Detached);
            Module.Output.AppendLine("Attached to " + session.Pid);
            RefreshAllowedActions();
        }

        private void detachButton_Click(object sender, RoutedEventArgs e)
        {
            session.Detach();
            session = null;
            script = null;
            RefreshAllowedActions();
        }

        private void session_Detached(object sender, EventArgs e)
        {
            if (sender == session)
            {
                Module.Output.AppendLine("Detached from Session with Pid: " + session.Pid);
                session = null;
                script = null;
                RefreshAllowedActions();
            }
        }

        private void createScriptButton_Click(object sender, RoutedEventArgs e)
        {
            if (script != null)
            {
                try
                {
                    script.Unload();
                }
                catch (Exception ex)
                {
                    Module.Output.AppendLine("Failed to unload previous script: " + ex.Message);
                }
                script = null;
                scriptLoaded = false;
                RefreshAllowedActions();
            }

            try
            {
                script = session.CreateScript(scriptSource.Text);
                scriptLoaded = false;
                RefreshAllowedActions();
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("CreateScript failed: " + ex.Message);
                return;
            }
            script.Message += new Frida.ScriptMessageHandler(script_Message);
        }

        private void loadScriptButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                script.Load();
                scriptLoaded = true;
                RefreshAllowedActions();
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("Load failed: " + ex.Message);
            }
        }

        private void unloadScriptButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                script.Unload();
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("Failed to unload script: " + ex.Message);
            }
            script = null;
            scriptLoaded = false;
            RefreshAllowedActions();
        }

        private void postToScriptButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                script.Post("{\"type\":\"banana\"}");
            }
            catch (Exception ex)
            {
                Module.Output.AppendLine("PostMessage failed: " + ex.Message);
            }
        }

        class MessageItem
        {
            public string type { get; set; }
            public string payload { get; set; }
        }

        private void script_Message(object sender, Frida.ScriptMessageEventArgs e)
        {
            if (sender == script)
            {
                //debugConsole.Items.Add(String.Format("Message from Script: {0}", ));
                //debugConsole.Items.Add(String.Format("  Data: {0}", e.Data == null ? "null" : String.Join(", ", e.Data)));
                try
                {
                    var msg = JsonConvert.DeserializeObject<MessageItem>(e.Message);
                    if (String.IsNullOrEmpty(msg.payload))
                    {
                        Module.Output.AppendLine(e.Message);
                        return;
                    }
                    Module.Output.AppendLine(msg.payload);
                }
                catch (Exception)
                {
                    Module.Output.AppendLine(e.Message);
                }
                
            }
        }
    }
}
