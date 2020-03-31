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


//  ===
//  Functions
//  ===

int_fast32_t* permutation_scan( int_fast32_t size ) {
    // To avoid code duplication
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

//  ===
//  Main
//  ===

int main() {
    
    _DEBUG_PRINTF("Debug stream initialized\n");
    
    int_fast32_t alphabet_size;
    int_fast32_t rotors_count;
    int_fast32_t reflectors_count;
    
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
    
    
    return 0;
    
}
