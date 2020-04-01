//
//  main.cpp
//  AiSD_Project_2
//
//  Created by Kacper Kędzierski on 31/03/2020.
//  Copyright © 2020 Kacper Kędzierski. All rights reserved.
//

#include <iostream>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

// Debug stream
#ifdef _DEBUG
  #define _DEBUG_PRINTF(...) fprintf(stderr, __VA_ARGS__ );
#else
  #define _DEBUG_PRINTF(...)
#endif

using namespace std;

//  ===
//  Data structures
//  ===

typedef struct {
    int_fast32_t notches_count;
    int_fast32_t* notches;
    int_fast32_t* permutation;
    int_fast32_t* permutation_reverse;
} rotor_t;

typedef struct {
    int_fast32_t* permutation;
} reflector_t;

typedef struct {
    int_fast32_t rotors_count;
    rotor_t** rotors;
    int_fast32_t* rotors_positions;
    reflector_t* reflector;
} encryptor_t;


//  ===
//  Functions
//  ===

int_fast32_t* permutation_scan( int_fast32_t size ) {
    int_fast32_t* return_permutation = (int_fast32_t*)malloc( size * sizeof(int_fast32_t));
    
    for( int_fast32_t i = 0; i < size; i++ ) scanf(" %" SCNdFAST32, &return_permutation[i]);
    return return_permutation;
}

int_fast32_t* permutation_revert( int_fast32_t alphabet_size, int_fast32_t permutation[] ) {
    int_fast32_t* permutation_reverted = (int_fast32_t*)malloc( alphabet_size * sizeof(int_fast32_t) );
    
    int_fast32_t current_index = alphabet_size - 1;
    for( int_fast32_t i = 0; i < alphabet_size; i++ ) permutation_reverted[i] = permutation[current_index--];

    return permutation_reverted;
}

rotor_t* rotor_scan( int_fast32_t size ) {
    rotor_t* return_rotor = (rotor_t*)malloc(sizeof(rotor_t));
    
    // Scan permuation - call permutation_scan function
    return_rotor->permutation = permutation_scan(size);
    
    // Scan notches
    scanf("%" SCNdFAST32, &return_rotor->notches_count);
    if( return_rotor->notches_count > 0 )
        return_rotor->notches = (int_fast32_t*)malloc( return_rotor->notches_count * sizeof(int_fast32_t));
    
    for( int_fast32_t i = 0; i < return_rotor->notches_count; i++ ) {
        scanf(" %" SCNdFAST32, &return_rotor->notches[i]);
        return_rotor->notches[i]--;
    }
    
    // Revert permutation for backward movement
    return_rotor->permutation_reverse = permutation_revert( size, return_rotor->permutation );
    return return_rotor;
}

reflector_t* reflector_scan( int_fast32_t size ) {
    reflector_t* return_reflector = (reflector_t*)malloc(sizeof(reflector_t));
    
    return_reflector->permutation = permutation_scan(size);
    return return_reflector;
}

void encryptor_free( encryptor_t* encryptor ) {
    free(encryptor->rotors);
    free(encryptor->rotors_positions);
    
    free(encryptor);
}

//  ===
//  Main
//  ===

int main() {
    
    _DEBUG_PRINTF("Debug stream initialized\n");
    
    int_fast32_t alphabet_size;
    int_fast32_t rotors_count;
    int_fast32_t reflectors_count;
    int_fast32_t tasks_count;
    int_fast32_t rotor_index;
    int_fast32_t rotor_position;
    int_fast32_t reflector_index;
    
    scanf("%" SCNdFAST32, &alphabet_size);
    
    // Initialize rotors definitions
    scanf("%" SCNdFAST32, &rotors_count);
    rotor_t** rotors = (rotor_t**)malloc( rotors_count * sizeof(rotor_t) );
    for( int_fast32_t i = 0; i < rotors_count; i++ ) {
        rotors[i] = rotor_scan(alphabet_size);
    }
    
    // Initialize reflectors definitions
    scanf("%" SCNdFAST32, &reflectors_count);
    reflector_t** reflectors = (reflector_t**)malloc( reflectors_count * sizeof(reflector_t) );
    for( int_fast32_t i = 0; i < reflectors_count; i++ ) {
        reflectors[i] = reflector_scan(alphabet_size);
    }
    
    scanf("%" SCNdFAST32, &tasks_count);
    for( int_fast32_t i = 0; i < tasks_count; i++ ) {
        
        // Initialize encryptor
        encryptor_t* enigma = (encryptor_t*)malloc(sizeof(encryptor_t));
        
        scanf("%" SCNdFAST32, &enigma->rotors_count);
        enigma->rotors = (rotor_t**)malloc( enigma->rotors_count * sizeof(rotor_t) );
        enigma->rotors_positions = (int_fast32_t*)malloc( enigma->rotors_count * sizeof(int_fast32_t) );
        
        // Scan encryptor rotors
        for( int_fast32_t j = 0; j < enigma->rotors_count; j++ ) {
            scanf("%" SCNdFAST32, &rotor_index);
            scanf("%" SCNdFAST32, &rotor_position);
            
            enigma->rotors[j] = rotors[rotor_index];
            enigma->rotors_positions[j] = --rotor_position;
        }
        
        // Scan encryptor reflector
        scanf("%" SCNdFAST32, &reflector_index);
        enigma->reflector = reflectors[reflector_index];
        
        // Free encryptor
        encryptor_free(enigma);
        
    }
    
    return 0;
    
}
