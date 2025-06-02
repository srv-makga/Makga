using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace ProxyServer
{
    public class App
    {
        private static List<IPEndPoint> serverEndpoints_ = new List<IPEndPoint>
           {
               new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8081),
               new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8082)
           };


        public static void StartTcpProxyServer(int port)
        {
            TcpListener listener = new TcpListener(IPAddress.Any, port);
            listener.Start();
            Console.WriteLine($"Proxy Server listening on port {port}...");

            while (true)
            {
                TcpClient client = listener.AcceptTcpClient();
                Task.Run(() => HandleClient(client));
            }
        }

        public static void HandleClient(TcpClient client)
        {
            try
            {
                Console.WriteLine("Client connected.");
                NetworkStream clientStream = client.GetStream();

                // Select a server to forward the request to (round-robin or random)  
                IPEndPoint targetServer = SelectServer();
                TcpClient serverClient = new TcpClient();
                serverClient.Connect(targetServer);
                NetworkStream serverStream = serverClient.GetStream();

                // Forward data between client and server  
                Task clientToServer = Task.Run(() => ForwardData(clientStream, serverStream));
                Task serverToClient = Task.Run(() => ForwardData(serverStream, clientStream));

                Task.WaitAll(clientToServer, serverToClient);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error handling client: {ex.Message}");
            }
            finally
            {
                client.Close();
            }
        }

        private static IPEndPoint SelectServer()
        {
            // Simple round-robin selection  
            Random random = new Random();
            return serverEndpoints_[random.Next(serverEndpoints_.Count)];
        }

        private static void ForwardData(NetworkStream input, NetworkStream output)
        {
            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = input.Read(buffer, 0, buffer.Length)) > 0)
            {
                output.Write(buffer, 0, bytesRead);
            }
        }
    }
}
