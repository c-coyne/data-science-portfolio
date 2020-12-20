# Data Science Portfolio

This repository contains my portfolio of data science projects that I completed either for professional purposes or for self-development and education. I completed most projects in Python using Jupyter Notebooks, but a few use other laguages (such as C++ / Arduino).

## Contents

### Machine Learning

   - [Linear Regression Analysis (US Small Cities' Health)](https://github.com/c-coyne/data_science_portfolio/tree/main/Machine%20Learning/Project%201%20-%20Linear%20Regression%20(US%20Small%20Cities'%20Health)): A univariate linear regression model developed via gradient descent techniques for cost function optimization to identify and predict relationships between the death rate in US small towns as a function of various healthcare metrics. This project includes an Exploratory Data Analysis (EDA) and various statitical tools, such as hypothesis testing.
  - [Multi-Class Classification (Handwritten Digits)](https://github.com/c-coyne/data_science_portfolio/tree/main/Machine%20Learning/Project%202%20-%20Multi-Class%20Classification%20(Handwritten%20Digits)): A logistic regression multi-class classifier model trained and tested to read handwritten digits. The model is built from scratch and includes a discussion of the sigmoid hypothesis function, constituents of the cost function, vectorization, and model assessment if the model were to be improved.
  - [Support Vector Machines (Heart Disease Diagnosis)](https://github.com/c-coyne/data_science_portfolio/tree/main/Machine%20Learning/Project%203%20-%20SVM%20Classifiers%20(Heart%20Disease%20Diagnosis)): A support vector machine (SVM) classifier built to diagnose heart disease in patients based on other patient health information. The project includes an example linear and non-linear 2D SVM classifier to exemplify the concept, then applies the same principles to a 13-attribute dataset. The hyperparameters are then optimized to improve model performance.
  
  *Tools: scikit-learn, Pandas, Seaborn, Matplotlib, Numpy, Scipy, Plotly*

### Data Cleaning, Processing & Extracting

   - [Scraping Data off Unstructured PDFs (SEC ADV Forms)](https://github.com/c-coyne/data_science_portfolio/tree/main/Data%20Cleaning%2C%20Processing%20%26%20Extracting/Scraping%20Data%20off%20Unstructured%20PDFs%20(SEC%20ADV%20Forms)): Python script that uses the Tika library to parse PDFs and RegEx to pull data of interest. This script is specific to Form ADV (Uniform Application for Investment Adviser Registration and Report by Exempt Reporting Advisers) from the U.S. Securities and Exchange Commission (SEC).
   
   *Tools: Tika, RegEx*

### Applied Data Analysis

   - [Capability Study Analysis (Cure-in-Place Gaskets)](https://github.com/c-coyne/data_science_portfolio/tree/main/Applied%20Data%20Analysis/Cure-in-Place%20Gasket%20Process%20Capability): Data cleaning and analysis for process capability of a liquid-dispensed cure-in-place gasket. The capability study included 5 parts measured with a robot-mounted vision system tracing dry and wet bead height/width/volume, resulting in 128,072 datapoints.
   
   *Tools: Pandas, Numpy, Scipy, Matplotlib*

### Embedded Systems

   - [Standalone Benchtop Spin Stand](https://github.com/c-coyne/data_science_portfolio/tree/main/Embedded%20Systems/Benchtop%20Spin%20Stand): C++ / Arduino code to operate a stand-alone benchtop spin stand designed to evaluate gasket retention when subjected to force experienced during shipping. The spin stand includes 24V, 12V and 5V circuits; internal LEDs; multiple potentiometers to facilitate user input; dual 20x4 LCD screens with I2C serial interface adapters; front and rear fan circuits; and multiple switches for real-time customization of the testing equipment.
   
   *Tools: LiquidCrystal_I2C, Wire, attachInterrupt*
