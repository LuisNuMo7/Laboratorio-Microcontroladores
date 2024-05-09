import pandas as pd
import matplotlib.pyplot as plt
path="./Laboratorio3/Tensiones.csv"

df = pd.read_csv(path, delimiter=",")
print(df)
df.plot()
plt.title("Kp = 1, Ki = 0.3, Kd = 0.3")
plt.show()
