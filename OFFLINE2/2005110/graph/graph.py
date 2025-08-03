
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Load the CSV data
file_path = "2005110_CCAODV.csv"  # Replace with your actual file path
aodv_file_path = "2005110_AODV.csv"
data = pd.read_csv(file_path)
aodv_data = pd.read_csv(aodv_file_path)

# Columns for plotting
x_column = "Nodes"  # 3rd column (x-axis)
y_columns = ["Throughput", "EndToEndDelay", "DeliveryRatio", "DropRatio"]  # Columns to plot against x-axis
first_column = "Speed"  # 1st column
second_column = "PacketsPerSecond"  # 2nd column

# Check the data to ensure correct loading
print("Data Overview:")
print(data.head())  # Print the first few rows to check if the data is loaded correctly

# Multiply Throughput by 100 (this is critical for proper scaling)
data["Throughput"] = data["Throughput"] * (100)  # Modify the 'Throughput' column
aodv_data["Throughput"] = aodv_data["Throughput"] * (100)

# Check if multiplication worked properly
print("Data with modified Throughput:")
print(data[["Nodes", "Throughput"]])
print(aodv_data[["Nodes", "Throughput"]])

# Remove the first row, as it contains the column names
data = data.iloc[1:].reset_index(drop=True)  # Reset index after removing the first row
aodv_data = aodv_data.iloc[1:].reset_index(drop=True)

# Create a PDF to save the plots
output_pdf = "2005110.pdf"  # Name of the output PDF
with PdfPages(output_pdf) as pdf:
    # Iterate through every 4 rows at a time
    for start in range(0, len(data), 4):  # Process every 4th row
        plt.figure(figsize=(10, 6))  # Create a new figure for each graph

        # Select the subset of 4 rows
        subset = data.iloc[start:start + 4]  # Select 4 rows
        aodv_subset = aodv_data.iloc[start:start + 4]

        # Ensure that the data is sorted by 'Nodes' to connect the points correctly
        subset = subset.sort_values(by=x_column)
        aodv_subset = aodv_subset.sort_values(by=x_column)


        # Plot each graph for this subset of 4 rows
        for y_column in y_columns:
            plt.figure(figsize=(10, 6))  # Create a new figure for each plot

            # Plot the data for the current subset
            plt.plot(
                subset[x_column],
                subset[y_column],
                marker='o',
                label=f'{x_column} vs {y_column} (Rows {start + 1} to {start + 4})',
                color='blue'
            )

            plt.plot(
                aodv_subset[x_column],
                aodv_subset[y_column],
                marker='o',
                label=f'{x_column} vs {y_column} (Rows {start + 1} to {start + 4})',
                color='red'
            )


            # Add the title with the first three columns values
            title = f"{first_column}: {data[first_column][start]}, {second_column}: {data[second_column][start]}"
            plt.title(f"Graph: {x_column} vs {y_column}\n{title}", fontsize=14)

            # Annotate the y-values at each point with 6 decimal places
            for i, row in subset.iterrows():
                plt.text(
                    row[x_column],
                    row[y_column],
                    f"{row[y_column]:.6f}",  # Format the y-values to 6 decimal places
                    fontsize=9,
                    color='red',
                    ha='center',
                    va='bottom'  # Position the annotation slightly above the point
                )

            for i, row in aodv_subset.iterrows():
                plt.text(
                    row[x_column],
                    row[y_column],
                    f"{row[y_column]:.6f}",  # Format the y-values to 6 decimal places
                    fontsize=9,
                    color='red',
                    ha='center',
                    va='bottom'  # Position the annotation slightly above the point
                )

            # Set the axis labels, grid, and ensure y=0 is at the bottom
            plt.xlabel(x_column)
            plt.ylabel(y_column)
            plt.grid(True)
            plt.legend()

            # Adjust y-limits dynamically based on the min and max values in the y-column
            # y_min = subset[y_column].min()  # Find the minimum y value for the current subset
            # y_max = subset[y_column].max()  # Find the maximum y value for the current subset
            y_min = min(subset[y_column].min(), aodv_subset[y_column].min())  # Get the min value for both datasets
            y_max = max(subset[y_column].max(), aodv_subset[y_column].max())
            plt.ylim(y_min - 0.1 * (y_max - y_min), y_max + 0.1 * (y_max - y_min))  # Add a 10% padding on both sides

            # Reverse the y-axis to start from 0 at the bottom
            #plt.gca().invert_yaxis()

            # Save each plot to the PDF (each plot will be on a new page)
            pdf.savefig()
            plt.close()  # Close the figure to free memory

print(f"Graphs have been saved to {output_pdf}")
