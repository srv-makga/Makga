using System.Diagnostics;
using System.Text.Json;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AdminTool.Models;

namespace AdminTool.Controllers;

[Authorize]
public class ApiProxyController : Controller
{
    private readonly IHttpClientFactory _httpFactory;
    public ApiProxyController(IHttpClientFactory httpFactory) => _httpFactory = httpFactory;

    public IActionResult Index()
    {
        ViewData["Title"] = "API 호출";
        return View();
    }

    [HttpPost, ValidateAntiForgeryToken]
    public async Task<IActionResult> Send([FromBody] ApiCallRequest req)
    {
        var sw = Stopwatch.StartNew();
        try
        {
            using var client = _httpFactory.CreateClient();
            client.Timeout = TimeSpan.FromSeconds(30);

            var request = new HttpRequestMessage(new HttpMethod(req.Method), req.Url);

            // Auth
            if (req.AuthType == "Bearer" && !string.IsNullOrEmpty(req.AuthValue))
                request.Headers.Authorization = new("Bearer", req.AuthValue);
            else if (req.AuthType == "Basic" && !string.IsNullOrEmpty(req.AuthValue))
            {
                var encoded = Convert.ToBase64String(System.Text.Encoding.UTF8.GetBytes(req.AuthValue));
                request.Headers.Authorization = new("Basic", encoded);
            }

            // Custom headers
            if (!string.IsNullOrWhiteSpace(req.HeadersJson))
            {
                var headers = JsonSerializer.Deserialize<Dictionary<string, string>>(req.HeadersJson);
                if (headers != null)
                    foreach (var (k, v) in headers)
                        request.Headers.TryAddWithoutValidation(k, v);
            }

            // Body
            if (!string.IsNullOrWhiteSpace(req.Body) &&
                (req.Method is "POST" or "PUT" or "PATCH"))
            {
                var ct = request.Headers.Contains("Content-Type")
                    ? request.Headers.GetValues("Content-Type").First()
                    : "application/json";
                request.Content = new StringContent(req.Body, System.Text.Encoding.UTF8, ct);
            }

            var response = await client.SendAsync(request);
            sw.Stop();

            var body = await response.Content.ReadAsStringAsync();
            var respHeaders = response.Headers
                .Concat(response.Content.Headers)
                .ToDictionary(h => h.Key, h => string.Join(", ", h.Value));

            return Json(new ApiCallResponse
            {
                Success = true,
                StatusCode = (int)response.StatusCode,
                StatusText = response.ReasonPhrase,
                ResponseHeaders = JsonSerializer.Serialize(respHeaders),
                Body = body,
                ElapsedMs = sw.ElapsedMilliseconds,
            });
        }
        catch (Exception ex)
        {
            sw.Stop();
            return Json(new ApiCallResponse
            {
                Success = false,
                Error = ex.Message,
                ElapsedMs = sw.ElapsedMilliseconds,
            });
        }
    }
}
