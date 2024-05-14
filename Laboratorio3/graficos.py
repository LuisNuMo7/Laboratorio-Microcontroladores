import pandas as pd
import matplotlib.pyplot as plt
path="./Laboratorio3/Tensiones0_1.csv"

df = pd.read_csv(path, delimiter=",")
print(df)
df.plot()
plt.xlabel("Tiempo (ms)")
plt.ylabel("Temperatura (C)")
plt.title("Kp = 1, Ki = Kd = 0.1")
plt.show()
