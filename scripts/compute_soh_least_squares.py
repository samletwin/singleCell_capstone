import numpy as np
import os
import pandas as pd

def read_csv_data(file_path):
    df = pd.read_csv(file_path)
    return df['Battery_Current'].values, df['Battery_Voltage'].values

def list_csv_files(folder_path):
    return [f for f in os.listdir(folder_path) if f.endswith('.csv')]

# Predefined folder containing CSV files
folder_path = os.path.join(os.getcwd(),"scripts","log","output")

def computeSoh(voltSamples: list, currentSamples: list, debug: bool = False):
    # Create the A matrix
    A_matrix = np.ones(shape=(len(voltSamples),2))
    A_matrix[:, 1] = currentSamples
    # Create the B vector
    B_vector = np.array(object=voltSamples)
    
    A_transpose = np.transpose(A_matrix)
    # Matrix multiplication
    step1 = np.matmul(A_transpose, A_matrix)
    
    # Compute inverse
    try:
        step2_inv = np.linalg.inv(step1)
    except np.linalg.LinAlgError:
        print("Error computing inverse of matrix for computing SOH.")
    
    result1 = np.matmul(step2_inv, A_transpose)
    result = np.dot(result1, B_vector)
    if (debug):
        print("A Matrix")
        print(A_matrix)
        print("B_vector")
        print(B_vector)
        print("A_t Matrix")
        print(A_transpose)
        print("Step 1 : A_t * A")
        print(step1)
        print("Step2: inverse step 1")
        print(step2_inv)
        print("Final Step 1: step_2_inv * A_t")
        print(result1)
        print("Final Step 2: result1 * B")
        print(result)
    return result

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
    result = computeSoh(voltage_measurements, current_measurements, False)
    print(f"Calculated OCV: {result[0]}, Calculated SOH: {result[1]}")
    

    
if __name__ == "__main__":
    main()


