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

namespace Oakwood.CrashReporter
{
    public partial class CrashForm : Form
    {
        public CrashForm()
        {
            InitializeComponent();
        }

        void SendReport()
        {
            string name = Program.report[0];
            string server = Program.report[1];
            string report = Program.report[2];
            report.Replace("{br}", Environment.NewLine);

            if (name == "") name = $"{Environment.UserName}@{Environment.MachineName}";
            if (server == "") server = "Not connected";

            string myJson = $"{{'Name':'{name}','Host':'{server}','Dump':'{report}'}}";
            using (var client = new HttpClient())
            {
                client.PostAsync(
                    "http://report.oakwood-mp.net/push",
                     new StringContent(myJson, Encoding.UTF8, "application/json"));
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(checkBox1.Checked)
            {
                string name = Program.report[0];
                string server = Program.report[1];
                string[] report = Program.report[2].Split(new string[] { "{br}" }, StringSplitOptions.None);

                if (name == "") name = $"{Environment.UserName}@{Environment.MachineName}";
                if (server == "") server = "Not connected";

                ReportForm f = new ReportForm();

                f.textBox1.AppendText($"Name: {name}");
                f.textBox1.AppendText(Environment.NewLine);
                f.textBox1.AppendText($"Server: {server}");
                f.textBox1.AppendText(Environment.NewLine);
                f.textBox1.AppendText(Environment.NewLine);
                f.textBox1.AppendText("=== CRASH REPORT BEGIN ===");
                f.textBox1.AppendText(Environment.NewLine);
                foreach(var line in report)
                {
                    f.textBox1.AppendText(line);
                    f.textBox1.AppendText(Environment.NewLine);
                }
                f.textBox1.AppendText(Environment.NewLine);
                f.textBox1.AppendText("===  CRASH REPORT END  ===");

                DialogResult res = f.ShowDialog();

                if(res == DialogResult.Yes)
                {
                    SendReport();
                    MessageBox.Show("Thanks for reporting the crash!\nWe appreciate your help to us while developing this mod.", "Report sent", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                SendReport();
                MessageBox.Show("Thanks for reporting the crash!\nWe appreciate your help to us while developing this mod.", "Report sent", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
