# ViraTrace - HE

Implementation of [ViraTrace](https://github.com/ViraTrace/InfectionModel) approach of distributed contact tracing 
and infection risk estimation using homomorphic encryption, implemented using [Microsoft SEAL](https://github.com/microsoft/SEAL). 

This approach enhances the privacy of the nodes in the network. 

## Contents

- [Setup](#setup)
- [ViraTrace Summary](#viratrace-summary)
- [Homomorphic Encryption](#homomorphic-encryption)
- [Advantages](#advantages)
- [Performance](#performance)

## Setup

Adapt the steps from the Dockerfile for your platform, or build the container and use the shell of a running instance.

## ViraTrace Summary

ViraTrace is a simulation based approach for ranking risk (i.e. probability of being infected) in a network. In the 
paper, it is demonstrated that this approach elegantly accounts for the degree of contact and network topology, 
while being free of the paradoxes of some of the alternative approaches. 

### State

1. A node's state is modelled as a vector of booleans. 
1. The count of values that are `true` is interpreted as the risk score.
1. The length of the vector is interpreted as the number of simulations being run in parallel. 
1. A node with a confirmed diagnosis has a risk score equal to the simulation size, while one who has no possibility of 
the disease has a risk score of zero. 

### Infectivity

1. The infectivity (i.e. probability of transmission) of a node is modelled as a vector of booleans. The values may be 
interpreted as iid draws from a latent Bernoulli variable. 
1. The function that gived success probability of the latent bernoulli variable sampled to get the infectivity vector is 
isomorphic to the one that gives the _true_ infectivity, but is transformed to give a usable distribution of risk scores for
a _reasonable_ simulation size.

### Transmission

1. A node's transmission likelihood is the logical conjunction of a its state and its infectivity.
1. A node's posterior risk score is the logical disjunction of its state and the transmission likelihood of the other node.

### Example

As an example, say Alice and Bob meet with their prior state vectors given by:

```
alice_prior = [0, 1, 1, 1, 0, 0, 0, 1, 1, 0]
bob_prior   = [1, 1, 0, 1, 0, 0, 0, 0, 0, 1]
```

The probabilty of Alice transmitting the infection to Bob is 0.3 (calculated using her current state, duration of contact, 
distance etc.). The infectivity and likelihood vectors for Alice, and the posterior risk score for Bob are given by:

```
alice_infectivity = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0] // can vary between runs
alice_likelihood  = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0]
bob_posterior     = [1, 1, 1, 1, 0, 0, 0, 0, 0, 1]
```

## Homomorphic Encryption

The private data held by each node is its state vector and the tranmission likelihood for the interaction. Considering 
Alice and Bob, there are two ways to propogate risk through the network:

1. Alice shares her state with Bob, who computes the disjunction with his transmission likelihood and returns the posterior risk score.
1. Bob shares his transmission likelihood with Alice, who then updates her state.

This project takes the first approach. Concretely,

1. Alice homomorphically encrypts her state and sends the cipher text to Bob.
1. Bob computes the logical disjunction on the encrypted ciphertext and returns the transformed ciphertext.
1. Alice decrypts the cipher text and sets her state to the posterior risk score. 

#### Implementation

1. The state vectors are mapped to integers, with `true` mapped to `0` and `false` mapped to `1`. With this mapping, 
logical disjunction is equivalent to multiplication. 
1. The state vector (of Alice) is encrypted using the BFV scheme, and multiplied (by Bob) with a plain text encoding 
of the transmission likelihood. 
1. Alice decrypts the ciphertext and inverse maps the integers to booleans. This is her posterior risk score. 

## Advantages

Without homomorphic encryption, privacy conscious implementations would take the second approach, i.e. share the transmission
likelihood using public key encryption. With the approach described above, only the marginal transmission likelihood, 
i.e. excess risk due to the interaction, is exchanged. The advantages are:

1. Privacy is increased without sacrificing efficacy. 
1. The lost privacy is concordant with the magnitude of the externality. 

To understand the implications of the second point, consider the case of a highly contagious risk Alice interacting with a healthy Bob. In this case, Alice poses a much higher risk to Bob than Bob poses to Alice. Alice's posterior state vector will be 
nearly the same as her prior (since it was mostly `true`), therefore the information she gains about Bob's infectivity is 
minimal. On the other hand, Bob's posterior will be significantly different from Alice's (since it was mostly `false`),
and he gains a lot of information about Alice's infectivity. Without homomorphic encryption, both would have learnt the 
same amount of information about the other. 

## Performance


