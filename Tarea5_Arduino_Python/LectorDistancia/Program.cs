// =============================================================================
// TAREA 5 - Leer desde C# lo que sensa el Arduino   (parte 2 de 2)
// =============================================================================
// Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
// Profesor   : Luis Bessewell Feliz
// Estudiante : Cristian Carrera - Matricula 2024-1932
// Institucion: Instituto Tecnologico de Las Americas (ITLA)
//
// Se conecta al Arduino por el puerto serie, recibe las lecturas del sensor de
// distancia, las muestra en vivo, las guarda en un CSV y al terminar saca un
// informe con estadisticas.
//
// La parte 1 es el sketch de la placa, en la carpeta de al lado:
//     Tarea5_Distancia_Serial/Tarea5_Distancia_Serial.ino
//
// -----------------------------------------------------------------------------
// COMO SE EJECUTA
// -----------------------------------------------------------------------------
//   Desde Visual Studio 2022:  abrir LectorDistancia.csproj y pulsar F5.
//   Desde la terminal:         dotnet run
//                              dotnet run COM3      (si hay varios puertos)
//                              dotnet run 30        (captura 30 s y para sola)
//                              dotnet run COM3 30
//
//   Sin numero, corre hasta que se pulse Ctrl+C. Con numero, se para sola a
//   los segundos indicados: comodo para grabar, porque deja las dos manos
//   libres para mover el objeto delante del sensor.
//
// IMPORTANTE: el Monitor Serie del IDE de Arduino tiene que estar CERRADO. El
// puerto solo admite un programa a la vez.
//
// -----------------------------------------------------------------------------
// POR QUE HACE FALTA UN PAQUETE APARTE PARA EL PUERTO SERIE
// -----------------------------------------------------------------------------
// En el .NET Framework antiguo, System.IO.Ports venia incluido. En .NET moderno
// -que es multiplataforma- se saco a un paquete NuGet aparte, porque el puerto
// serie se implementa distinto en Windows y en Linux. Por eso el proyecto lleva
// la referencia a System.IO.Ports; sin ella, SerialPort no existe.
//
// -----------------------------------------------------------------------------
// QUE APORTA ESTE PROGRAMA QUE EL ARDUINO NO PUEDE HACER
// -----------------------------------------------------------------------------
// El UNO tiene 2 KB de RAM: no puede guardar un historico ni analizarlo. Lo que
// sabe hacer es medir el mundo fisico en tiempo real. Esta computadora es justo
// al reves: no toca el mundo fisico, pero le sobra memoria y disco.
//
//     Arduino  ->  sensa y avisa
//     C#       ->  recibe, guarda y analiza
//
// En clase el profesor lo planteo con el ejemplo de una maquina de coser: poner
// un sensor y medir "cuanto se detiene, que tan rapido hace". Eso es lo que
// calcula el informe del final.
//
// Y ademas es PROGRAMACION DISTRIBUIDA: dos programas, en dos maquinas, que no
// comparten memoria y se coordinan solo por mensajes.
// =============================================================================

using System.Globalization;
using System.IO.Ports;
using System.Text;

namespace LectorDistancia;

/// <summary>Una lectura ya interpretada, tal como llego de la placa.</summary>
record Lectura(int Numero, long Milis, double DistanciaCm, string Zona, string Estado);

static class Program
{
    const int Baudios = 9600;          // tiene que coincidir con el Serial.begin()
    const double SinEco = -1.0;        // lo que manda la placa cuando no ve nada
    const int PeriodoEnvioMs = 200;    // cada cuanto envia el sketch

    // La placa manda su cabecera UNA sola vez, al arrancar. Si no se llega a
    // ver, se usan estos nombres y se sigue adelante.
    static readonly string[] ColumnasPorDefecto =
        { "LECTURA", "MILIS", "DISTANCIA_CM", "ZONA", "ESTADO" };

    static readonly List<Lectura> Lecturas = new();
    static volatile bool _seguir = true;

    static int Main(string[] args)
    {
        // La placa manda los decimales con punto. Si la maquina esta en español,
        // el separador por defecto es la coma y double.Parse fallaria. Por eso
        // se fuerza la cultura invariable en todas las conversiones.
        CultureInfo.DefaultThreadCurrentCulture = CultureInfo.InvariantCulture;
        Console.OutputEncoding = Encoding.UTF8;

        Console.WriteLine(new string('=', 66));
        Console.WriteLine("TAREA 5 - Lectura del sensor de distancia desde C#");
        Console.WriteLine("Cristian Carrera - 2024-1932 - ITLA");
        Console.WriteLine(new string('=', 66));
        Console.WriteLine();

        // Los argumentos pueden venir en cualquier orden: lo que empiece por
        // COM es el puerto, y lo que sea un numero son los segundos a capturar.
        string? puerto = args.FirstOrDefault(a => a.StartsWith("COM",
                                    StringComparison.OrdinalIgnoreCase));
        int segundosLimite = 0;
        foreach (var a in args)
            if (int.TryParse(a, out int n) && n > 0) segundosLimite = n;

        puerto ??= DetectarPuerto();
        if (puerto is null) return 1;

        using var arduino = new SerialPort(puerto, Baudios)
        {
            ReadTimeout = 2000,
            // pyserial levanta DTR por su cuenta; SerialPort no. Sin esto la placa
            // no se reinicia y la numeracion arranca donde la dejo la sesion anterior.
            DtrEnable = true,
            NewLine = "\n"
        };

        try
        {
            arduino.Open();
        }
        catch (Exception e)
        {
            Console.WriteLine($"\nNo se pudo abrir {puerto}: {e.Message}");
            Console.WriteLine("Causas mas frecuentes:");
            Console.WriteLine("  - El Monitor Serie del IDE de Arduino esta abierto. Cierralo.");
            Console.WriteLine("  - Ese no es el puerto correcto.");
            return 1;
        }

        // Al abrir el puerto, la señal DTR REINICIA la placa: arranca de cero
        // justo cuando este programa se conecta.
        //
        // El orden de estas dos lineas importa y es facil equivocarse: hay que
        // descartar lo que haya AHORA -restos del arranque- y esperar DESPUES.
        // Al reves se borraria la cabecera, que para entonces ya llego, y como
        // la placa no la vuelve a mandar el programa se quedaria esperando una
        // linea que no va a volver.
        arduino.DiscardInBuffer();
        Console.WriteLine("Esperando a que la placa arranque (auto-reset por DTR)...");
        Thread.Sleep(2500);

        // Ctrl+C no debe matar el proceso de golpe: hay que guardar antes.
        Console.CancelKeyPress += (_, e) => { e.Cancel = true; _seguir = false; };

        Console.WriteLine("\nLeyendo. Mueve la mano delante del sensor.");
        if (segundosLimite > 0)
            Console.WriteLine($"Se detendra sola en {segundosLimite} s.\n");
        else
            Console.WriteLine("Ctrl+C para terminar y ver el informe.\n");

        var reloj = System.Diagnostics.Stopwatch.StartNew();
        Console.WriteLine($"{"#",5}  {"tiempo",8}  {"distancia",10}  {"zona",-9}  estado");
        Console.WriteLine(new string('-', 60));

        string[]? columnas = null;
        int descartadas = 0;

        while (_seguir)
        {
            if (segundosLimite > 0 && reloj.Elapsed.TotalSeconds >= segundosLimite)
                break;

            string cruda;
            try
            {
                cruda = arduino.ReadLine().Trim();
            }
            catch (TimeoutException)
            {
                continue;               // no llego nada en 2 s; se vuelve a mirar
            }
            catch (Exception)
            {
                break;                  // puerto cerrado o desconectado
            }

            if (cruda.Length == 0) continue;

            if (columnas is null)
            {
                if (cruda.StartsWith("LECTURA"))
                {
                    columnas = cruda.Split(',').Select(x => x.Trim()).ToArray();
                    Console.WriteLine($"(cabecera recibida: {string.Join(", ", columnas)})\n");
                    continue;
                }

                // No es la cabecera. Si aun asi parece un dato completo, se sigue
                // con los nombres conocidos en vez de esperar para siempre.
                if (Parsear(cruda, ColumnasPorDefecto) is not null)
                {
                    columnas = ColumnasPorDefecto;
                    Console.WriteLine("(no se vio la cabecera; se usan los nombres conocidos)\n");
                }
                else
                {
                    continue;
                }
            }

            var lectura = Parsear(cruda, columnas);
            if (lectura is null) { descartadas++; continue; }

            Lecturas.Add(lectura);

            string textoDistancia = lectura.DistanciaCm == SinEco
                ? "sin eco"
                : $"{lectura.DistanciaCm:F1} cm";

            Console.WriteLine($"{lectura.Numero,5}  {lectura.Milis / 1000.0,7:F1}s  " +
                              $"{textoDistancia,10}  {lectura.Zona,-9}  {lectura.Estado}");
        }

        arduino.Close();
        Console.WriteLine("\n\nDetenido.");

        if (descartadas > 0)
            Console.WriteLine($"({descartadas} lineas incompletas descartadas, es normal al arrancar)");

        if (Lecturas.Count == 0)
        {
            Console.WriteLine("No llego ningun dato valido. Comprueba que subiste el sketch");
            Console.WriteLine("Tarea5_Distancia_Serial a la placa.");
            return 1;
        }

        GuardarCsv();
        Informe();
        return 0;
    }

    // =========================================================================
    // 1. Encontrar la placa
    // =========================================================================
    static string? DetectarPuerto()
    {
        string[] puertos = SerialPort.GetPortNames();

        if (puertos.Length == 0)
        {
            Console.WriteLine("No se detecto ningun puerto serie.");
            Console.WriteLine("Revisa que la placa este conectada por USB.");
            Console.WriteLine("Con placas clonicas suele faltar el driver del CH340.");
            return null;
        }

        Console.WriteLine("Puertos disponibles:");
        foreach (var p in puertos) Console.WriteLine($"   {p}");

        if (puertos.Length == 1)
        {
            Console.WriteLine($"\nSolo hay un puerto; se usara {puertos[0]}");
            return puertos[0];
        }

        Console.WriteLine("\nHay varios puertos. Indica cual, por ejemplo:");
        Console.WriteLine("   dotnet run COM3");
        return null;
    }

    // =========================================================================
    // 2. Interpretar una linea
    // =========================================================================
    /// <summary>
    /// Convierte una linea CSV en una Lectura. Devuelve null si no sirve.
    /// Se comprueba el numero de campos ANTES de convertir nada: asi una linea
    /// partida por la mitad se descarta en vez de reventar el programa.
    /// </summary>
    static Lectura? Parsear(string linea, string[] columnas)
    {
        string[] partes = linea.Split(',');
        if (partes.Length != columnas.Length) return null;

        for (int i = 0; i < partes.Length; i++) partes[i] = partes[i].Trim();

        // Se localiza cada campo POR NOMBRE, no por posicion: si mañana la placa
        // cambia el orden de las columnas, esto sigue funcionando.
        int Indice(string nombre) => Array.IndexOf(columnas, nombre);

        int iNum = Indice("LECTURA"), iMs = Indice("MILIS"),
            iDist = Indice("DISTANCIA_CM"), iZona = Indice("ZONA"),
            iEst = Indice("ESTADO");

        if (iNum < 0 || iMs < 0 || iDist < 0 || iZona < 0 || iEst < 0) return null;

        if (!int.TryParse(partes[iNum], out int numero)) return null;
        if (!long.TryParse(partes[iMs], out long milis)) return null;
        if (!double.TryParse(partes[iDist], NumberStyles.Float,
                             CultureInfo.InvariantCulture, out double dist)) return null;

        return new Lectura(numero, milis, dist, partes[iZona], partes[iEst]);
    }

    // =========================================================================
    // 3. Guardar
    // =========================================================================
    /// <summary>
    /// Visual Studio no ejecuta desde la carpeta del proyecto sino desde la de
    /// compilacion, asi que el CSV acabaria enterrado ahi dentro. Se sube por el
    /// arbol hasta dar con la carpeta que contiene el .csproj.
    /// </summary>
    static string CarpetaDelProyecto()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir is not null)
        {
            if (dir.GetFiles("*.csproj").Length > 0) return dir.FullName;
            dir = dir.Parent;
        }
        return Directory.GetCurrentDirectory();
    }

    static void GuardarCsv()
    {
        string nombre = $"datos_distancia_{DateTime.Now:yyyyMMdd_HHmmss}.csv";
        nombre = Path.Combine(CarpetaDelProyecto(), nombre);
        using var w = new StreamWriter(nombre, false, Encoding.UTF8);
        w.WriteLine("LECTURA,MILIS,DISTANCIA_CM,ZONA,ESTADO");
        foreach (var l in Lecturas)
            w.WriteLine($"{l.Numero},{l.Milis}," +
                        $"{l.DistanciaCm.ToString("F1", CultureInfo.InvariantCulture)}," +
                        $"{l.Zona},{l.Estado}");

        Console.WriteLine($"\n{Lecturas.Count} lecturas guardadas en  {Path.GetFileName(nombre)}");
        Console.WriteLine($"   ({Path.GetDirectoryName(nombre)})");
    }

    // =========================================================================
    // 4. El informe: que se puede sacar de los datos
    // =========================================================================
    static void Informe()
    {
        int total = Lecturas.Count;
        double segundos = total * PeriodoEnvioMs / 1000.0;

        // Las distancias reales, sin los -1 de "no vi nada".
        var validas = Lecturas.Where(l => l.DistanciaCm != SinEco)
                              .Select(l => l.DistanciaCm).ToList();

        Console.WriteLine("\n" + new string('=', 66));
        Console.WriteLine("INFORME DE LA SESION");
        Console.WriteLine(new string('=', 66));
        Console.WriteLine($"  Lecturas recibidas ....... {total}");
        Console.WriteLine($"  Duracion ................. {segundos:F1} s");

        if (validas.Count > 0)
        {
            Console.WriteLine($"  Distancia minima ......... {validas.Min():F1} cm");
            Console.WriteLine($"  Distancia maxima ......... {validas.Max():F1} cm");
            Console.WriteLine($"  Distancia media .......... {validas.Average():F1} cm");
        }
        int sinEco = total - validas.Count;
        Console.WriteLine($"  Lecturas sin eco ......... {sinEco}  ({sinEco * 100.0 / total:F0} %)");

        // --- Tiempo en cada zona --------------------------------------------
        Console.WriteLine("\n  Tiempo en cada zona:");
        foreach (var g in Lecturas.GroupBy(l => l.Zona).OrderByDescending(g => g.Count()))
        {
            double seg = g.Count() * PeriodoEnvioMs / 1000.0;
            double pct = g.Count() * 100.0 / total;
            string barra = new string('#', (int)Math.Round(g.Count() * 34.0 / total));
            Console.WriteLine($"     {g.Key,-9} {seg,6:F1} s  {pct,5:F1} %  {barra}");
        }

        // --- La maquina: en marcha o detenida -------------------------------
        // Esto es lo que el profesor planteo con el ejemplo de la maquina de
        // coser: medir cuanto trabaja y cuanto se detiene.
        int enMarcha = Lecturas.Count(l => l.Estado == "EN_MARCHA");
        int detenida = total - enMarcha;

        Console.WriteLine("\n  Estado de la maquina:");
        Console.WriteLine($"     EN MARCHA {enMarcha * PeriodoEnvioMs / 1000.0,6:F1} s  " +
                          $"({enMarcha * 100.0 / total:F0} %)");
        Console.WriteLine($"     DETENIDA  {detenida * PeriodoEnvioMs / 1000.0,6:F1} s  " +
                          $"({detenida * 100.0 / total:F0} %)");

        // --- Cuantas veces tuvo que parar -----------------------------------
        // Se cuentan las TRANSICIONES, no las lecturas: pasar de EN_MARCHA a
        // DETENIDA es UNA parada, aunque dure veinte lecturas seguidas.
        int paradas = 0, racha = 0, rachaLarga = 0;
        string? anterior = null;
        foreach (var l in Lecturas)
        {
            if (l.Estado == "DETENIDA")
            {
                if (anterior != "DETENIDA") { paradas++; racha = 0; }
                racha++;
                rachaLarga = Math.Max(rachaLarga, racha);
            }
            anterior = l.Estado;
        }

        Console.WriteLine($"\n     Veces que se detuvo ... {paradas}");
        if (paradas > 0)
            Console.WriteLine($"     Parada mas larga ...... {rachaLarga * PeriodoEnvioMs / 1000.0:F1} s");
        Console.WriteLine(new string('=', 66));
    }
}
