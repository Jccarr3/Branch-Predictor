/*
overall need to knows:
    bit shift PC to the right by 2(least significant 2 bits can be ignored)
        this will be dome in sim_bp.cc to avoid having to do this in branch_predictor class
    m = number of low-order bits from PC used as index for Branch Prediction Table
    n = number of bits in global branch history
    Counter variable should not exceed 3 or go below 0

    must create vector to store branch outcomes(used in gshare and bimodal)
        vector size will be initialized to 2^m
        vector data will be initialized to 2 on startup

    must create vector for global branch history(used in gshare)
        vector size will be initialized to n
        vector data will be initialized to 0 on startup




Steps for Bimodal branch predictor(n = 0):
    set branch_table size 2^m, set all initial values to  2
    set gbh_table size to 0

    1. Determine branch's index into the table
    2. Make prediction based on the counter value at calculated index
    3. Update the branch based on the actual outcome of taken or not taken

Steps for gshare branch predictor(n > 0):
    set branch_table size to 2^m, set all initial values to  2
    set gbh_table size to n, set all intial values to 0

    1. Determine index bits
    2. Make prediction using index to get counter from predictor table
    3. Update branch prediction table based on outcome
    4. update global branch history register
*/
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <vector>

using namespace std;

class branch_predictor{
    vector<uint32_t> branch_table, gbh_table;
    uint32_t m,n;



    public:
        branch_predictor(uint32_t index_size, uint32_t type){               //takes in arguments: index size = m, and type = n
            

        }
};