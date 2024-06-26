/***********************************************************************************
* FourQlib: a high-performance crypto library based on the elliptic curve FourQ
*
*    Copyright (c) Microsoft Corporation. All rights reserved.
*
* Abstract: testing code for cryptographic functions based on FourQ 
************************************************************************************/   

#include "../FourQ_api.h"
#include "../FourQ_params.h"
 
#include "test_extras.h"
#include <stdio.h>
#include "aes.h"
#include "aes256.h"
#include "blake2.h"
#include "zmq.h"
#include <stdlib.h>

#define HIGH_SPEED 1

#define CMD_REQUEST_VERIFICATION         0x000010

// Benchmark and test parameters 

//For easy testing, no random keys are used in this implementation. secret_key, public_key should be generated new every time.

#if defined(HIGH_SPEED) // This is ESEMv2
    #define BENCH_LOOPS       100000      // Number of iterations per bench
    #define BPV_V             40
    #define ESEM_L            3
    #define BPV_N             128
#else 
    #define BENCH_LOOPS       100000
    #define BPV_V             18
    #define ESEM_L            3
    #define BPV_N             1024
#endif
 
void menu(){
    printf("NOTE: Currently, our implementation only has the communication between the verifier and the server \n");
    printf("NOTE: Therefore, Key Generation, Signer and Verifier should be run on same terminal. \n");        
    // printf("NOTE: Before running the server, key generation should be run once. \n");        
    printf("Select one of the following: \n");
    printf("(1) Key Generation\n");
    printf("(2) Signer\n");
    printf("(3) Server\n");
    printf("(4) Verifier\n");
    printf("(5) Exit\n\n\n");

}

/**
 * Prints the given byte array in hexadecimal format.
 *
 * @param data The byte array to print.
 * @param length The length of the byte array.
 */
void print_hex(const unsigned char *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02X", data[i]); // Prints each byte in hex format
    }
    printf("\n");
}

// Helper function to generate public key from secret key
ECCRYPTO_STATUS generateKeys(aes256_context_t *ctx, unsigned char *prf_out2, unsigned char *publicTemp, uint64_t iteration, unsigned char *publicAll, unsigned char *secretAll, double *total_time)
{
    aes256_blk_t prf_out;
    clock_t start, end;

    uint8_t counter[16] = {0};
    counter[15] = (uint8_t)(iteration + 1);

    // Encrypt counter using provided AES context
    start = clock();
    aes256_encrypt_ecb(ctx, &prf_out);
    end = clock();

    // Measure encryption time
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    *total_time += cpu_time_used; // Accumulate the time for each iteration

    // Print encryption time
    //printf("Encryption time for iteration %llu: %f seconds\n", iteration + 1, cpu_time_used);

    // Copy encrypted counter to buffer
    memcpy(prf_out2, prf_out.raw, 32);
    modulo_order((digit_t *)prf_out2, (digit_t *)prf_out2);

    // Generate public key
    start = clock();
    ECCRYPTO_STATUS status = PublicKeyGeneration(prf_out2, publicTemp);
    if (status != ECCRYPTO_SUCCESS)
    {
        return status;
    }
    end = clock();

    // Measure public key gen time
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    //printf("Public key generation time for iteration %llu: %f seconds\n", iteration + 1, cpu_time_used);

    // Copy keys to output buffers
    memcpy(publicAll + iteration * 64, publicTemp, 64);
    memcpy(secretAll + iteration * 32, prf_out2, 32);

    return ECCRYPTO_SUCCESS;
}

/**
 * Generates public and secret keys for the ESEM (Efficient Secure Enrollment Mechanism) protocol.
 *
 * This function performs the following steps:
 * 1. Generates AES-256 encryption keys using a counter-based approach.
 * 2. Generates a public/secret key pair using the ECCRYPTO library.
 * 3. Generates multiple public and secret key pairs using the generated AES-256 keys and stores them in the provided buffers.
 * 4. Calculates the average encryption time for the key generation process.
 *
 * @param sk_aes The buffer to store the AES-256 secret key.
 * @param secret_key The buffer to store the generated secret key.
 * @param public_key The buffer to store the generated public key.
 * @param publicAll_1 The buffer to store the first set of generated public keys.
 * @param publicAll_2 The buffer to store the second set of generated public keys.
 * @param publicAll_3 The buffer to store the third set of generated public keys.
 * @param secretAll_1 The buffer to store the first set of generated secret keys.
 * @param secretAll_2 The buffer to store the second set of generated secret keys.
 * @param secretAll_3 The buffer to store the third set of generated secret keys.
 * @param tempKey1 The buffer to store the first temporary AES-256 key.
 * @param tempKey2 The buffer to store the second temporary AES-256 key.
 * @param tempKey3 The buffer to store the third temporary AES-256 key.
 * @return ECCRYPTO_STATUS The status of the key generation process.
 */
ECCRYPTO_STATUS ESEM_KeyGen(unsigned char *sk_aes, unsigned char *secret_key, unsigned char *public_key, unsigned char *publicAll_1, unsigned char *publicAll_2, unsigned char *publicAll_3, unsigned char *secretAll_1, unsigned char *secretAll_2, unsigned char *secretAll_3, unsigned char *tempKey1, unsigned char *tempKey2, unsigned char *tempKey3)
{
    ECCRYPTO_STATUS Status = ECCRYPTO_SUCCESS;
    clock_t start, end;

    unsigned char publicTemp[64];
    aes256_key_t aes_key;

    // Initialize AES context with generated key 
    aes256_context_t ctx;

    // Generate 3 distinct AES keys using counters 
    // These will be used to generate multiple key pairs
    aes256_blk_t ctr1, ctr2, ctr3;

    // Measure time to encrypt counters
    start = clock();
    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Encryption time for counters: %f seconds\n", cpu_time_used);

    memcpy(tempKey1, ctr1.raw, 32);
    memcpy(tempKey2, ctr2.raw, 32);
    memcpy(tempKey3, ctr3.raw, 32);

    aes256_done(&ctx);

    // Generate initial public/private key pair
    start = clock();
    if (Status != ECCRYPTO_SUCCESS)
    {
        return Status;
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Key pair generation time: %f seconds\n", cpu_time_used);
    printf("Initial secret_key: ");
    print_hex(secret_key, 32);
    printf("Initial public_key: ");  
    print_hex(public_key, 64);

    // Initialize AES contexts with generated keys
    aes256_context_t ctx1, ctx2, ctx3;
    aes256_init(&ctx1, (aes256_key_t *)tempKey1);
    aes256_init(&ctx2, (aes256_key_t *)tempKey2);
    aes256_init(&ctx3, (aes256_key_t *)tempKey3);

    unsigned char prf_out2[32];
    double total_time_ctx1 = 0.0, total_time_ctx2 = 0.0, total_time_ctx3 = 0.0;

    // Generate multiple key pairs using each context
    for (uint64_t i = 0; i < BPV_N; i++)
    {
        // Generate initial AES-256 key
        if (!RAND_bytes(sk_aes, 32)) {
            return ECCRYPTO_ERROR;
        }

        // Initialize AES context 
        aes256_init(&ctx, &aes_key);

        // Encrypt counter
        aes256_encrypt_ecb(&ctx, &ctr1);

        // Derive secret key
        memcpy(secret_key, ctr1.raw, 32);

        // Generate public key 
        Status = PublicKeyGeneration(secret_key, public_key);

        // Call helper function for each iteration and each context
        Status = generateKeys(&ctx1, prf_out2, publicTemp, i, publicAll_1, secretAll_1, &total_time_ctx1);
        if (Status != ECCRYPTO_SUCCESS)
        {
            return Status;
        }
    }
    printf("secret_key after first round of AES encryption: ");
    print_hex(secret_key, sizeof(secret_key));
    printf("public_key after first round of AES encryption: ");  
    print_hex(public_key, sizeof(public_key));

    for (uint64_t i = 0; i < BPV_N; i++)
    {
        // Generate AES key
        if (!RAND_bytes(sk_aes, 32)) {
            return ECCRYPTO_ERROR;
        }  

        // Initialize AES context 
        aes256_init(&ctx, &aes_key);

        // Encrypt counter
        aes256_encrypt_ecb(&ctx, &ctr2);

        // Derive secret key
        memcpy(secret_key, ctr2.raw, 32);  

        // Generate public key 
        Status = PublicKeyGeneration(secret_key, public_key);

        // Call helper function for each iteration and each context
        Status = generateKeys(&ctx2, prf_out2, publicTemp, i, publicAll_2, secretAll_2, &total_time_ctx2);
        if (Status != ECCRYPTO_SUCCESS)
        {
            return Status;
        }
    }
    printf("secret_key after second round of AES-256 encryption: ");
    print_hex(secret_key, sizeof(secret_key));
    printf("public_key after second round of AES-256 encryption: ");  
    print_hex(public_key, sizeof(public_key));

    for (uint64_t i = 0; i < BPV_N; i++)
    {
        // Generate AES key
        if (!RAND_bytes(sk_aes, 32)) {
            return ECCRYPTO_ERROR;
        }  

        // Initialize AES context 
        aes256_init(&ctx, &aes_key);

        // Encrypt counter
        aes256_encrypt_ecb(&ctx, &ctr3);

        // Derive secret key
        memcpy(secret_key, ctr3.raw, 32);

        // Generate public key 
        Status = PublicKeyGeneration(secret_key, public_key);

        // Call helper function for each iteration and each context
        Status = generateKeys(&ctx3, prf_out2, publicTemp, i, publicAll_3, secretAll_3, &total_time_ctx3);
        if (Status != ECCRYPTO_SUCCESS)
        {
            return Status;
        }
    }

    aes256_done(&ctx1);
    aes256_done(&ctx2);
    aes256_done(&ctx3);

    // Calculate average time over all iterations
    double average_time_ctx1 = total_time_ctx1 / BPV_N;
    double average_time_ctx2 = total_time_ctx2 / BPV_N;
    double average_time_ctx3 = total_time_ctx3 / BPV_N;

    // Calculate average time over all contexts
    double total_average_time = average_time_ctx1 + average_time_ctx2 + average_time_ctx3;
    total_average_time /= 3;

    printf("Final secret_key: ");
    print_hex(secret_key, 32);
    printf("Final public_key: ");  
    print_hex(public_key, 64);

    printf("Average encryption time for ctx1: %f seconds\n", average_time_ctx1);
    printf("Average encryption time for ctx2: %f seconds\n", average_time_ctx2);
    printf("Average encryption time for ctx3: %f seconds\n", average_time_ctx3);
    printf("Total average encryption time: %f seconds\n", total_average_time);
    printf("sk-aes: ");
    print_hex(sk_aes, 32);

    return ECCRYPTO_SUCCESS;
}

ECCRYPTO_STATUS ESEM_Sign(unsigned char sk_aes[32], unsigned char secret_key[32], unsigned char *message, unsigned char *signature){

    // ECCRYPTO_STATUS Status = ECCRYPTO_SUCCESS;

    block* prf_out;
    unsigned char *prf_out2;
    prf_out = malloc(16*2);
    prf_out2 = malloc(16*2);
    uint64_t i, index, index2;

    unsigned char randValue[16] = {0}; //This is x in the scheme
    unsigned char counter[8] = {0};
    unsigned char hashOutput[36] = {0};

    unsigned char secretTemp[32];
    unsigned char secretTemp2[32];
    unsigned char lastSecret[32];
    digit_t* r = (digit_t*)(lastSecret);
    digit_t* S = (digit_t*)(signature+16);  
    digit_t* Secret = (digit_t*)(secretTemp2);  

    unsigned char tempKey1[32];
    unsigned char tempKey2[32];
    unsigned char tempKey3[32];

    blake2b(randValue, counter, secret_key, 16,8,32);
    // print_hex(randValue, 16);

    memcpy(signature, randValue,  16);

    block key;
    key = toBlock((uint8_t*)sk_aes);
    setKey(key);

    index = 1;
    ecbEncCounterMode(index,2,prf_out);
    memmove(tempKey1,prf_out,32);

    key = toBlock((uint8_t*)tempKey1);
    setKey(key);

    blake2b(hashOutput, randValue, tempKey1, 36, 16, 32);

    index2 = hashOutput[0] + ((hashOutput[1]/64) * 256);

    ecbEncCounterMode(index2,2,prf_out);
    memmove(secretTemp,prf_out,32);

    modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);

    index2 = hashOutput[2] + ((hashOutput[3]/64) * 256);

    ecbEncCounterMode(index2,2,prf_out);
    memmove(secretTemp2,prf_out,32);

    modulo_order((digit_t*)secretTemp2, (digit_t*)secretTemp2);
    add_mod_order((digit_t*)secretTemp, (digit_t*)secretTemp2, r);

    for (i = 2; i < BPV_V; ++i) { 
        index2 = hashOutput[2*i] + ((hashOutput[2*i+1]/64) * 256);
      
        ecbEncCounterMode(index2,2,prf_out);
        memmove(secretTemp,prf_out,32);

        modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);
        add_mod_order((digit_t*)secretTemp, r, r); // Add the r_i's and compute the final r
    }

    key = toBlock((uint8_t*)sk_aes);
    setKey(key);

    index = 2;
    ecbEncCounterMode(index,2,prf_out);
    memmove(tempKey2,prf_out,32);

    key = toBlock((uint8_t*)tempKey2);
    setKey(key);

    blake2b(hashOutput, randValue, tempKey2, 36, 16, 32);

    for (i = 0; i < BPV_V; ++i) { 
        index2 = hashOutput[2*i] + ((hashOutput[2*i+1]/64) * 256);
      
        ecbEncCounterMode(index2,2,prf_out);
        memmove(secretTemp,prf_out,32);

        modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);
        add_mod_order((digit_t*)secretTemp, r, r); // Add the r_i's and compute the final r
    }


    key = toBlock((uint8_t*)sk_aes);
    setKey(key);

    index = 3;
    ecbEncCounterMode(index,2,prf_out);
    memmove(tempKey3,prf_out,32);

    key = toBlock((uint8_t*)tempKey3);
    setKey(key);

    blake2b(hashOutput, randValue, tempKey3, 36, 16, 32);

    for (i = 0; i < BPV_V; ++i) { 
        index2 = hashOutput[2*i] + ((hashOutput[2*i+1]/64) * 256);
      
        ecbEncCounterMode(index2,2,prf_out);
        memmove(secretTemp,prf_out,32);

        modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);
        add_mod_order((digit_t*)secretTemp, r, r); // Add the r_i's and compute the final r -- Last r is calculated here (if l = 3)
    }

    unsigned char hashedMsg[32] = {0}; 
    blake2b(hashedMsg, message, randValue, 32, 32, 16);

    modulo_order((digit_t*)hashedMsg, (digit_t*)hashedMsg);

    to_Montgomery((digit_t*)hashedMsg, S);
    to_Montgomery((digit_t*)secret_key, Secret);
    Montgomery_multiply_mod_order(S, Secret, S);
    from_Montgomery(S, S);
    subtract_mod_order(r, S, S);



    free(prf_out);
    free(prf_out2); 

    return ECCRYPTO_SUCCESS;

}

ECCRYPTO_STATUS ESEM_Sign_v2(unsigned char secret_key[32], unsigned char *message, unsigned char *secretAll_1, unsigned char *secretAll_2, unsigned char *secretAll_3, unsigned char tempKey1[32], unsigned char tempKey2[32], unsigned char tempKey3[32], unsigned char *signature){

    uint64_t i;

    unsigned char randValue[16] = {0}; //This is x in the scheme
    unsigned char counter[8] = {0};
    unsigned char hashOutput[40] = {0};

    unsigned char secretTemp[32];
    unsigned char secretTemp2[32];
    unsigned char lastSecret[32];
    digit_t* r = (digit_t*)(lastSecret);
    digit_t* S = (digit_t*)(signature+16);  
    digit_t* Secret = (digit_t*)(secretTemp2);  


    blake2b(randValue, counter, secret_key, 16,8,32);

    memcpy(signature, randValue,  16);

    blake2b(hashOutput, randValue, tempKey1, 40, 16, 32);

    hashOutput[0] = hashOutput[0]/2;
    memmove(secretTemp, secretAll_1 + hashOutput[0]*32, 32);
    // modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);

    hashOutput[1] = hashOutput[1]/2;
    memmove(secretTemp2, secretAll_1 + hashOutput[1]*32, 32);
    // modulo_order((digit_t*)secretTemp2, (digit_t*)secretTemp2);
    add_mod_order((digit_t*)secretTemp, (digit_t*)secretTemp2, r);

    for (i = 2; i < BPV_V; ++i) { 
        hashOutput[i] = hashOutput[i]/2;
        memmove(secretTemp,secretAll_1 + hashOutput[i]*32, 32);

        // modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);
        add_mod_order((digit_t*)secretTemp, r, r); // Add the r_i's and compute the final r
    }


    blake2b(hashOutput, randValue, tempKey2, 40, 16, 32);

    for (i = 0; i < BPV_V; ++i) { 
        hashOutput[i] = hashOutput[i]/2;
        memmove(secretTemp,secretAll_2 + hashOutput[i]*32, 32);

        // modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);
        add_mod_order((digit_t*)secretTemp, r, r); // Add the r_i's and compute the final r
    }


    blake2b(hashOutput, randValue, tempKey3, 40, 16, 32);
    
    for (i = 0; i < BPV_V; ++i) { 
        hashOutput[i] = hashOutput[i]/2;
        memmove(secretTemp,secretAll_3 + hashOutput[i]*32, 32);

        // modulo_order((digit_t*)secretTemp, (digit_t*)secretTemp);
        add_mod_order((digit_t*)secretTemp, r, r); // Add the r_i's and compute the final r
    }


    unsigned char hashedMsg[32] = {0}; 
    blake2b(hashedMsg, message, randValue, 32, 32, 16);

    modulo_order((digit_t*)hashedMsg, (digit_t*)hashedMsg);

    to_Montgomery((digit_t*)hashedMsg, S);
    to_Montgomery((digit_t*)secret_key, Secret);
    Montgomery_multiply_mod_order(S, Secret, S);
    from_Montgomery(S, S);
    subtract_mod_order(r, S, S);



    return ECCRYPTO_SUCCESS;

}


ECCRYPTO_STATUS ESEM_Server(unsigned char *publicAll_1, unsigned char *publicAll_2, unsigned char *publicAll_3, unsigned char tempKey1[32], unsigned char tempKey2[32], unsigned char tempKey3[32]){

    ECCRYPTO_STATUS Status = ECCRYPTO_SUCCESS;

    unsigned char randValue[16];
    unsigned char hashOutput[36] = {0};
    uint64_t i, index2;
    unsigned char lastPublic1[64];
    unsigned char lastPublic2[64];
    unsigned char lastPublic3[64];
    unsigned char publicTemp[64];   
    point_extproj_t TempExtproj1, TempExtproj2, TempExtproj3;
    point_extproj_precomp_t TempExtprojPre1, TempExtprojPre2, TempExtprojPre3;
    point_extproj_t RVerify1, RVerify2, RVerify3;

    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");

    zmq_recv (responder, randValue, 16, 0);
    print_hex(randValue, 16);


    blake2b(hashOutput, randValue, tempKey1, 36, 16, 32);

    index2 = hashOutput[0] + ((hashOutput[1]/64) * 256);
    
    memmove(publicTemp,publicAll_1 +64*index2, 64);
    point_setup((point_affine*)publicTemp, RVerify1);

    index2 = hashOutput[2] + ((hashOutput[3]/64) * 256);

    memmove(publicTemp,publicAll_1 +64*index2, 64);
    point_setup((point_affine*)publicTemp, TempExtproj1);

    R1_to_R2(TempExtproj1, TempExtprojPre1);
    eccadd(TempExtprojPre1, RVerify1);


    for (i = 2; i < BPV_V; ++i) { // Same as above happens in the loop
        index2 = hashOutput[2*i] + ((hashOutput[2*i+1]/64) * 256);

        memmove(publicTemp,publicAll_1 +64*index2,64);
        point_setup((point_affine*)publicTemp, TempExtproj1);

        R1_to_R2(TempExtproj1, TempExtprojPre1);
        eccadd(TempExtprojPre1, RVerify1);   // Add the R[i]'s and compute the final R
    }

    eccnorm(RVerify1, (point_affine*)lastPublic1);

    zmq_send(responder, lastPublic1, 64, 0);

    zmq_recv (responder, randValue, 16, 0);
    print_hex(randValue, 16);



    blake2b(hashOutput, randValue, tempKey2, 36, 16, 32);

    index2 = hashOutput[0] + ((hashOutput[1]/64) * 256);
    
    memmove(publicTemp,publicAll_2 +64*index2, 64);
    point_setup((point_affine*)publicTemp, RVerify2);

    index2 = hashOutput[2] + ((hashOutput[3]/64) * 256);

    memmove(publicTemp,publicAll_2 +64*index2, 64);
    point_setup((point_affine*)publicTemp, TempExtproj2);

    R1_to_R2(TempExtproj2, TempExtprojPre2);
    eccadd(TempExtprojPre2, RVerify2);


    for (i = 2; i < BPV_V; ++i) { // Same as above happens in the loop
        index2 = hashOutput[2*i] + ((hashOutput[2*i+1]/64) * 256);

        memmove(publicTemp,publicAll_2 +64*index2,64);
        point_setup((point_affine*)publicTemp, TempExtproj2);

        R1_to_R2(TempExtproj2, TempExtprojPre2);
        eccadd(TempExtprojPre2,RVerify2);   // Add the R[i]'s and compute the final R
    }

    eccnorm(RVerify2, (point_affine*)lastPublic2);

    zmq_send(responder, lastPublic2, 64, 0);

    zmq_recv (responder, randValue, 16, 0);
    print_hex(randValue, 16);


    blake2b(hashOutput, randValue, tempKey3, 36, 16, 32);

    index2 = hashOutput[0] + ((hashOutput[1]/64) * 256);
    
    memmove(publicTemp,publicAll_3 +64*index2, 64);
    point_setup((point_affine*)publicTemp, RVerify3);

    index2 = hashOutput[2] + ((hashOutput[3]/64) * 256);

    memmove(publicTemp,publicAll_3 +64*index2, 64);
    point_setup((point_affine*)publicTemp, TempExtproj3);

    R1_to_R2(TempExtproj3, TempExtprojPre3);
    eccadd(TempExtprojPre3, RVerify3);


    for (i = 2; i < BPV_V; ++i) { // Same as above happens in the loop
        index2 = hashOutput[2*i] + ((hashOutput[2*i+1]/64) * 256);

        memmove(publicTemp,publicAll_3 +64*index2,64);
        point_setup((point_affine*)publicTemp, TempExtproj3);

        R1_to_R2(TempExtproj3, TempExtprojPre3);
        eccadd(TempExtprojPre3,RVerify3);   // Add the R[i]'s and compute the final R
    }

    eccnorm(RVerify3, (point_affine*)lastPublic3);

    zmq_send(responder, lastPublic3, 64, 0);


    return Status;

}


ECCRYPTO_STATUS ESEM_Server_v2(unsigned char *publicAll_1, unsigned char *publicAll_2, unsigned char *publicAll_3, unsigned char tempKey1[32], unsigned char tempKey2[32], unsigned char tempKey3[32]){

    ECCRYPTO_STATUS Status = ECCRYPTO_SUCCESS;

    unsigned char randValue[16];
    unsigned char hashOutput[40] = {0};
    uint64_t i, index2;
    unsigned char lastPublic1[64];
    unsigned char lastPublic2[64];
    unsigned char lastPublic3[64];
    unsigned char publicTemp[64];   
    point_extproj_t TempExtproj1, TempExtproj2, TempExtproj3;
    point_extproj_precomp_t TempExtprojPre1, TempExtprojPre2, TempExtprojPre3;
    point_extproj_t RVerify1, RVerify2, RVerify3;

    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");

    zmq_recv (responder, randValue, 16, 0);
    print_hex(randValue, 16);


    blake2b(hashOutput, randValue, tempKey1, 40, 16, 32);

    index2 = hashOutput[0]/2;
    
    memmove(publicTemp,publicAll_1 +64*index2, 64);
    point_setup((point_affine*)publicTemp, RVerify1);

    index2 = hashOutput[1]/2;

    memmove(publicTemp,publicAll_1 +64*index2, 64);
    point_setup((point_affine*)publicTemp, TempExtproj1);

    R1_to_R2(TempExtproj1, TempExtprojPre1);
    eccadd(TempExtprojPre1, RVerify1);


    for (i = 2; i < BPV_V; ++i) { // Same as above happens in the loop
        index2 = hashOutput[i]/2;

        memmove(publicTemp,publicAll_1 +64*index2,64);
        point_setup((point_affine*)publicTemp, TempExtproj1);

        R1_to_R2(TempExtproj1, TempExtprojPre1);
        eccadd(TempExtprojPre1, RVerify1);   // Add the R[i]'s and compute the final R
    }

    eccnorm(RVerify1, (point_affine*)lastPublic1);

    zmq_send(responder, lastPublic1, 64, 0);

    zmq_recv (responder, randValue, 16, 0);
    print_hex(randValue, 16);



    blake2b(hashOutput, randValue, tempKey2, 40, 16, 32);

    index2 = hashOutput[0]/2;
    
    memmove(publicTemp,publicAll_2 +64*index2, 64);
    point_setup((point_affine*)publicTemp, RVerify2);

    index2 = hashOutput[1]/2;

    memmove(publicTemp,publicAll_2 +64*index2, 64);
    point_setup((point_affine*)publicTemp, TempExtproj2);

    R1_to_R2(TempExtproj2, TempExtprojPre2);
    eccadd(TempExtprojPre2, RVerify2);


    for (i = 2; i < BPV_V; ++i) { // Same as above happens in the loop
        index2 = hashOutput[i]/2;

        memmove(publicTemp,publicAll_2 +64*index2,64);
        point_setup((point_affine*)publicTemp, TempExtproj2);

        R1_to_R2(TempExtproj2, TempExtprojPre2);
        eccadd(TempExtprojPre2,RVerify2);   // Add the R[i]'s and compute the final R
    }

    eccnorm(RVerify2, (point_affine*)lastPublic2);

    zmq_send(responder, lastPublic2, 64, 0);

    zmq_recv (responder, randValue, 16, 0);
    print_hex(randValue, 16);


    blake2b(hashOutput, randValue, tempKey3, 40, 16, 32);

    index2 = hashOutput[0]/2;
    
    memmove(publicTemp,publicAll_3 +64*index2, 64);
    point_setup((point_affine*)publicTemp, RVerify3);

    index2 = hashOutput[1]/2;

    memmove(publicTemp,publicAll_3 +64*index2, 64);
    point_setup((point_affine*)publicTemp, TempExtproj3);

    R1_to_R2(TempExtproj3, TempExtprojPre3);
    eccadd(TempExtprojPre3, RVerify3);


    for (i = 2; i < BPV_V; ++i) { // Same as above happens in the loop
        index2 = hashOutput[i]/2;

        memmove(publicTemp,publicAll_3 +64*index2,64);
        point_setup((point_affine*)publicTemp, TempExtproj3);

        R1_to_R2(TempExtproj3, TempExtprojPre3);
        eccadd(TempExtprojPre3,RVerify3);   // Add the R[i]'s and compute the final R
    }

    eccnorm(RVerify3, (point_affine*)lastPublic3);

    zmq_send(responder, lastPublic3, 64, 0);

    zmq_close (responder);
    zmq_ctx_destroy (context);

    return Status;

}


ECCRYPTO_STATUS ESEM_Verifier(unsigned char *signature,  unsigned char *message, unsigned char public_key[64]){

    ECCRYPTO_STATUS Status = ECCRYPTO_SUCCESS;

    unsigned char public_value1[64];
    unsigned char public_value2[64];
    unsigned char public_value3[64];
    unsigned char lastPublic[64];
    unsigned char lastPublic_Verify[64];


    point_extproj_t TempExtproj;
    point_extproj_precomp_t TempExtprojPre;
    point_extproj_t RVerify;


    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    zmq_send (requester, signature, 16, 0);
    zmq_recv (requester, public_value1, 64, 0);

    zmq_send (requester, signature, 16, 0);
    zmq_recv (requester, public_value2, 64, 0);

    zmq_send (requester, signature, 16, 0);
    zmq_recv (requester, public_value3, 64, 0);


    point_setup((point_affine*)public_value1, RVerify);
    point_setup((point_affine*)public_value2, TempExtproj);

    R1_to_R2(TempExtproj, TempExtprojPre);
    eccadd(TempExtprojPre, RVerify);

    point_setup((point_affine*)public_value3, TempExtproj);

    R1_to_R2(TempExtproj, TempExtprojPre);
    eccadd(TempExtprojPre, RVerify);   // Add the R[i]'s and compute the final R

    eccnorm(RVerify, (point_affine*)lastPublic);

    unsigned char hashedMsg[32] = {0}; 
    blake2b(hashedMsg, message, signature, 32, 32, 16);

    modulo_order((digit_t*)hashedMsg, (digit_t*)hashedMsg);


    ecc_mul_double((digit_t*)(signature+16), (point_affine*)public_key, (digit_t*)hashedMsg, (point_affine*)lastPublic_Verify);

    if(memcmp(lastPublic, lastPublic_Verify, 64) == 0)
        printf("Verified");
    else
        printf("Not Verified");

    zmq_close (requester);
    zmq_ctx_destroy (context);

    return Status;

}





int main()
{
    //AES Key
    unsigned char sk_aes[32] = {0x54, 0xa2, 0xf8, 0x03, 0x1d, 0x18, 0xac, 0x77, 0xd2, 0x53, 0x92, 0xf2, 0x80, 0xb4, 0xb1, 0x2f, 0xac, 0xf1, 0x29, 0x3f, 0x3a, 0xe6, 0x77, 0x7d, 0x74, 0x15, 0x67, 0x91, 0x99, 0x53, 0x69, 0xc5}; 

    //Schnorr Key
    unsigned char secret_key[32] =  {0x54, 0xa2, 0xf8, 0x03, 0x1d, 0x18, 0xac, 0x77, 0xd2, 0x53, 0x92, 0xf2, 0x80, 0xb4, 0xb1, 0x2f, 0xac, 0xf1, 0x29, 0x3f, 0x3a, 0xe6, 0x77, 0x7d, 0x74, 0x15, 0x67, 0x91, 0x99, 0x53, 0x69, 0xc5}; 
    unsigned char *publicAll_1, *publicAll_2, *publicAll_3, *secretAll_1, *secretAll_2, *secretAll_3, *message, *signature;
    unsigned char tempKey1[32], tempKey2[32], tempKey3[32], public_key[64]; //These are the keys to be shared with Parties.
    publicAll_1 = malloc(BPV_N*64);
    publicAll_2 = malloc(BPV_N*64);
    publicAll_3 = malloc(BPV_N*64);
    secretAll_1 = malloc(BPV_N*32);
    secretAll_2 = malloc(BPV_N*32);
    secretAll_3 = malloc(BPV_N*32);
    message = malloc(32);
    signature = malloc(48);
    memset(message, 0, 32);
    uint64_t benchLoop;
    benchLoop = 0;

    //  Benchmarking variables 
    double SignTime, VerifyTime;
    SignTime = 0.0;
    VerifyTime = 0.0;
    clock_t start, start2;
    clock_t end, end2; 
    // unsigned long long cycles, cycles1, cycles2;     
    // unsigned long long vcycles, vcycles1, vcycles2;

    ECCRYPTO_STATUS Status = ECCRYPTO_SUCCESS;
    int userType;

    modulo_order((digit_t*)secret_key, (digit_t*)secret_key);

    Status = ESEM_KeyGen(sk_aes, secret_key, public_key, publicAll_1, publicAll_2, publicAll_3, secretAll_1, secretAll_2, secretAll_3, tempKey1, tempKey2, tempKey3);
    if (Status != ECCRYPTO_SUCCESS) {
        printf("Problem Occurred in KeyGen");
    }

#if defined(HIGH_SPEED)
    printf("High Speed\n");
    for(benchLoop = 0; benchLoop <BENCH_LOOPS; benchLoop++){
        start = clock();
        Status = ESEM_Sign_v2(secret_key, message, secretAll_1, secretAll_2, secretAll_3, tempKey1, tempKey2, tempKey3, signature);
        end = clock();
        SignTime = SignTime +(double)(end-start);
    }
#else 
    for(benchLoop = 0; benchLoop <BENCH_LOOPS; benchLoop++){
        start = clock();
        Status = ESEM_Sign(sk_aes, secret_key, message, signature);
        end = clock();
        SignTime = SignTime +(double)(end-start);
    }
#endif
    if (Status != ECCRYPTO_SUCCESS) {
        printf("Problem Occurred in Sign");
    }

    printf("%fus per sign\n", ((double) (SignTime * 1000)) / CLOCKS_PER_SEC / BENCH_LOOPS * 1000);
    print_hex(signature, 48);


    printf("This is a proof-of-concept implementation!!! \n");


    while(1){
        menu();

        scanf ("%d",&userType);
        if(userType==1){
            printf("Key Generation\n");
            Status = ESEM_KeyGen(sk_aes, secret_key, public_key, publicAll_1, publicAll_2, publicAll_3, secretAll_1, secretAll_2, secretAll_3, tempKey1, tempKey2, tempKey3);
            if (Status != ECCRYPTO_SUCCESS) {
                printf("Problem Occurred in KeyGen");
            }
        }
        else if(userType==2){
            printf("Signer\n");
#if defined(HIGH_SPEED)
            printf("High Speed\n");
            // for(benchLoop = 0; benchLoop <BENCH_LOOPS; benchLoop++){
                // start = clock();
            Status = ESEM_Sign_v2(secret_key, message, secretAll_1, secretAll_2, secretAll_3, tempKey1, tempKey2, tempKey3, signature);
                // end = clock();
                // SignTime = SignTime +(double)(end-start);
            // }
#else 
            // for(benchLoop = 0; benchLoop <BENCH_LOOPS; benchLoop++){
                // start = clock();
            Status = ESEM_Sign(sk_aes, secret_key, message, signature);
                // end = clock();
                // SignTime = SignTime +(double)(end-start);
            // }
#endif
            if (Status != ECCRYPTO_SUCCESS) {
                printf("Problem Occurred in Sign");
            }

            // printf("%fus per sign\n", ((double) (SignTime * 1000)) / CLOCKS_PER_SEC / BENCH_LOOPS * 1000);
            print_hex(signature, 48);
            // SignTime = 0.0;
        }
        else if(userType==3){
            printf("Server\n");
#if defined(HIGH_SPEED)
            Status = ESEM_Server_v2(publicAll_1, publicAll_2, publicAll_3, tempKey1, tempKey2, tempKey3);
#else
            Status = ESEM_Server(publicAll_1, publicAll_2, publicAll_3, tempKey1, tempKey2, tempKey3);
#endif

            printf("Three (l) different servers are simulated in a single one, so three rounds of communication happens");
            if (Status != ECCRYPTO_SUCCESS) {
                printf("Problem Occurred in Sign");
            }
        }
        else if(userType==4){
            printf("Verifier\n");
            // memset(message, 1, 32);
            ESEM_Verifier(signature, message, public_key);
        }
        else if(userType==5){
            printf("Exiting\n");
            goto cleanup;
        }
        else
            goto cleanup;
    }
    
    


cleanup:


    
    
    free(publicAll_1);
    free(publicAll_2);
    free(publicAll_3);

    free(secretAll_1);
    free(secretAll_2);
    free(secretAll_3);
    free(message);
    return Status;
 
}
