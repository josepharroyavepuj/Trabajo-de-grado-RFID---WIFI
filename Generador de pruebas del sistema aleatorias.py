import pandas as pd
import random


# Etiquetas de equipos
equipos = [
    "Monitor de signos vitales",
    "Bomba de infusión",
    "Electrocardiógrafo",
    "Ventilador mecánico",
    "Ecógrafo"  # Reemplazando Desfibrilador
]

tags = [f"TAG-00{i+1}" for i in range(len(equipos))]

# Áreas con los nuevos nombres
areas = [
    "Área quirúrgica",
    "Álmacen",
    "UCI adulto - consultorio PED",
    "Trauma - UCI PED",
    "Hospitalización"
]

# Reiniciar el conjunto de datos
datos = {
    "Prueba": [],
    "Sistema": [],
    "Equipo": [],
    "TAG": [],
    "Área": []
}

# Generar datos aleatorios para cada prueba
for prueba in range(1, 13):  # Pruebas del 1 al 12
    sistema = "Con Sistema" if prueba % 2 == 0 else "Sin Sistema"
    for equipo, tag in zip(equipos, tags):
        area = random.choice(areas)
        datos["Prueba"].append(f"Prueba {prueba}")
        datos["Sistema"].append(sistema)
        datos["Equipo"].append(equipo)
        datos["TAG"].append(tag)
        datos["Área"].append(area)

# Crear DataFrame con la nueva prueba incluida
df = pd.DataFrame(datos)

# Guardar en un archivo Excel
output_file_with_prueba_7 = "/content/Pruebas_BLE_Con_Prueba_7.xlsx"
df.to_excel(output_file_with_prueba_7, index=False)

output_file_with_prueba_7
