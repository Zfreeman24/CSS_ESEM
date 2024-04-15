# ESEM

Energy-Aware Signature for Embedded Medical Devices (ESEM) is a lightweight signature scheme that minimizes the energy consumption of the signer, specifically designed for implantable medical devices where the energy consumption is of top priority.

## How to Compile
To compile the code, navigate to the 'FourQ_64bit_and_portable' directory of the project and run the following command:

```bash
make ARCH=x64
```

If you're having issues or the output files are not there try:

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

Not only did we increase the security of the key generation, we also increased the time it took to generate by a whopping 90.05%! 

3 Runs Average Before Our Implementation: 0.000745
3 Runs Average After Our Implementation: 0.0000742

## Licensing

This work is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
