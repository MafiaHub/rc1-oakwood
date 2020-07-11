using System;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Oakwood.CrashReporter
{
    static class Program
    {
        public static string[] report;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            if(args.Length < 1)
            {
                MessageBox.Show("Nice try. :)", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            report = args[0].Split('|');

            if(report.Length != 3)
            {
                MessageBox.Show("Welp, your crash report is corrupted, bye!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new CrashForm());
        }
    }
}
