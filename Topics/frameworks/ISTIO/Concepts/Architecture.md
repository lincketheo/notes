# Architecture

- Control Plane:
    - Istiod - injects proxies to each pod
    - Use to be multiple components until v1.5
- Service to Service
    - Virtual Service
        - How to route to a given destination
    - Destination Rule
        - What happens to traffic for that destination