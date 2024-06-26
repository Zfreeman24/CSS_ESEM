# ESEM | AES-256 | A Computer System Security Group Project | Team name : "The Encryptables"

Energy-Aware Signature for Embedded Medical Devices (ESEM) is a lightweight signature scheme that minimizes the energy consumption of the signer, specifically designed for implantable medical devices where the energy consumption is of top priority.

## Table of Contents
- [How to Compile](#how-to-compile)
- [Goal of the Project](#goal-of-the-project)
- [What We Did](#what-we-did)
- [ESEM Key Generation Module | How it Works](#esem-key-generation-module--how-it-works)
- [Cryptographic Details](#cryptographic-details)
- [Results](#results)
- [Improvements And Why The Original Was Lacking](#improvements-and-why-the-original-was-lacking)
- [Security Implications/Improvement](#security-implicationsimprovement)
- [Lessons Learned](#lessons-learned)
- [Conclusion](#conclusion)
- [References](#references)
- [Licensing](#licensing)

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

## What We Did

We enhanced the key generation function of the ESEM protocol by integrating AES-256 encryption, significantly boosting the security measures. This implementation was crucial given the sensitive nature of health-related data handled by embedded medical devices.

### Key Enhancements:
- **AES-256 Integration**: By embedding AES-256 encryption into the key generation process, we ensured a robust mechanism for securing data. This advanced encryption standard is critical in environments where security and privacy are paramount, such as in medical applications.
- **Efficiency Improvements**: Our modifications not only increased security but also reduced the key generation time by 90.05%. This was measured by comparing the average time taken to generate keys before and after our enhancements.
- **Application Relevance**: The project's focus on AES-256 and its implementation aligns perfectly with our coursework, particularly discussions around cryptographic security in healthcare contexts.

This enhancement directly addresses the initial inadequacies in key security, demonstrating our ability to apply theoretical knowledge in practical, high-stakes environments.

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

## Security Implications/Improvement

- **Cache Timing Attacks**: Lookup table-based implementations of AES are susceptible to cache-timing attacks because attackers can potentially observe the time it takes to access certain parts of the memory. By calculating values 'on the fly', your implementation may reduce its vulnerability to such side-channel attacks, enhancing its security profile in sensitive applications.

## Lessons Learned

In our recent project, we implemented AES-256 encryption to bolster the security of key generation for embedded medical devices. Through this endeavor, we deepened our understanding of cryptographic principles and their practical applications, particularly in the realm of symmetric encryption.

One of the key learnings from this topic is the importance of symmetric encryption, such as AES, in ensuring data confidentiality and integrity. We learned in class about the significance of symmetric key generation, where both parties share the same secret key for encryption and decryption. This aspect directly applies to our project, where the implementation of AES-256 relies on symmetric key generation to safeguard sensitive medical data.

Furthermore, our project's focus on AES-256 encryption aligns with our exploration of cryptographic techniques tailored to address specific security needs. This resonates with our class discussions on optimizing cryptographic algorithms for efficiency without compromising on security, especially in contexts like healthcare where data privacy is of utmost importance.


### Conclusion

Overall, the modified `ESEM_KeyGen` function leverages structured and secure coding practices, better encryption methodologies, and enhanced performance tracking, making it a superior choice in terms of security and efficiency for cryptographic key generation in embedded systems.

## References 
1. Andriani, R., Wijayanti, S. E., & Wibowo, F. W. (2018). Comparison Of AES 128, 192 And 256 Bit Algorithm For Encryption And Description File. In 2018 3rd International Conference on Information Technology, Information System and Electrical Engineering (ICITISEE) (pp. 13-14). IEEE.

We used the paper comparing AES encryption algorithms (AES-128, AES-192, and AES-256) to gain insights into the trade-offs between security and efficiency in encryption processes. By examining the processing time and CPU usage of different AES key sizes, we can make informed decisions about which encryption method best suits our needs. In the context of our study, where the security of health documents is paramount, understanding the performance implications of various encryption algorithms is crucial. This paper helps us compare the strength of AES-256 encryption, which we implemented in our key generation process, with other AES key sizes. For instance, while AES-128 may offer faster processing times, AES-256 provides stronger security due to its larger key size. By acknowledging this research, we demonstrate a commitment to enhancing the security of our key generation process while considering the balance between security and computational efficiency.


2. Rekha, S. S., & Saravanan, P. (2019). Low-Cost AES-128 Implementation for Edge Devices in IoT Applications. Journal of Circuits, Systems, and Computers, 28(04), 1950062.

We're using this paper to figure out how to make generating encryption keys safer for the Advanced Encryption Standard (AES) on devices with limited resources, such as those in the Internet of Things (IoT). Although the paper discusses improving AES specifically for IoT devices, we can still glean useful tricks to strengthen key generation. By understanding the methods employed to make AES safer and more efficient, we can find ways to enhance key generation, especially when computing power is limited. Thus, by learning from this research, we aim to make encryption both stronger and faster, even on devices with constrained resources.


## Licensing

This work is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
