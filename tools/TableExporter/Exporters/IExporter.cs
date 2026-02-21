using TableExporter.Models;

namespace TableExporter.Exporters;

public interface IExporter
{
    void Export(TableData table, string outputDir);
}
