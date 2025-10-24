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
    vector<uint32_t> b_branch_table, g_branch_table,chooser_table;
    uint32_t gbh_reg = 0, index_mask,bimodal_mask, index_bits, gbh_bits, chooser_size, bimodal_bits, predict_taken = 0, predict_not = 0, mispredictions = 0, predictions = 0;
    char* type;
    



    public:
        branch_predictor(char* style, uint32_t m, uint32_t n = 0, uint32_t k = 0, uint32_t x = 0){               //takes in arguments: index size = m, and type = n
            //First set tables and important variables
            index_bits = m;
            gbh_bits = n;
            chooser_size = k;
            bimodal_bits = x;
            type = style;
            index_mask = (1 << index_bits) - 1;

            if(gbh_bits < 1){               //if bimodal branch predictor
                bimodal_bits = index_bits;
                bimodal_mask = (1 << bimodal_bits) - 1;
               b_branch_table.resize(1 << bimodal_bits);        //branch table size = m
               for(uint32_t i = 0; i < b_branch_table.size(); i++){
                    b_branch_table[i] = 2;
                }
            }
            else if(strcmp(type,"gshare") == 0){
                g_branch_table.resize(1 << index_bits);        //branch table size = m
                for(uint32_t i = 0; i < g_branch_table.size(); i++){
                    g_branch_table[i] = 2;
                }
            }

            if(strcmp(type, "hybrid") == 0){
                //code for creating hybrid table
                bimodal_mask = (1 << bimodal_bits) - 1;


                b_branch_table.resize(1 << bimodal_bits);
                g_branch_table.resize(1 << index_bits);
                chooser_table.resize(1 << chooser_size);
                for(uint32_t i = 0; i < chooser_table.size(); i++){
                    chooser_table[i] = 1;
                }
                for(uint32_t i = 0; i < g_branch_table.size(); i++){
                    g_branch_table[i] = 2;
                }
                for(uint32_t i = 0; i < b_branch_table.size(); i++){
                    b_branch_table[i] = 2;
                }
            }

            //Set tables and important variables^^^^^^^^^
        }

        void predict(uint32_t shifted_PC, char result){
            predictions++;      //incremenent number of predictions made
            //code for bimodal prediction
            if(gbh_bits < 1){           
                uint32_t p = bimodal_predict(shifted_PC, result);
                if(p){
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
                update_bimodal(shifted_PC, result);
                return;
                //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            }
            //Code for Bimodal Prediction^^^^^^^^^^^^^^


            //Code for gshare prediction
            if(strcmp(type,"gshare") == 0){
                uint32_t p = gshare_predict(shifted_PC, result);
                if(p){
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
                update_gshare(shifted_PC, result);
                update_gbh(result);
                return;
            }
            //code for gshare prediction^^^^^^^^^^

            //Code for hybrid prediction
            else{
                uint32_t g_prediction,b_prediction, g_correct = 1, b_correct = 1;
                //find index for chooser table
                uint32_t chooser_mask = (1 << chooser_size) - 1;
                uint32_t index = shifted_PC & chooser_mask;
                //find index for chooser table^^^^^^^^

                //update and make predictions based on chooser table values
                
                //call both gshare and bimodal find
                g_prediction = gshare_predict(shifted_PC, result);
                b_prediction = bimodal_predict(shifted_PC, result);
                //check chooser table for which outcome to choose and update said table
                if(chooser_table[index] >= 2){
                    update_gshare(shifted_PC, result);
                    if(g_prediction){
                        predict_taken++;
                        if(result == 'n'){
                            mispredictions++;
                            g_correct = 0;
                        }
                    }
                    else{
                        predict_not++;
                        if(result == 't'){
                            mispredictions++;
                            g_correct = 0;
                        }
                    }
                }
                else{
                    update_bimodal(shifted_PC, result);
                    if(b_prediction){
                        predict_taken++;
                        if(result == 'n'){
                            mispredictions++;
                            b_correct = 0;
                        }
                    }
                    else{
                        predict_not++;
                        if(result == 't'){
                            mispredictions++;
                            b_correct = 0;
                        }
                    }
                }
                //update gbh_reg
                update_gbh(result);
                //update chooser
                if((g_correct && !b_correct) && (chooser_table[index] < 3)){
                    chooser_table[index]++;
                }
                else if((!g_correct && b_correct) && (chooser_table[index] > 0)){
                    chooser_table[index]--;
                }
                
                
  
                return;
            }

            //code for hybrid prediction^^^^^^^^^^^


        }

        uint32_t gshare_predict(uint32_t shifted_PC, char result){
            uint32_t prediction;
            //caclulate index
            uint32_t gbh_temp = gbh_reg << (index_bits - gbh_bits);       //allign gbh with upper n bits from m
            uint32_t index = gbh_temp ^ (shifted_PC & index_mask);
            //calculate index^^^^^^^^^^

            //make prediction & adjust variables
            if(g_branch_table[index] >= 2){
                prediction = 1;
            }
            else{      
                prediction = 0;
            }
            //make prediction & adjust variables^^^^^^^^^^^

         
            return prediction;
        }

        void update_gshare(uint32_t shifted_PC, char result){
            uint32_t gbh_temp = gbh_reg << (index_bits - gbh_bits);       //allign gbh with upper n bits from m
            uint32_t index = gbh_temp ^ (shifted_PC & index_mask);
            if((result == 't') && (g_branch_table[index] < 3)){
                g_branch_table[index]++;
            }
            else if((result == 'n') && (g_branch_table[index] > 0)){
                g_branch_table[index]--;
            }
            return;
        }
      
        uint32_t bimodal_predict(uint32_t shifted_PC, char result){
            uint32_t prediction;
            uint32_t index = shifted_PC & bimodal_mask;           //find index from PC using Mask

            //make prediction based on table
            if(b_branch_table[index] >= 2){
                prediction = 1;
            }
            else{
                prediction = 0;
            }

            return prediction;
            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        }
       
      void update_bimodal(uint32_t shifted_PC, char result){
        uint32_t prediction;
        uint32_t index = shifted_PC & bimodal_mask;           //find index from PC using Mask
        if((result == 't') && (b_branch_table[index] < 3)){
            b_branch_table[index]++;
        }
        else if((result == 'n') && (b_branch_table[index] > 0)){
            b_branch_table[index]--;
        }
        return;
      }
      
        void update_gbh(char result){
            gbh_reg = gbh_reg >> 1;
            if(result == 't'){
                uint32_t gbh_mask = (uint32_t)(1 << (gbh_bits - 1));
                gbh_reg = gbh_reg | gbh_mask;
            }
            return;
        }

        void print_contents(){
            printf("OUTPUT\n");
            printf("number of predictions:   %d\n", predictions);
            printf("number of mispredictions:  %d\n", mispredictions);
            printf("misprediction rate:        %.2f%%\n", 100*((double)mispredictions/(double)predictions));
            if(strcmp(type, "bimodal") == 0){
                printf("FINAL BIMODAL CONTENTS\n");
                for(uint32_t i = 0; i < b_branch_table.size(); i++){
                    printf("%d     %d\n", i, b_branch_table[i]);
                }
            }
            if(strcmp(type, "gshare") == 0){
                printf("FINAL GSHARE CONTENTS\n");
                for(uint32_t i = 0; i < g_branch_table.size(); i++){
                    printf("%d     %d\n", i, g_branch_table[i]);
                }
            }
            if (strcmp(type, "hybrid") == 0){
                printf("FINAL CHOOSER CONTENTS\n");
                for(uint32_t i = 0; i < chooser_table.size(); i++){
                    printf("%d     %d\n", i, chooser_table[i]);
                }
                printf("FINAL GSHARE CONTENTS\n");
                for(uint32_t i = 0; i < g_branch_table.size(); i++){
                    printf("%d     %d\n", i, g_branch_table[i]);
                }
                printf("FINAL BIMODAL CONTENTS\n");
                for(uint32_t i = 0; i < b_branch_table.size(); i++){
                    printf("%d     %d\n", i, b_branch_table[i]);
                }

            }
            return;
        }


};