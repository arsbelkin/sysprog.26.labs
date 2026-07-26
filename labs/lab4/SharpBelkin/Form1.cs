using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.JavaScript;
using System.Security.Cryptography.Xml;
using System.Threading;
using static System.Runtime.InteropServices.JavaScript.JSType;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

using System.Linq;


namespace SharpBelkin
{
    public partial class Form1 : Form
    {
        [DllImport("DllTcpBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void sendMsgFromSharp(MsgStructBelkin msg);

        [DllImport("DllTcpBelkin.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool receiveMsgFromSharp(ref MsgStructBelkin msg);

        MsgStructBelkin msg = new();
        EventWaitHandle confirmEvent = new EventWaitHandle(false, EventResetMode.ManualReset);
        Thread myThread;

        bool isConnected = false;

        List<string> counter = new();

        public Form1()
        {
            InitializeComponent();
            ChangeButtons();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        public void Worker()
        {
            sendMsgFromSharp(MsgFactory.connectMsg);
            isConnected = receiveMsgFromSharp(ref msg);
            ChangeButtons();

            if (isConnected)
                label2.Text = msg.data;

            while (isConnected)
            {
                isConnected = receiveMsgFromSharp(ref msg);

                if (!isConnected)
                {
                    counter.Clear();
                    fillBox(counter);
                    ChangeButtons();
                    label2.Text = "";
                    break;
                }

                switch (msg.msgType)
                {
                    case MessageTypesBelkin.MT_INFO:
                        List<string> ids = msg.data.Split(',', StringSplitOptions.RemoveEmptyEntries).ToList();

                        counter = ids;
                        fillBox(counter);

                        confirmEvent.Set();
                        break;

                    case MessageTypesBelkin.MT_DATA:
                        textBox2.AppendText(msg.data);
                        textBox2.AppendText("\n");
                        break;
                }
            }

            confirmEvent.Set();
            return;
        }

        private void ChangeButtons()
        {
            checkBox1.Checked = isConnected;
            button1.Enabled = !isConnected;
            button2.Enabled = isConnected;
            send_btn.Enabled = isConnected;
            comboBox1.Enabled = isConnected;
        }

        private void fillBox(List<string> ids)
        {
            comboBox1.Items.Clear();

            if (ids.Count == 0)
            {
                return;
            }

            comboBox1.Items.Add("Все клиенты");

            foreach (string id in ids)
            {
                comboBox1.Items.Add($"Клиент {id}");
            }
        }



        private void button3_Click(object sender, EventArgs e)
        {
            int to_index = comboBox1.SelectedIndex;
            if (to_index < 0)
                return;

            if (to_index == 0)
                to_index = -1;
            else
                to_index = int.Parse(comboBox1.Text.Split(' ', StringSplitOptions.RemoveEmptyEntries).ToList()[1]);

            string textMsg = textBox1.Text;
            if (string.IsNullOrWhiteSpace(textMsg))
                return;

            MsgStructBelkin dataMsg = new MsgStructBelkin
            {
                to = to_index,
                msgType = MessageTypesBelkin.MT_DATA,
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
            if (!isConnected)
                return;

            sendMsgFromSharp(MsgFactory.disconnectMsg);
            confirmEvent.WaitOne();
        }

        private void comboBox1_DropDown(object sender, EventArgs e)
        {
            sendMsgFromSharp(MsgFactory.infoMsg);
            confirmEvent.WaitOne();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            myThread = new Thread(Worker);
            myThread.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            sendMsgFromSharp(MsgFactory.disconnectMsg);
            confirmEvent.WaitOne();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
    }
}
