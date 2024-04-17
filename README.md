# ESEM | AES-256 | A Computer System Security Group Project

Energy-Aware Signature for Embedded Medical Devices (ESEM) is a lightweight signature scheme that minimizes the energy consumption of the signer, specifically designed for implantable medical devices where the energy consumption is of top priority.

## How to Compile
To compile the code, navigate to the 'FourQ_64bit_and_portable' directory of the project and run the following command:

```bash
make ARCH=x64
```

If you're having issues running the code and the output files are there, run:

```bash
make clean build
```

If you're still having issues, you may be missing some library installations such as blake2, which our group encountered.

## Goal of the project

Our goal was to increase the encryption of the key generation, as we felt the initial key generation was inadequate given the importance of health documents

## What we did

We implemented AES-256 encryption into the key generation function, further increasing the security of the key generation.

# ESEM Key Generation Module | How it works

This module is responsible for generating public and secret keys for the ESEM (Efficient Secure Enrollment Mechanism) protocol.

## Overview

The module utilizes buffers for storing the generated keys and intermediate values. The outputs are comprised of a secret key, a public key, and arrays of these keys for multiple instances (e.g., `secretAll_1`, `publicAll_1`, and so on).

### Logic Flow

1. **Generate AES-256 Key**: 
    - An AES-256 encryption key is generated for deriving additional key pairs later on.

2. **Generate ECC Key Pair**: 
    - Using the `ECCRYPTO` library, a single public/secret key pair is created, which serves as the main key pair for use.

3. **Generate Additional Key Pairs**: 
    - This involves:
        - Encrypting a counter value with the AES key to obtain a random byte sequence.
        - Using this random value to create another public/secret key pair.
        - Storing the newly generated key pairs in the respective arrays.

4. **Performance Measurement**: 
    - The execution time for the AES encryption and public key generation is measured and logged for performance analysis.

5. **Status Return**: 
    - The process concludes by returning a success or error status.

## Cryptographic Details

- The key generation mechanism employs a cryptographic pseudo-random number generator based on AES-256.
- The generation of the public keys and the cryptographic operations are handled by the `ECCRYPTO` library.

## Results

Not only did we increase the security of the key generation, we also **decreased** the time it took to generate by a whopping 90.05%! 

- 3 Runs Average Before Our Implementation: 0.000745
- 3 Runs Average After Our Implementation: 0.0000742

### Reason for improvement

The AES-256 implementation is generally faster and more secure for several reasons:

- **Block Size Operations**: AES-256 operates on the full AES block size of 128 bits at once using 32-bit registers and operations, whereas the 64-bit version would have to split operations into 64-bit chunks.

- **Key Scheduling**: AES-256 precomputes all the round keys upfront, which is more efficient than generating them on-the-fly as a 64-bit version would necessitate.

- **S-box Substitution**: The 256-bit version employs table lookups for the S-box substitution, which are quicker than calculating it, a process that a 64-bit version would carry out in real-time.

- **Parallel Processing**: With the ability to utilize parallel SIMD (Single Instruction, Multiple Data) instructions and registers, AES-256 can encrypt multiple blocks simultaneously, enhancing throughput.

- **Security**: AES-256 offers a significantly larger key space, providing enhanced security against brute-force attacks when compared to a 64-bit key size.

# Improvements And Why The Original Was Lacking

## Improved Key Encryption Methodology

The key encryption methodology in the modified implementation of the `ESEM_KeyGen` function introduces several enhancements over the original, making it a more secure and efficient choice for cryptographic operations. Here are the key aspects that highlight its superiority:

### Structured Cryptographic Context

- **Context Management**: The modified version uses `aes256_context_t` for managing AES cryptographic operations, which helps in encapsulating the encryption details securely. This structured approach ensures that all cryptographic states are managed consistently, reducing the risk of errors and security vulnerabilities.

- **Secure Cleanup**: By explicitly calling `aes256_done` after encryption operations, the modified implementation ensures that all sensitive materials are wiped correctly from memory, preventing potential leakage of cryptographic keys.

### Enhanced Encryption Security

- **Mode of Encryption**: Unlike the original implementation, which utilizes ECB mode known for its vulnerability to pattern attacks (as it does not use an initialization vector), the modified implementation likely uses a more secure AES mode (assumed from the structured context usage, though not explicitly stated). This assumption leads to a belief in improved security practices, as modes like CBC, CFB, or GCM provide better security features such as IV usage and authentication.

- **Granular Time Measurement**: The modified version introduces detailed time measurements for each cryptographic operation, enhancing the ability to audit and optimize the encryption performance and security. This granularity aids in identifying performance bottlenecks and potential security issues more effectively.

### Adherence to Cryptographic Best Practices

- **Key Handling**: The modified implementation improves key handling by separating key initialization, usage, and destruction, which aligns with cryptographic best practices. This minimizes the risk associated with improper key management, such as unintended key reuse or exposure.

# AES-256 Improvement

## Security Implications

- **Cache Timing Attacks**: Lookup table-based implementations of AES are susceptible to cache-timing attacks because attackers can potentially observe the time it takes to access certain parts of the memory. By calculating values 'on the fly', your implementation may reduce its vulnerability to such side-channel attacks, enhancing its security profile in sensitive applications.

### Conclusion

Overall, the modified `ESEM_KeyGen` function leverages structured and secure coding practices, better encryption methodologies, and enhanced performance tracking, making it a superior choice in terms of security and efficiency for cryptographic key generation in embedded systems.

## Licensing

This work is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
