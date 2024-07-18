import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV file
file_path = r'C:\Users\Sam\Desktop\capstone\pipo\capstone_pio\scripts\log\output\arbin_plus_circuit_log.csv'
data = pd.read_csv(file_path)

# Function to plot data
def plot_data(battery_start_time, arbin_start_time):
    # Filter data based on start times
    battery_data = data[data['Elapsed_Time'] >= battery_start_time]
    battery_data['Elapsed_Time'] = battery_data['Elapsed_Time'] - battery_start_time
    arbin_data = data[data['Arbin_Time'] >= arbin_start_time]

    # Create subplots
    fig, axs = plt.subplots(2, 1, figsize=(12, 12))

    # Plot Battery Voltage vs Arbin Voltage
    axs[0].plot(battery_data['Elapsed_Time'], battery_data['Battery_Voltage'], label='Battery Voltage (mV)')
    axs[0].plot(arbin_data['Arbin_Time'], arbin_data['Arbin_Voltage']*1000, label='Arbin Voltage (mV)', linestyle='--')
    axs[0].set_xlabel('Elapsed Time (s)')
    axs[0].set_ylabel('Voltage (mV)')
    axs[0].set_title('Battery Voltage vs Arbin Voltage')
    axs[0].legend()
    axs[0].grid(True)

    # Plot Battery Current vs Arbin Current
    axs[1].plot(battery_data['Elapsed_Time'], battery_data['Battery_Current'], label='Battery Current (mA)')
    axs[1].plot(arbin_data['Arbin_Time'], arbin_data['Arbin_Current']*1000, label='Arbin Current (mA)', linestyle='--')
    axs[1].set_xlabel('Elapsed Time (s)')
    axs[1].set_ylabel('Current (mA)')
    axs[1].set_title('Battery Current vs Arbin Current')
    axs[1].legend()
    axs[1].grid(True)

    # Adjust layout
    plt.tight_layout()
    plt.show()

# Example usage
battery_start_time = 21.5  # Modify this to your desired start time for battery data
arbin_start_time = 0  # Modify this to your desired start time for Arbin data
plot_data(battery_start_time, arbin_start_time)
