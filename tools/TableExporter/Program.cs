using System.Windows.Forms;

namespace TableExporter;

// FolderBrowserDialog 는 COM 기반이므로 반드시 [STAThread] 필요
static class Program
{
    [STAThread]
    static void Main()
    {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new MainForm());
    }
}
