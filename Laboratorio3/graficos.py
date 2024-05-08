import pandas as pd
import matplotlib.pyplot as plt
path="./Laboratorio3/Tensiones.csv"
# path="Tensiones.csv"

df = pd.read_csv(path, delimiter=",")
print(df)
df.plot()
plt.show()
