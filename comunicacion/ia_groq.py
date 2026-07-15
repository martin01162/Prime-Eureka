import sys
import os
import io

# Forzar que stdout/stdin usen UTF-8 sin importar la codificación por defecto del sistema
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', newline='\n')
sys.stdin = io.TextIOWrapper(sys.stdin.buffer, encoding='utf-8', newline='\n')

from groq import Groq

# La API key se lee de la variable de entorno GROQ_API_KEY
client = Groq(api_key="TU_API_KEY")

MODELO = "llama-3.3-70b-versatile"  # podés cambiarlo por otro modelo de Groq

def preguntar(pregunta: str) -> str:
    respuesta = client.chat.completions.create(
        model=MODELO,
        messages=[
            {"role": "system", "content": "Responde de forma breve y directa."},
            {"role": "user", "content": pregunta}
        ],
        temperature=0.7,
    )
    return respuesta.choices[0].message.content.strip()

def main():
    # Modo bucle: lee una pregunta por línea desde stdin, responde por stdout
    while True:
        linea = sys.stdin.readline()
        if not linea:  # EOF, el pipe se cerró
            break
        pregunta = linea.strip()
        if pregunta == "" or pregunta.lower() == "salir":
            break

        try:
            texto = preguntar(pregunta)
        except Exception as e:
            texto = f"ERROR: {e}"

        # IMPORTANTE: sin saltos de línea internos que rompan el protocolo,
        # y flush inmediato para que C++ no se quede esperando
        texto_una_linea = texto.replace("\n", " ")
        print(texto_una_linea)
        sys.stdout.flush()

if __name__ == "__main__":
    main()