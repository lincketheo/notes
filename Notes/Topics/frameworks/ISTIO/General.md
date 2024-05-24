# Configuration
- Uses CustomResourceDefinitions (CRD)
    - Custom components to allow configuring third party tech 
    - Used like any other k8s objects
- ISTIO CRD's (two main):
    - Virtual Service
        - How to route traffic to a specific service
    - Destination Rule
        - Policies on traffic (load balancing etc)
# Certificate Management
# ISTIO Ingress Gateway
- Entrypoint into k8s cluster
- Alternative to nginx ingress controller
- Pod - load balancer - accepts incomming traffic into cluster
