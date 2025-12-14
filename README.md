# Bimodal-Branch-Predictor
A C++ simulator for evaluating dynamic branch prediction techniques, including Bimodal, Gshare, and Hybrid branch predictors. The simulator processes real branch trace files and reports prediction accuracy, misprediction rates, and final predictor table contents.

This project was developed as part of ECE 463/563 – Computer Architecture at NC State University, and closely follows industry-standard branch prediction designs used in modern processors.

## Features

#### Bimodal Branch Predictor

- Configurable table size using 2-bit saturating counters

- Evaluates the impact of predictor size on accuracy

#### Gshare Branch Predictor

- XORs the global branch history with PC index bits

- Configurable global history length (n) and table size (m)

- Demonstrates how history length affects prediction accuracy

#### Hybrid Branch Predictor

- Combines bimodal and gshare predictors

- Uses a chooser table of 2-bit counters to select the better predictor dynamically

#### Detailed Statistics

- Total predictions

- Total mispredictions

- Misprediction rate

- Final contents of predictor tables

## Experimental Results

#### Key findings from the experiments (see report.pdf for full analysis):

- Increasing bimodal table size reduces misprediction rate until aliasing is eliminated.

- Gshare outperforms bimodal when sufficient table capacity is available.

- Global history can hurt accuracy at small table sizes but significantly improves accuracy at larger sizes.

- The hybrid predictor dynamically adapts to branch behavior by choosing between bimodal and gshare predictions.

#### Best observed configuration:

- Bimodal: m = 17, misprediction rate ≈ 11.17%

- Gshare: m = 20, n = 11, misprediction rate ≈ 6.37%

## Technologies Used

- C++

- STL containers (vector)

- Bitwise operations and masking

- Saturating counters

- Shell scripting for experiment automation
