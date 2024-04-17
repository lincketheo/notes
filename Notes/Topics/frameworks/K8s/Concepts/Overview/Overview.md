Sources:
    - https://kubernetes.io/docs/concepts/overview/

# Overview
- History
    - Google open sourced K8s in 2014

## Evolution
![Evolution](../assets/Container_Evolution.svg "Evolution")
- Virtualization
    - Virtualization allows you to run multiple VMs on single physical CPU
    - Applications can be isolated between VMs and provide security that one app can't be accessed by another application
- Container
    - Relaxed isolation properties to share the OS among applications
    - Lightweight 
    - Owns:
        - Filesystem, share of CPU, memory, process space etc.
    - Benefits over VMs:
        - Easier to make than VMs
        - Easier to do CI CD 
        - Dev and Ops seperation of concerns
            - Decoupling applications from infrastructure
        - Observability
            - OS Level information and health and other signals
        - Environmental consistency 
        - Cloud and OS portability (Ubuntu, RHEL, CoreOS.. etc)
        - Application centric management
            - Raises level of abstraction from running an OS on virtual hardware
        - Loosely coupled, distributed, elastic, librated micro services
        - No monolithic stacks 
        - Resource isolation
        - Resource utilization: high efficiency and density

## Kubernetes
What if a container goes down, how do you handle this meta aspect of programming?
Provides:
- Service discovery and load balancing
    - Expose containers using DNS name / IP Address
- Storage orchestration
    - Mount storage system of your choice (cloud, local etc.)
- Automated rollouts and rollbacks
- Automatic bin packing
    - Provide kubernetes with cluster of nodes that it can use to run containerized tasks
    - Tell how much CPU / memory
- Self healing
    - containers that fail:
        - replace, kill or restart
- Secret and configuration management
    - Deploy / update secrets and application configuration without rebuilding container images
- Batch execution
    - Manage batch and CI workloads, replacing containers that fail
- Horizontal scaling
    - Scale using a UI or based on CPU usage
- IPv4/IPv6 stack
    - Allocation of addresses to pods and services
- Designed for extensibility
    - Add features without changing upstream code

## NOT:
- Limit types of applications supported
- Doesn't deploy source code
- Doesn't provide application level services:
    - middleware (message buses)
    - data processing frameworks (e.g. spark) 
    - databases (e.g. mysql)
    - caches
    - cluster storage systems (e.g. ceph)
- Doesn't dictate logging / monitoring / alerting
- Does not provide nor mandate configuration language / system
- Does not adopt machine configuration 
- Not just an orchestration system
    - **Orchestration System**:
        - Defined workflow:
            - Do A, then B then C
    - Instead: independent composable control processes that continuously drive the state towards the provided desired state
