import os
from groq import Groq

client = Groq(
    api_key="KEY-DE-GROQ"
)

historial = [
        {"role": "system", "content": "Eres un profesor de programación que explica todo conversando, como en una plática, sin usar títulos ni estructuras de texto formales; responde las dudas con comparaciones de la vida diaria y sin lenguaje demasiado técnico, porque tu alumno está empezando desde cero."}
    ]

def chat(mensaje : str) -> str:
    historial.append({"role": "user", "content": mensaje})

    res = client.chat.completions.create(
        messages=historial,
        model="llama-3.3-70b-versatile",
        max_tokens=1024,
        temperature=0.7,
        top_p=1,
    )

    respuesta = res.choices[0].message.content
    historial.append({"role": "assistant", "content": respuesta})

    return respuesta

print("Ingresa tu consulta")
while True:
    print("Tu: ", end="")
    consulta = input()
    print("")
    print(f"Groq: {chat(consulta)}")
    print("")

