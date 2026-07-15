#include <windows.h>
#include <iostream>
#include <string>

struct ProcesoHijo {
    HANDLE hEscribir;  // C++ escribe aquí -> va al stdin de Python
    HANDLE hLeer;       // C++ lee aquí <- viene del stdout de Python
    PROCESS_INFORMATION pi;
};

ProcesoHijo iniciarPython(const std::wstring& script) {
    HANDLE hEntradaLeer, hEntradaEscribir;
    HANDLE hSalidaLeer, hSalidaEscribir;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Pipe para el stdin del hijo (nosotros escribimos, el hijo lee)
    CreatePipe(&hEntradaLeer, &hEntradaEscribir, &sa, 0);
    SetHandleInformation(hEntradaEscribir, HANDLE_FLAG_INHERIT, 0);

    // Pipe para el stdout del hijo (el hijo escribe, nosotros leemos)
    CreatePipe(&hSalidaLeer, &hSalidaEscribir, &sa, 0);
    SetHandleInformation(hSalidaLeer, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    si.hStdInput = hEntradaLeer;
    si.hStdOutput = hSalidaEscribir;
    si.hStdError = hSalidaEscribir;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi = {};

    std::wstring cmd = L"\"C:\\Users\\marti\\proyectos\\JTG\\comunicacion\\venv\\Scripts\\python.exe\"" L" -u \"" + script + L"\"";

    CreateProcessW(
        NULL, &cmd[0], NULL, NULL,
        TRUE, 0, NULL, NULL, &si, &pi
    );

    // El padre no necesita estos extremos
    CloseHandle(hEntradaLeer);
    CloseHandle(hSalidaEscribir);

    return { hEntradaEscribir, hSalidaLeer, pi };
}

std::string leerLinea(HANDLE hLeer) {
    std::string resultado;
    char c;
    DWORD leidos;
    while (ReadFile(hLeer, &c, 1, &leidos, NULL) && leidos > 0) {
        if (c == '\n') break;
        if (c != '\r') resultado += c;
    }
    return resultado;
}

void escribirLinea(HANDLE hEscribir, const std::string& texto) {
    std::string linea = texto + "\n";
    DWORD escritos;
    WriteFile(hEscribir, linea.c_str(), (DWORD)linea.size(), &escritos, NULL);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);  // la consola interpreta UTF-8 al mostrar
    SetConsoleCP(CP_UTF8); 
    ProcesoHijo py = iniciarPython(L"ia_groq.py");

    std::string pregunta = "Explica que es la entropia en una frase";
    std::cout << "Preguntando: " << pregunta << "\n";

    escribirLinea(py.hEscribir, pregunta);
    std::string respuesta = leerLinea(py.hLeer);

    std::cout << "IA respondio: " << respuesta << "\n";

    // Otra pregunta en la misma sesion
    escribirLinea(py.hEscribir, "Y la de Boltzmann?");
    respuesta = leerLinea(py.hLeer);
    std::cout << "IA respondio: " << respuesta << "\n";

    CloseHandle(py.hEscribir); // cierra stdin del hijo -> Python sale del bucle
    WaitForSingleObject(py.pi.hProcess, INFINITE);

    CloseHandle(py.pi.hProcess);
    CloseHandle(py.pi.hThread);
    CloseHandle(py.hLeer);
    system("pause");
    return 0;
}