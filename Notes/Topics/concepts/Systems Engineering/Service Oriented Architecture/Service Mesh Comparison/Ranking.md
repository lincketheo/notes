# Primary Judgments:
1. System Describability 
    - How much of the state of the system can I READ during runtime and compile time
    - Bare minimum: 
        - Which nodes are connected / alive / open to connections at a moment
    - Bonus:
        - Can I read the entire state of the system in a single READ call 
        - Compare runtime with compile time
        - Without the need for UI Admin consoles or interactive platforms (e.g. is there an api provided?)
2. System Modifiability **
    - How much of the state of the system can I CREATE / UPDATE during runtime and compile time
    - Same as (1), but for modifying
3. Open Standards
    - A SM with a standard config file / description language would allow an application like ours flexibility to move around in the future
    - Pro: more people can use

- Sidecar offloads security 
- Registry:
    - Service Registry
    - Connection registry - route restrictions


## Requirements not as important (in no particular order)
1. Usability and developer experience
    - Idea:
        - Use NATs then zeromq as pub sub back end
2. Third party dependencies:
    - Envoy?
    - Containerization tooling:
        - Docker
        - Kubernetes
3. Ecosystem:
    - Must be kubernetes
    - AWS
    - EKS
    - Red Hat Open Shift
    - Consul
4. Application: 
    - Key differences in a sm platform that help a certain set of requirements:
    - e.g. 
        - Kuma - can partition multiple service meshes - useful for regulation and one source of control
        - Network Service Mesh - Useful for lower level telocommunications  
5. Runtime Observability:
    - How well can I monitor this solution during real time?