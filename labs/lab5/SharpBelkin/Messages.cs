using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;

namespace SharpBelkin
{
    public enum MessageTypesBelkin : int
    {
        MT_CONNECT = 0,
        MT_DISCONNECT = 1,
        MT_INFO = 2,
        MT_DATA = 3
    }

    [StructLayout(LayoutKind.Sequential)]
    struct MessageHeaderBelkin
    {
        [MarshalAs(UnmanagedType.I4)]
        public MessageTypesBelkin type;
        [MarshalAs(UnmanagedType.I4)]
        public int size;
        [MarshalAs(UnmanagedType.I4)]
        public int to;
        [MarshalAs(UnmanagedType.I4)]
        public int from;
    }


    class MessageBelkin
    {
        public MessageHeaderBelkin header;
        public string data;

        public MessageBelkin(int to = -2, int from = 0, MessageTypesBelkin type = MessageTypesBelkin.MT_INFO, string data = "")
        {
            this.data = data;
            header = new MessageHeaderBelkin() { type = type, size = data.Length * 2, to = to, from = from };
        }

        static byte[] ToBytes(object obj)
        {
            int size = Marshal.SizeOf(obj);
            byte[] buff = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, buff, 0, size);
            Marshal.FreeHGlobal(ptr);
            return buff;
        }

        static T fromBytes<T>(byte[] buff) where T : struct
        {
            T data = default(T);
            int size = Marshal.SizeOf(data);
            IntPtr i = Marshal.AllocHGlobal(size);
            Marshal.Copy(buff, 0, i, size);
            var d = Marshal.PtrToStructure(i, data.GetType());
            if (d is not null)
            {
                data = (T)d;
            }
            Marshal.FreeHGlobal(i);
            return data;
        }

        public void send(Socket s)
        {
            s.Send(ToBytes(header), Marshal.SizeOf(header), SocketFlags.None);
            if (header.size != 0)
            {
                s.Send(Encoding.Unicode.GetBytes(data), header.size, SocketFlags.None);
            }
        }

        public bool receive(Socket s)
        {
            try 
            {
                byte[] buff = new byte[Marshal.SizeOf(header)];
                if (s.Receive(buff, Marshal.SizeOf(header), SocketFlags.None) == 0)
                {
                    return false;
                }
                header = fromBytes<MessageHeaderBelkin>(buff);
                if (header.size > 0)
                {
                    byte[] b = new byte[header.size];
                    s.Receive(b, header.size, SocketFlags.None);
                    data = Encoding.Unicode.GetString(b, 0, header.size);

                }
                return true;
            }
            catch
            {
                return false;
            }
            
        }
    }
}
