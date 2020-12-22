# -*- coding: utf-8 -*-
"""
Created on Tue Dec 22 10:01:52 2020

@author: CCOYNE1
"""

import pandas as pd
import xlsxwriter

# Create a workbook to hold aggregated results
workbook = xlsxwriter.Workbook('summarized_results.xlsx')

# Write a function to pull, condense, and write our data to a new file
def organize_data(samples, tracks, cured = 'Yes'):

    # Loop through all of the Sample numbers 1 through # of samples
    for s in range(1,samples+1):

        # Loop through the Track numbers 0 and 1 per Sample
        for t in range(tracks):

            # Define our file path
            if cured == 'Yes':
                path = r'MeasurementData_Sample {} CURED_Track00{}.csv'.format(s, t)
            else:
                path = r'MeasurementData_Sample {}_Track00{}.csv'.format(s, t)

            # Read data from csv file into pandas dataframe
            df = pd.read_csv(path)
            # Title dataframe columns
            df.columns = ['Width', 'Height', 'Area', 'Rate']
            # Trim the data down to non-zero entries only
            df = df[df['Height'] > 0]
            
            # Create a worksheet in the workbook for the Sample and Track
            # Cater the worksheet name to whether or not the liquid gasket is cured
            if cured == 'Yes':
                worksheet = workbook.add_worksheet('Sample {} CURED Track {}'.format(s, t))
            else:
                worksheet = workbook.add_worksheet('Sample {} Track {}'.format(s,t))

            # Write the column headers to the new worksheet
            headers = ['Width', 'Height', 'Area', 'Rate']
            for i in range(4):
                worksheet.write(0, i, headers[i])

            # Write the dataframe to the new worksheet
            for i in range(len(df['Height'])):
                for col in range(4):
                    worksheet.write(i, col, df[headers[col]].iloc[i])

                # Collect and write the three measurements that will be used in the Design of Experiments (DOE) analysis
                # Start with width measurements
                worksheet.write(1, 5, 'Width')
                meas = ['1st Meas.', '2nd Meas.', '3rd Meas.']
                eqsWidth = ['=A226', '=A551', '=A776']
                for col in range(5,8):
                    worksheet.write(2, col, meas[col-5])
                    worksheet.write(3, col, eqsWidth[col-5])

                # Write the height measurements next
                worksheet.write(5, 5, 'Height')
                eqsHeight = ['=B226', '=B551', '=B776']
                for col in range(5,8):
                    worksheet.write(6, col, meas[col-5])
                    worksheet.write(7, col, eqsHeight[col-5])

# Pull data from all cured gasket files
organize_data(8, 2, 'Yes')
organize_data(8, 2, 'No')

# Close the workbook
workbook.close()