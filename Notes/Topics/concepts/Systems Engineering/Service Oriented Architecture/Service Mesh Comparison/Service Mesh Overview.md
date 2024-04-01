Sources:
    - https://buoyant.io/service-mesh-manifesto

# What is a Service Mesh?
- A bunch of user space proxies stuck next to each service
    - Data Plane
    - Intercept calls between services and does stuff with each call
    - Level 7 aware tcp proxies (like haproxy and NGINX) - choice varies - **Note each proxy choice in tech stacks**
        - API Gateway / ingress proxy: Calls from the outside world
        - Service Mesh proxy - Calls within the system
- A set of management processes
    - Control Plane
    - Coordinates behavior of the proxies
    - Provides API for the user
    - Set of components that provide whatever machinery the data plane needs to act in a coordinated fashion
    - Examples:
        - Service discovery
        - TLS certificate issuing
        - metric aggregation

# Why?
- Reliability features
- Observability features
- Security features


# Things to note:
1. What proxy it uses (Envoy common) (speed ranking?)
2. List of control plane services
3. Proxies per instance

