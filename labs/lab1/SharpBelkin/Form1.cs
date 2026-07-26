using System.Diagnostics;
using System.Security.Cryptography.Xml;
using System.Threading;


namespace SharpBelkin
{
    public partial class Form1 : Form
    {
        Process? childProcess = null;
        EventWaitHandle startEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StartEvent");
        EventWaitHandle stopEvent = new EventWaitHandle(false, EventResetMode.ManualReset, "StopEvent");
        EventWaitHandle confirmEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");
        EventWaitHandle closeEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "CloseEvent");

        int counter = 0;

        public Form1()
        {
            InitializeComponent();
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

                comboBox1.Items.AddRange(new String[] { "Главный поток", "Все потоки" });
            }
            else
            {
                int n = (int)numericUpDown1.Value;
                while (n > 0)
                {
                    ++counter;
                    startEvent.Set();
                    confirmEvent.WaitOne();
                    comboBox1.Items.Add($"Поток {counter}");
                    --n;
                }
            }
            
        }

        private void stop_btn_Click(object sender, EventArgs e)
        {
            if (!(childProcess == null || childProcess.HasExited))
            {
                stopEvent.Set();
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
                    closeEvent.Set();

                    childProcess.WaitForExit(1000);
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("Ошибка при закрытии консоли: " + ex.Message);
                }
            }
        }
    }
}
