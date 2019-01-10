using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;

namespace CrashReporter
{
    class Program
    {
        static void Main(string[] args)
        {
            string line_to_parse = args[0];
            string[] data = line_to_parse.Split('|');
            string name     = data[0];
            string server   = data[1];
            string dump     = data[2];

            var httpWebRequest = (HttpWebRequest)WebRequest.Create("http://oakmaster.madaraszd.net:8001/push");
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.Credentials = CredentialCache.DefaultCredentials;
            httpWebRequest.Method = "POST";
            httpWebRequest.UserAgent = ".NET Framework Example Client";

            if (name.Length <= 0)
                name = "Unknown";

            if (server.Length <= 0)
                server = "Not connected";

            using (var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {
                string json = "{\"name\":\"" + name + "\"," + "\"host\":\"" + server +"\"," + "\"dump\":\"" + dump + "\"}";
                streamWriter.Write(json);
                streamWriter.Flush();
                streamWriter.Close();
            }

            httpWebRequest.GetResponse();
        }
    }
}
