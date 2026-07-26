using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SharpBelkin
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct MsgStructBelkin
    {
        public int to;
        public int msgType;

        [MarshalAs(UnmanagedType.LPWStr)]
        public String data;
    }


    public static class MsgFactory
    {
        public static MsgStructBelkin startMsg = new MsgStructBelkin
        {
            to = -2,
            msgType = 0,
            data = ""
        };

        public static MsgStructBelkin stopMsg = new MsgStructBelkin
        {
            to = -2,
            msgType = 1,
            data = ""
        };

        public static MsgStructBelkin infoMsg = new MsgStructBelkin
        {
            to = -2,
            msgType = 2,
            data = ""
        };
    }
}
