using System;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Windows.Forms;

namespace Oakwood.BugReporter
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
            if(args.Length >= 1)
            {
                report = args[0].Split('|');

                if (report.Length < 2)
                {
                    MessageBox.Show("Welp, your crash report is corrupted, bye!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                else if (report.Length == 4)
                {
                    string name = report[0];
                    string server = report[1];
                    string title = report[2];
                    string msg = report[3];

                    if (name == null) name = $"{Environment.UserName}@{Environment.MachineName}";
                    if (server == null) server = "Main Menu";

                    string t = title.Replace("{sp}", " ");
                    string m = msg.Replace("{br}", "\\n").Replace("{sp}", " ");

                    string myJson = $"{{\"content\": \"A new report received!\",\"embeds\": [{{\"author\": {{\"name\": \"Oakwood\"}},\"title\": \"Bug Report\",\"color\": 16711680,\"fields\": [{{\"name\": \"Username\",\"value\": \"{name}\"}},{{\"name\": \"Server\",\"value\": \"{server}\"}},{{\"name\": \"Title\",\"value\": \"{t}\"}},{{\"name\": \"Message (steps to improduce, etc)\",\"value\": \"{m}\"}}],\"footer\": {{\"text\": \"Report received at {DateTime.Now.ToString("dd.MM.yyyy - HH:mm:ss")}\"}}}}]}}";
                    using (var client = new HttpClient())
                    {
                        var response = client.PostAsync(
                            "https://discordapp.com/api/webhooks/742444857775095867/iFe4wThx2hg6JxF5Y0O9Rl51eus125nCmRG-aU0NdpmvHAGcYzYtzpr4gClFNxeNK1qX",
                             new StringContent(myJson, Encoding.UTF8, "application/json"));
                        if (response.Result.StatusCode != System.Net.HttpStatusCode.NoContent && response.Result.StatusCode != System.Net.HttpStatusCode.OK)
                        {
                            Environment.Exit(1);
                        }
                        else
                        {
                            Environment.Exit(0);
                        }
                    }
                }
            }
            else
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new CrashForm());
            }
        }
    }
}
