using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.JavaScript;
using System.Security.Cryptography.Xml;
using System.Threading;
using static System.Runtime.InteropServices.JavaScript.JSType;


namespace SharpBelkin
{
    public partial class Form1 : Form
    {
        Process? childProcess = null;
        EventWaitHandle confirmEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");

        [DllImport("DLLMMFBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void sendMsgFromSharp(MsgStructBelkin msg);

        [DllImport("DLLMMFBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool initDll();

        [DllImport("DLLMMFBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool closeDll();

        int counter = 0;

        public Form1()
        {
            InitializeComponent();

            initDll();
        }

        private void createProcess()
        {
            childProcess = new Process();
            childProcess.StartInfo.FileName = "PlusiBelkin.exe";

            childProcess.EnableRaisingEvents = true;

            childProcess.Exited += ChildProcess_Exited;

            childProcess.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (childProcess == null || childProcess.HasExited)
            {
                createProcess();

                comboBox1.Items.AddRange(new System.String[] { "Главный поток", "Все потоки" });
            }
            else
            {
                int n = (int)numericUpDown1.Value;
                while (n > 0)
                {
                    sendMsgFromSharp(MsgFactory.startMsg);
                    
                    confirmEvent.WaitOne();
                    ++counter;
                    comboBox1.Items.Add($"Поток {counter}");
                    --n;
                }
            }
            
        }

        private void stop_btn_Click(object sender, EventArgs e)
        {
            if (!(childProcess == null || childProcess.HasExited))
            {
                sendMsgFromSharp(MsgFactory.stopMsg);

                confirmEvent.WaitOne();

                comboBox1.Items.Remove($"Поток {counter--}");
            }
        }

        private void ChildProcess_Exited(object sender, EventArgs e)
        {
            this.Invoke((MethodInvoker)delegate
            {
                comboBox1.Items.Clear();
                counter = 0;
                comboBox1.Items.Remove("Главный поток");
                comboBox1.Items.Remove("Все потоки");
                confirmEvent.Set();
            });
        }

        private void button3_Click(object sender, EventArgs e)
        {
            int to_index = comboBox1.SelectedIndex - 2;
            if (to_index < -2)
                return;

            string textMsg = textBox1.Text;
            if (string.IsNullOrWhiteSpace(textMsg))
                return;

            MsgStructBelkin dataMsg = new MsgStructBelkin
            {
                to = to_index,
                msgType = 3,
                data = textMsg
            };

            sendMsgFromSharp(dataMsg);

            confirmEvent.WaitOne();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!(childProcess == null || childProcess.HasExited))
            {
                try
                {
                    sendMsgFromSharp(MsgFactory.closeMsg);

                    childProcess.WaitForExit(1000);
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("Ошибка при закрытии консоли: " + ex.Message);
                }
            }

            closeDll();
        }
    }
}
