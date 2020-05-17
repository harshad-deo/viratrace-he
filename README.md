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

ViraTrace is a simulation based approach for ranking risk (i.e. probability of being infected) in a network. Concretely,

1. A node's state is modelled as a vector of booleans. The count of values that are true is interpreted as the risk 
score. The length of the vector is interpreted as the number of simulations being run in parallel. 
1. The infectivity (i.e. probability of transmission) of a node is modelled as a vector of booleans. The values may be 
interpreted as iid draws from a latent Bernoulli variable. 
1. A node's likelihood of transmitting the disease is the logical conjunction of a node's state and its infectivity.
1. A node's posterior risk score is the logical disjunction of its state and the infectivity of the other node.

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

The function that gived success probability of the latent bernoulli variable sampled to get the infectivity vector is 
isomorphic to the one that gives the _true_ infectivity, but is scaled to give a usable distribution of risk scores for
a _reasonable_ simulation size. In the paper, it is demonstrated that this approach elegantly accounts for the degree
of contact and network topology, while being free of the paradoxes of some of the alternative approaches. 

## Homomorphic Encryption

## Advantages

## Performance


