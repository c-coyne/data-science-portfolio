# -*- coding: utf-8 -*-
"""
Created on Wed Dec 16 17:13:01 2020

@author: CCOYNE1
"""

# Many PDFs can be read and processed with PyPDF2, which is
# a very straightforward library for working with PDFs in Python.
# In the case of unstructured PDFs, however, we'll use the Tika
# library instead.

# Import a library designed to read PDFs
from tika import parser

# Convert the PDF to plain text
file = input("Input file name: ")
text = parser.from_file(file)

# If desired, un-comment the text below to write the plain text to a text file
#f = open('filename.txt', 'w')
#f.write(text['content'])
#f.close()

# Import the regular expressions (regex) library
import re

# Write a function for each of the things we're looking for before extracting data

# Discretionary
def find_discretionary(text):
    
    # Look through the plain text and scan for a line starting with "Discretionary: "
    find_disc = re.search(r'(?<=Discretionary: ).*', text['content'])

    # In the line found, find a number starting with "$ "
    find_inner_disc = re.search(r'[$] \S+', find_disc.group())
    
    # Return the discretionary value
    return find_inner_disc.group()

# Administrator
def find_admin(text):
    
    # Look through the plain text and scan for a line starting with 
    # "Name of administrator" and ending with "Location"
    find_admin = re.search(r'Name of administrator(.*?)Location', text['content'], re.DOTALL)
    
    # Discard the extra text that the search picked up above
    admin = find_admin.group()[24:-14]
    
    # Return the name of the administrator
    return admin

# Custodian
def find_cust(text):
    
    # Look through the plain text and scan for a line starting with
    # "Legal name of custodian" and ending with "Primary"
    find_cust = re.search(r'Legal name of custodian(.*?)Primary', text['content'], re.DOTALL)
    
    # Discard the extra text that the search picked up above
    custodian = find_cust.group()[26:-13]
    
    # Return the name of the custodian, removing any lingering newline characters
    return custodian.lstrip()

# Prime brokers
def find_pb(text):
    
    # Look through the plain text and scan for a line starting with
    # "Name of the prime broker" and ending with "If the prime"
    find_pb = re.findall(r'Name of the prime broker(.*?)If the prime', text['content'], re.DOTALL)
    
    # Go through the list of all prime brokers found and clean up the matches
    pbs = []
    for item in find_pb:
        item_cleaned = item[2:-4].rstrip().lstrip()
        pbs.append(item_cleaned)

    # Identify the unique values in the list above
    unique_pbs = []
    for item in pbs:
        if item not in unique_pbs:
            unique_pbs.append(item)
    
    # Return a list of the prime brokers
    return unique_pbs

# Use the functions above to find the data we need
disc = find_discretionary(text)
admin = find_admin(text)
cust = find_cust(text)
pb = find_pb(text)

# Print the results of what we found
print("Discretionary: {}".format(disc))
print("Name of administrator: {}".format(admin))
print("Name of custodian: {}".format(cust))
if len(pb) > 0:
    print("Name of prime brokers:")
    for item in pb:
        print(item)
else:
    print("No prime brokers listed")
