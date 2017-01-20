using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;


namespace AKCore_Launcher
{
    public partial class Form1 : Form
    {
        System.Timers.Timer CheckProcesses;

        public Form1()
        {
            InitializeComponent();
            CheckProcesses = new System.Timers.Timer(5000);
            CheckProcesses.Elapsed += PollUpdates;
            CheckProcesses.Start();

        }

        bool hidden;
        private void PollUpdates(object sender, EventArgs e)
        {
            string[] processes = new string[] { "UniController", "AuthServer", "CharServer", "ChatServer", "GameServer" };

            if (lblConsole.Text == "Hidden")
                hidden = true;
            else
                hidden = false;

            if (Process.GetProcessesByName(processes[0]).Length == 0)
            {
                lblWeb.Text = "Not Running";
            }
            else
            {
                lblWeb.Text = "Running";
            }


            if (Process.GetProcessesByName(processes[1]).Length == 0)
            {
                lblAuth.Text = "Not Running";
            }
            else
            {
                lblAuth.Text = "Running";
            }

            if (Process.GetProcessesByName(processes[2]).Length == 0)
            {
                lblChar.Text = "Not Running";
            }
            else
            {
                lblChar.Text = "Running";
            }


            if (Process.GetProcessesByName(processes[3]).Length == 0)
            {
                lblComm.Text = "Not Running";
            }
            else
            {
                lblComm.Text = "Running";
            }


            if (Process.GetProcessesByName(processes[4]).Length == 0)
            {
                lblWorld.Text = "Not Running";
            }
            else
            {
                lblWorld.Text = "Running";
            }


        }

        private void btnDatabase_Click(object sender, EventArgs e)
        {
            bool isRunning = false;
            string ExeName = "UniController.exe";
            if (lblWeb.Text == "Running")
                isRunning = true;
            else
                isRunning = false;
            if (!isRunning)
            {
                // Prepare the process to run
                ProcessStartInfo start = new ProcessStartInfo();
                // Enter in the command line arguments, everything you would enter after the executable name itself
                start.Arguments = null;
                start.WorkingDirectory = "web";
                // Enter the executable to run, including the complete path
                start.WindowStyle = ProcessWindowStyle.Minimized;
                start.FileName = ExeName;
                Process procWeb = Process.Start(start);
            }
            else
            {
                foreach (var process in Process.GetProcessesByName("UniController"))
                {
                    process.Kill();
                }
            }
            PollUpdates(sender, e);
        }

        private void btnAuth_Click(object sender, EventArgs e)
        {
            bool isRunning = false;
            string ExeName = "authServer.exe";
            if (lblAuth.Text == "Running")
                isRunning = true;
            else
                isRunning = false;
            // Prepare the process to run
            if (!isRunning)
            {
                // Prepare the process to run
                ProcessStartInfo start = new ProcessStartInfo();
                // Enter in the command line arguments, everything you would enter after the executable name itself
                start.Arguments = null;
                // Enter the executable to run, including the complete path
                start.WindowStyle = ProcessWindowStyle.Minimized;
                start.FileName = ExeName;
                Process procWeb = Process.Start(start);
            }
            else
            {
                foreach (var process in Process.GetProcessesByName("AuthServer"))
                {
                    process.Kill();
                }
            }
            PollUpdates(sender, e);
        }

        private void btnChar_Click(object sender, EventArgs e)
        {
            string ExeName = "CharServer.exe";
            bool isRunning = false;
            if (lblChar.Text == "Running")
                isRunning = true;
            else
                isRunning = false;
            if (!isRunning)
            {
                // Prepare the process to run
                ProcessStartInfo start = new ProcessStartInfo();
                // Enter in the command line arguments, everything you would enter after the executable name itself
                start.Arguments = null;
                // Enter the executable to run, including the complete path
                start.WindowStyle = ProcessWindowStyle.Minimized;
                start.FileName = ExeName;
                Process procWeb = Process.Start(start);
            }
            else
            {
                foreach (var process in Process.GetProcessesByName("CharServer"))
                {
                    process.Kill();
                }
            }
            PollUpdates(sender, e);
        }

        private void btnComm_Click(object sender, EventArgs e)
        {
            string ExeName = "ChatServer.exe";
            bool isRunning = false;
            if (lblComm.Text == "Running")
                isRunning = true;
            else
                isRunning = false;
            if (!isRunning)
            {
                // Prepare the process to run
                ProcessStartInfo start = new ProcessStartInfo();
                // Enter in the command line arguments, everything you would enter after the executable name itself
                start.Arguments = null;
                // Enter the executable to run, including the complete path
                start.WindowStyle = ProcessWindowStyle.Minimized;
                start.FileName = ExeName;
                Process procWeb = Process.Start(start);
            }
            else
            {
                foreach (var process in Process.GetProcessesByName("ChatServer"))
                {
                    process.Kill();
                }
            }
            PollUpdates(sender, e);
        }

        private void btnGame_Click(object sender, EventArgs e)
        {
            string ExeName = "GameServer.exe";
            bool isRunning = false;
            if (lblWorld.Text == "Running")
                isRunning = true;
            else
                isRunning = false;
            if (!isRunning)
            {
                // Prepare the process to run
                ProcessStartInfo start = new ProcessStartInfo();
                // Enter in the command line arguments, everything you would enter after the executable name itself
                start.Arguments = null;
                // Enter the executable to run, including the complete path
                start.WindowStyle = ProcessWindowStyle.Minimized;
                start.FileName = ExeName;
                Process procWeb = Process.Start(start);
            }
            else
            {
                foreach (var process in Process.GetProcessesByName("GameServer"))
                {
                    process.Kill();
                }
            }
            PollUpdates(sender, e);
        }

        private void btnConsole_Click(object sender, EventArgs e)
        {

            if (lblConsole.Text == "Showing")
             lblConsole.Text = "Hidden";
            else
             lblConsole.Text = "Showing";

        }
    }
}