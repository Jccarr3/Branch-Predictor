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
#include <string.h>

using namespace std;

class branch_predictor{
    vector<uint32_t> branch_table;
    uint32_t gbh_reg, index_mask, index_bits, gbh_bits, chooser_bits, bimodal_bits, predict_taken = 0, predict_not = 0, mispredictions = 0, predictions = 0;
    char* type;
    



    public:
        branch_predictor(char* style, uint32_t m, uint32_t n = 0, uint32_t k = 0, uint32_t x = 0){               //takes in arguments: index size = m, and type = n
            //First set tables and important variables
            index_bits = m;
            gbh_bits = n;
            chooser_bits = k;
            bimodal_bits = x;
            type = style;
            index_mask = pow(2,index_bits) - 1;

            if(gbh_bits < 1){               //if bimodal branch predictor
                branch_table.resize(pow(2,index_bits));        //branch table size = m
            }
            else{
                branch_table.resize(pow(2,index_bits));        //branch table size = m
                gbh_reg = 0;
            }

            if(strcmp(type, "hybrid") == 0){
                //code for creating hybrid table
            }

            for(uint32_t i = 0; i < branch_table.size(); i++){
                branch_table[i] = 2;
            }
            //Set tables and important variables^^^^^^^^^
        }

        void predict(uint32_t shifted_PC, char result){
            predictions++;      //incremenent number of predictions made
            //code for bimodal prediction
            if(gbh_bits < 1){           
                uint32_t index = shifted_PC & index_mask;           //find index from PC using Mask

                //make prediction based on table
                if(branch_table[index] >= 2){
                    predict_taken++;
                    if(result == 'n'){          //update mispredictions if result does not match
                        mispredictions++;
                    }
                }
                else{
                    predict_not++;
                    if(result == 't'){
                        mispredictions++;
                    }
                }
                //^^^^^^^^^^^^^^^^^^^^^

                //update table for predictions
                if((result == 't') && (branch_table[index] < 3)){
                    branch_table[index]++;
                }
                else if((result == 'n') && (branch_table[index] > 0)){
                    branch_table[index]--;
                }
                return;
                //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            }
            //Code for Bimodal Prediction^^^^^^^^^^^^^^


            //Code for gshare prediction
            if(strcmp(type,"gshare") == 0){
                //caclulate index
                uint32_t gbh_temp = gbh_reg << (index_bits - gbh_bits);       //allign gbh with upper n bits from m
                uint32_t index = gbh_temp ^ (shifted_PC & index_mask);
                //calculate index^^^^^^^^^^

                //make prediction & adjust variables
                if(branch_table[index] >= 2){
                    predict_taken++;
                    if(result == 'n'){
                        mispredictions++;
                    }
                }
                else{
                    predict_not++;
                    if(result == 't'){
                        mispredictions++;
                    }
                }
                //make prediction & adjust variables^^^^^^^^^^^

                //adjust branch table
                if((result == 't') && (branch_table[index] < 3)){
                    branch_table[index]++;
                }
                else if((result == 'n') && (branch_table[index] > 0)){
                    branch_table[index]--;
                }
                //adjust branch table

                //adjust gbh register
                gbh_reg = gbh_reg >> 1;
                if(result == 't'){
                    uint32_t gbh_mask = (uint32_t)pow(2,gbh_bits-1);
                    gbh_reg = gbh_reg | gbh_mask;
                }
                //adjust gbh register^^^
            }
            //code for gshare prediction^^^^^^^^^^
        }


        void print_contents(){
            printf("OUTPUT\n");
            printf("number of predictions:   %d\n", predictions);
            printf("number of mispredictions:  %d\n", mispredictions);
            printf("misprediction rate:        %.2f%%\n", 100*((double)mispredictions/(double)predictions));
            if(strcmp(type, "bimodal") == 0){
                printf("FINAL BIMODAL CONTENTS\n");
            }
            if(strcmp(type, "gshare") == 0){
                printf("FINAL GSHARE CONTENTS\n");
            }
            for(int i = 0; i < branch_table.size(); i++){
                printf("%d     %d\n", i, branch_table[i]);
            }
            return;
        }


};