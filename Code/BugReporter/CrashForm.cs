using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Oakwood.BugReporter
{
    public partial class CrashForm : Form
    {
        public CrashForm()
        {
            InitializeComponent();
        }

        void SendReport(string title, string report)
        {
            string name = Program.report[0];
            string server = Program.report[1];
            report.Replace("{br}", Environment.NewLine);

            if (name == null) name = $"{Environment.UserName}@{Environment.MachineName}";
            if (server == null) server = "Main Menu";

            string r = report.Replace(Environment.NewLine, "\\n");

            string myJson = $"{{\"content\": \"A new report received!\",\"embeds\": [{{\"author\": {{\"name\": \"Oakwood\"}},\"title\": \"Bug Report\",\"color\": 16711680,\"fields\": [{{\"name\": \"Username\",\"value\": \"{name}\"}},{{\"name\": \"Server\",\"value\": \"{server}\"}},{{\"name\": \"Title\",\"value\": \"{title}\"}},{{\"name\": \"Message (steps to improduce, etc)\",\"value\": \"{r}\"}}],\"footer\": {{\"text\": \"Report received at {DateTime.Now.ToString("dd.MM.yyyy - HH:mm:ss")}\"}}}}]}}";
            using (var client = new HttpClient())
            {
                var response = client.PostAsync(
                    "https://discordapp.com/api/webhooks/742444857775095867/iFe4wThx2hg6JxF5Y0O9Rl51eus125nCmRG-aU0NdpmvHAGcYzYtzpr4gClFNxeNK1qX",
                     new StringContent(myJson, Encoding.UTF8, "application/json"));
                if(response.Result.StatusCode != System.Net.HttpStatusCode.NoContent && response.Result.StatusCode != System.Net.HttpStatusCode.OK)
                {
                    MessageBox.Show($"Failed to send bug report!\n{response.Result.StatusCode}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    MessageBox.Show("Report successfully sent.\n\nThank you for helping us!", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SendReport(textBox2.Text, textBox1.Text);
            Close();
            Environment.Exit(0);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Close();
            Environment.Exit(1);
        }

        private void CrashForm_Load(object sender, EventArgs e)
        {
            BringToFront();
            Focus();
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            label2.Text = $"Title ({textBox2.TextLength}/{textBox2.MaxLength})";
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            label3.Text = $"Message ({textBox1.TextLength}/{textBox1.MaxLength})";
        }
    }
}
