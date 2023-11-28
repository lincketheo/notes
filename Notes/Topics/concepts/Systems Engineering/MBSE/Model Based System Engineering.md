Sources:
    - https://insights.sei.cmu.edu/blog/introduction-model-based-systems-engineering-mbse/

# Model Based System Engineering 
- Formalize requirements, design, analysis, verification and validation of complex systems
- (Constrast with document centric engineering - documents to highlight the above)
- Mitigates _security risks_ earlier on in system development
        - Systems are _secure by design_ (rather than security as an afterthought)
            - [Threat Modeling](https://insights.sei.cmu.edu/blog/threat-modeling-12-available-methods/)
- Main
    1. Model
        - A simplified version of a complex system represented graphically, mathematically or physically
        - Models should sufficiently represent the system and the system should confirm the models
    2. Systems Thinking (https://medium.com/disruptive-design/tools-for-systems-thinkers-the-6-fundamental-concepts-of-systems-thinking-379cdac3dc6a)
        - Looking at a system as a part of a larger system
        - Explore:
            - Boundaries
            - Context
            - Lifecycle
            - Notes Behavior
            - Identify Pattners
        - Find things like:
            - Missing interactions
            - Duplication of effort
            - Missed opportunity for automation
    3. Systems Engineering
        - Techniques to ensure that all requirements are satisfied from a system

- Three systems involved in modeling process:
    1. Designed System
    2. Designed System's Context
    3. Modeling organization for the designed system

- Modeling Uses four instruments:
    1. Language
        - e.g. SysML, UML (SysML is similar but more formal)
    2. Structure
    3. Argumentation
        - Demonstrate in an easy way how the system must be built to be successful
    4. Presentation
        - Ensure comprehensibility

- Modeling Domains 
    1. Requirements/Capabilities
    2. Behavior
    3. Architecture/Structure
    4. Verification and Validation
    - See [NASA's Definitions](https://www.nasa.gov/reference/2-0-fundamentals-of-systems-engineering/)
        - Difference is MBSE uses models instead of documents

- Language of System Modeling
    1. Noun:
        - Actors, Blocks, Components, Requirements
    2. Verb:
        - Operational Activities, Functions, Use Cases
    3. Adjective:
        - Attributes
    4. Adverb:
        - Relationships, [needlines](https://docs.nomagic.com/display/UPDM2P190/Needline), exchanges, interfaces

- Four Quadrants
    - Problem (left), solution (right) (AKA Operational vs System points of view)
    - Operational POV
        - Views from:
            - Customers, users, operators, business people's views
        - Describes:
            - Business processes, objectives, organizational structure, use cases, information flows
    - System POV
        - Architecture of the system that solves the problem posed in the operational side of the model
        - Describes:
            - Behavior, Structure, Dataflows, allocation of functionality + Solution alternatives

[Four Quadrants](./assets/MBSE-FourQuadrants.webp "Four Quadrants")
            