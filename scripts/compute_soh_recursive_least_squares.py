import numpy as np
import pandas as pd
import os

class OnlineLeastSquares:
    def __init__(self):
        # Initialize P as a large value to indicate initial uncertainty
        self.P = np.array([[1e6, 0], [0, 1e6]], dtype=np.float32)
        # Initialize phi as zero
        self.phi = np.zeros(2, dtype=np.float32)

    def update(self, current_mA, voltage_mV):
        # Construct new data point
        A = np.array([1, current_mA], dtype=np.float32)

        # Calculate Kalman gain
        PA = self.P @ A
        APA = A.T @ PA + 1
        K = PA / APA

        # Update phi
        y = voltage_mV - (A.T @ self.phi)
        self.phi = self.phi + K * y

        # Update P
        I = np.eye(2, dtype=np.float32)
        self.P = (I - np.outer(K, A)) @ self.P

    def get_results(self):
        return {
            'OCV_f32': self.phi[0],
            'internalResistance_f32': self.phi[1]
        }

def read_csv_data(file_path):
    df = pd.read_csv(file_path)
    return df['Battery_Current'].values, df['Battery_Voltage'].values

def list_csv_files(folder_path):
    return [f for f in os.listdir(folder_path) if f.endswith('.csv')]

# Predefined folder containing CSV files
folder_path = os.path.join(os.getcwd(),"scripts","log","output")

def main():
    # List CSV files in the predefined folder
    csv_files = list_csv_files(folder_path)
    # Display the CSV files for the user to choose
    print("Select a CSV file to plot:")
    for idx, file in enumerate(csv_files):
        print(f"{idx + 1}. {file}")

    print(f"{len(csv_files) + 1}. Enter full path to a different CSV file")

    # Get user's choice
    choice = int(input("Enter the number corresponding to your choice: "))

    if choice == len(csv_files) + 1:
        # User wants to enter a full path to a different CSV file
        file_path = input("Enter the full path to the CSV file: ")
    else:
        # User selects from the predefined folder
        file_path = os.path.join(folder_path, csv_files[choice - 1])
    
    if not os.path.exists(file_path):
        print(f"File {file_path} does not exist.")
        return

    current_measurements, voltage_measurements = read_csv_data(file_path)
    
    ols = OnlineLeastSquares()

    # Update the model with each measurement
    for current, voltage in zip(current_measurements, voltage_measurements):
        ols.update(current, voltage)
        results = ols.get_results()
        print(f"Updated results: OCV = {results['OCV_f32']}, Internal Resistance = {results['internalResistance_f32']}")

if __name__ == "__main__":
    main()
