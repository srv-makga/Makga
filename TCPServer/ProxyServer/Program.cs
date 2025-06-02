namespace ProxyServer
{
    class Program
    {
        static void Main(string[] args)
        {
            App.StartTcpProxyServer(18000);
            Console.WriteLine("Starting Proxy Server...");
        }
    }
}