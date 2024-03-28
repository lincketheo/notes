Sources:
    - https://aws.amazon.com/what-is/service-oriented-architecture/

# Service Oriented Architecture
- Uses software components called services to create business applications
- Each service provides business capability

## Benefits
- Faster Time to Market
    - Reuse services across different processes
- Efficient Maintenance
    - Easier to create, update and debug small services than large code blocks of monolithic applications
- Greater Adaptability
    - More adabtable to advancements in technology by hotswapping small services

## Basic Principles
- Interoperability
    - Each service includes description that specify functionality of the service
    - Any client system can run a service, regardless of platform / programming language
        - C# and Python services the same
        - Changes in one service (shouldn't) affect other components using the service
- Loose Coupling
    - Services have as few dependencies as possible on external resources
        - e.g. data models, information systems
    - Services should be stateless without retaining any information from past sessions / transactions
- Abstraction
    - Clients use SOA don't need to know the service's code logic / implementation details
    - Services appear like a black box
- Granularity
    - Services have appropriate size and scope
    - Ideally: One business function per service
    - Can combine multiple services to create one complex operation

## Components
- Service
    - Basic Building Block
    - Private or Public
    - Features:
        1. Service Implementation
            - Code that builds the logic for performing specific service function
            - Example: User authentication, bill calculation
        2. Service Contract
            - Defines nature of the service and its associated terms and conditions
            - Example: Prereqs for using service, cost, quality of service provided
        3. Service Interface
            - Other services communicate through service interface
            - Defines how you can invoke the service to perform activities or exchange data
- Service Provider
    - Creates, Maintains, Provides one or more services that others can use
- Service Consumer
    - Requests the service provider to run a specific service
- Service Registry
    - Network Accessible directory of available services
    - Stores:
        - Service description
        - documents from service providers

## How does Service Oriented Architecture Work
- Services function independently
- Consumer requests information and sends input data to the service
- Service Processes data performs task, send back response
- Communication Protocols:
    - Simple Object Access Protocol (SOAP)
    - RESTful HTTP
    - Apache Thrift
    - Apache ActiveMQ
    - Java Message Service (JMS)

## ESB (Enterprise Service Bus)
- Software used when communicating with a system that has multiple services
- Establish communication between services and service consumers no matter the technology

## Limitations
- Limited Scalability
    - Services share many resources and need to coordinate to perform their functionality
- Increasing Interdependencies
    - Become more complex over time, harder to modify / debug if lots of services are calling each other in a loop
    - Shared resources (databases) slow down the system
- Single Point of Failure
    - ESB creates single source of failure

## Microservices
- Very small independent components that specialize on one task only
- Communicate through APIs
- VS SOA
    - Evolution of SOA
    - Address shortcommings of SOA
    - **Favor Data Duplication over Data Sharing**
        - Completely independent with their own communication protocols that are exposed through lightweight APIs

# SysML
    - Syntax of MBSE
# 