using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SharpBelkin
{
    public enum MessageTypesBelkin : int 
    {
        MT_CONNECT = 0,
        MT_DISCONNECT = 1,
        MT_INFO = 2,
        MT_DATA = 3
    }


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct MsgStructBelkin
    {
        public int to;
        public MessageTypesBelkin msgType;

        [MarshalAs(UnmanagedType.LPWStr)]
        public String data;
    }


    public static class MsgFactory
    {
        public static MsgStructBelkin connectMsg = new MsgStructBelkin
        {
            to = -2,
            msgType = MessageTypesBelkin.MT_CONNECT,
            data = ""
        };

        public static MsgStructBelkin disconnectMsg = new MsgStructBelkin
        {
            to = -2,
            msgType = MessageTypesBelkin.MT_DISCONNECT,
            data = ""
        };

        public static MsgStructBelkin infoMsg = new MsgStructBelkin
        {
            to = -2,
            msgType = MessageTypesBelkin.MT_INFO,
            data = ""
        };
    }
}
