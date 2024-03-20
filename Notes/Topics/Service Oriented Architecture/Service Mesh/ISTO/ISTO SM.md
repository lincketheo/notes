# ISTO Service Mesh
- Features:
    - Secure Service to Service communication in a cluster with TLS encryption
    - Automatic load balancing for HTTP gRPC WebScoket and TCP traffic
    - Fine Grained control of traffic behavior with routing rules, retries, failovers and fault injection
    - Pluggable policy layer and configuration API supporting access controls, rate limits and quotas
    - Automatic metrics, logs, and traces for all traffic within a cluster - cluster ingress and egress
- Control plane runs on kubernetes

## How it works
- Data Plane
    - Communication between services
    - Proxy to intercept all network traffic
- Control Plane
    - Takes configuration and view of services and dynamically programs proxy servers

## Concepts
- Traffic Management
    - Routing traffic within a cluster and across clusters
- Observability
    - Generate telementry for all communication within a service mesh
    - Metrics, Logs, Traces
- Security Capabilities
    - Protection against man in the middle, flexible access controls, auditing tools, mutual TLS

# Questions
- SM vs Kubernetes