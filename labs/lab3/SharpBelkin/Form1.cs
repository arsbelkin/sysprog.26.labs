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
        [DllImport("DllTcpBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void sendMsgFromSharp(MsgStructBelkin msg);

        [DllImport("DllTcpBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int receiveMsgFromSharp();

        int counter = 0;

        public Form1()
        {
            InitializeComponent();

            sendMsgFromSharp(MsgFactory.infoMsg);
            counter = receiveMsgFromSharp();
            fillBox(counter);
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void fillBox(int n)
        {
            comboBox1.Items.Clear();

            if (n < 0)
            {
                MessageBox.Show("сервер не запущен или возникла ошибка");
                return;
            }

            comboBox1.Items.Add("Главный поток");
            comboBox1.Items.Add("Все потоки");

            int i = 1;

            while (i <= n)
            {
                comboBox1.Items.Add($"Поток {i}");
                ++i;
            }
        }


        private void button1_Click(object sender, EventArgs e)
        {
            int n = (int)numericUpDown1.Value;
            while (n > 0)
            {
                sendMsgFromSharp(MsgFactory.startMsg);

                counter = receiveMsgFromSharp();

                fillBox(counter);
                --n;
            }
        }

        private void stop_btn_Click(object sender, EventArgs e)
        {
            sendMsgFromSharp(MsgFactory.stopMsg);
            counter = receiveMsgFromSharp();
            fillBox(counter);
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
            try
            {
                sendMsgFromSharp(MsgFactory.infoMsg);
                receiveMsgFromSharp();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Ошибка при закрытии консоли: " + ex.Message);
            }
        }

        private void comboBox1_DropDown(object sender, EventArgs e)
        {
            sendMsgFromSharp(MsgFactory.infoMsg);
            counter = receiveMsgFromSharp();
            fillBox(counter);
        }
    }
}
