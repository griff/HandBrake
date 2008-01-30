/*  Program.cs 
 	
 	   This file is part of the HandBrake source code.
 	   Homepage: <http://handbrake.m0k.org/>.
 	   It may be used under the terms of the GNU General Public License. */

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using System.Globalization;


namespace Handbrake
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // Development Code Expiry.
            // Remember to comment out on public release!!!
            if (DateTime.Now > DateTime.Parse("2008/02/27", new CultureInfo("en-US"))) { MessageBox.Show("Sorry, This development build of Handbrake has expired."); return; } 

            // Check the system meets the system requirements.
            Boolean launch = true;
            try
            {
                // Make sure the screen resolution is not below 1024x768
                System.Windows.Forms.Screen scr = System.Windows.Forms.Screen.PrimaryScreen;
                if ((scr.Bounds.Width < 1024) || (scr.Bounds.Height < 720))
                {
                    MessageBox.Show("Your system does not meet the minimum requirements for HandBrake. \n" + "Your screen is running at: " + scr.Bounds.Width.ToString() + "x" + scr.Bounds.Height.ToString() + " \nScreen resolution is too Low. Must be 1024x720 or greater", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    launch = false;
                }

                // Make sure the system has enough RAM. 384MB or greater
                uint memory = MemoryCheck.CheckMemeory();
                memory = memory / 1024 / 1024;

                if (memory < 256)
                {
                    MessageBox.Show("Your system does not meet the minimum requirements for HandBrake. \n Insufficient RAM. 384MB or greater required. You have: " + memory.ToString() + "MB", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    launch = false;
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("frmMain.cs - systemCheck() " + exc.ToString());
            }

            // Either Launch or Close the Application
            if (launch == true)
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new frmMain());
            }
            else
            {
                Application.Exit();
            }
        }
    }

    class MemoryCheck
    {
        public struct MEMORYSTATUS
        {
            public UInt32 dwLength;
            public UInt32 dwMemoryLoad;
            public UInt32 dwTotalPhys; // Used
            public UInt32 dwAvailPhys;
            public UInt32 dwTotalPageFile;
            public UInt32 dwAvailPageFile;
            public UInt32 dwTotalVirtual;
            public UInt32 dwAvailVirtual;
            // Aditional Varibles left in for future usage (JIC)
        }

        [DllImport("kernel32.dll")]
        public static extern void GlobalMemoryStatus
        (
            ref MEMORYSTATUS lpBuffer
        );

        public static uint CheckMemeory()
        {
            // Call the native GlobalMemoryStatus method
            // with the defined structure.
            MEMORYSTATUS memStatus = new MEMORYSTATUS();
            GlobalMemoryStatus(ref memStatus);

            uint MemoryInfo = memStatus.dwTotalPhys;

            return MemoryInfo;
        }
    }
}