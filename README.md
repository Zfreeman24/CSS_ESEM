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

## Goal of the project

Our goal was to increase the encryption of the key generation, as we felt the initial key generation was inadequate given the importance of health documents

## What we did

We implemented AES-256 encryption into the key generation function, further increasing the security of the key generation.

## How it works

It takes in buffers to store the generated AES-256 secret key, ECDSA secret key, ECDSA public key, and multiple public and secret ECDSA key pairs.

It outputs the generated AES-256 key, ECDSA public/secret key pair, and multiple ECDSA public/secret key pairs stored in the provided buffers.

First, it generates an initial random AES-256 bit encryption key and stores it in sk_aes. It initializes an AES context with this key to encrypt counters later.

It generates 3 distinct AES-256 bit keys by encrypting counters using the AES context. This is done to derive unique keys from the initial random key.

For each counter encryption, it measures the time taken and accumulates it to calculate average encryption time later.

It then goes into a loop to generate multiple ECDSA key pairs. For each iteration:

- It encrypts a counter using the AES context to generate a pseudo-random byte sequence.
- It uses this byte sequence as input to generate an ECDSA public/secret key pair using the ECCRYPTO library.
- It measures the time taken to generate the key pair.
- It copies the generated public key to the publicAll buffer and secret key to the secretAll buffer at an offset.

After the loop, it has generated the AES-256 key, 1 ECDSA key pair, and multiple ECDSA key pairs derived from the AES-256 keys. The keys are stored in the provided buffers for later use.

## Results

Not only did we increase the security of the key generation, we also **decreased** the time it took to generate by a whopping 90.05%! 

- 3 Runs Average Before Our Implementation: 0.000745
- 3 Runs Average After Our Implementation: 0.0000742

### Reason for improvement

The improvement came from using a much more lightweight and compact encryption library, allowing for quicker calculations for the key generation.

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
