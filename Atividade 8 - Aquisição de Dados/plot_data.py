import serial
import threading
from collections import deque
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# ==========================================
# CONFIGURAÇÕES DA PORTA SERIAL
# ==========================================
SERIAL_PORT = 'COM6'  # Troque para a sua porta (ex: 'COM5' no Windows ou '/dev/ttyUSB0' no Linux)
BAUD_RATE = 115200    # Troque para a velocidade que o seu dispositivo usa (ex: 9600, 115200)
# ==========================================

# Configurações do gráfico
MAX_POINTS = 100  # Número máximo de pontos mostrados na tela (janela deslizante)

# Filas para armazenar os dados em tempo real
t_data = deque(maxlen=MAX_POINTS)
nx_data = deque(maxlen=MAX_POINTS)
ny_data = deque(maxlen=MAX_POINTS)
nz_data = deque(maxlen=MAX_POINTS)
fx_data = deque(maxlen=MAX_POINTS)
fy_data = deque(maxlen=MAX_POINTS)
fz_data = deque(maxlen=MAX_POINTS)

def read_from_serial():
    """
    Função que roda em uma thread separada lendo os dados da porta Serial.
    """
    try:
        # Abre a conexão serial
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Conectado com sucesso na porta {SERIAL_PORT} a {BAUD_RATE} bps.")
        
        while True:
            # Lê uma linha da serial, decodifica para texto e remove espaços/quebras de linha
            try:
                line = ser.readline().decode('utf-8').strip()
            except UnicodeDecodeError:
                continue # Ignora lixo na serial que não seja texto válido

            if not line:
                continue
            
            try:
                # Separa os dados pelo ponto e vírgula
                parts = line.split(';')
                if len(parts) == 7:
                    t = float(parts[0])
                    nx, ny, nz = float(parts[1]), float(parts[2]), float(parts[3])
                    fx, fy, fz = float(parts[4]), float(parts[5]), float(parts[6])
                    
                    # Adiciona às listas
                    t_data.append(t)
                    nx_data.append(nx)
                    ny_data.append(ny)
                    nz_data.append(nz)
                    fx_data.append(fx)
                    fy_data.append(fy)
                    fz_data.append(fz)
            except ValueError:
                # Ignora linhas com formato incorreto
                pass
                
    except serial.SerialException as e:
        print(f"Erro ao conectar na porta serial: {e}")
        print("Verifique se a porta está correta e se não há outro programa usando ela (como o Monitor Serial do Arduino).")

# Inicia a thread de leitura da serial
thread = threading.Thread(target=read_from_serial, daemon=True)
thread.start()

# Configuração da figura e dos subplots (Eixo X, Y e Z)
fig, (ax_x, ax_y, ax_z) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
fig.canvas.manager.set_window_title('Acelerômetro via Serial')
fig.suptitle(f'Dados Normais vs Filtrados ({SERIAL_PORT})', fontsize=14)

# Eixo X
line_nx, = ax_x.plot([], [], label='Normal X', color='lightcoral', linestyle='--')
line_fx, = ax_x.plot([], [], label='Filtrado X', color='red')
ax_x.set_ylabel('Eixo X')
ax_x.legend(loc='upper right')
ax_x.grid(True)

# Eixo Y
line_ny, = ax_y.plot([], [], label='Normal Y', color='lightgreen', linestyle='--')
line_fy, = ax_y.plot([], [], label='Filtrado Y', color='green')
ax_y.set_ylabel('Eixo Y')
ax_y.legend(loc='upper right')
ax_y.grid(True)

# Eixo Z
line_nz, = ax_z.plot([], [], label='Normal Z', color='lightblue', linestyle='--')
line_fz, = ax_z.plot([], [], label='Filtrado Z', color='blue')
ax_z.set_ylabel('Eixo Z')
ax_z.set_xlabel('Timestamp (ms)')
ax_z.legend(loc='upper right')
ax_z.grid(True)

def update(frame):
    if len(t_data) == 0:
        return
    
    # Atualiza as linhas
    line_nx.set_data(t_data, nx_data)
    line_fx.set_data(t_data, fx_data)
    
    line_ny.set_data(t_data, ny_data)
    line_fy.set_data(t_data, fy_data)
    
    line_nz.set_data(t_data, nz_data)
    line_fz.set_data(t_data, fz_data)
    
    # Ajusta limites
    for ax in (ax_x, ax_y, ax_z):
        ax.relim()
        ax.autoscale_view()
        ax.set_xlim(t_data[0], t_data[-1])

    return line_nx, line_fx, line_ny, line_fy, line_nz, line_fz

# Cria a animação
ani = animation.FuncAnimation(fig, update, interval=50, cache_frame_data=False)

plt.tight_layout()
plt.show()