Sources:
    - https://istio.io/latest/docs/ops/deployment/deployment-models/

# Deployment Models
- Questions:
    - Will the mesh be confined to a single cluster or distributed across multiple clusters? 
    - Will all the services be located in a single fully connected network, or will gateways be required to connect services across multiple networks? 
    - Is there a single control plane, potentially shared across clusters, or are there multiple control planes deployed to ensure high availability (HA)? 
    - Are all clusters going to be connected into a single multicluster service mesh or will they be federated into a multi-mesh deployment?