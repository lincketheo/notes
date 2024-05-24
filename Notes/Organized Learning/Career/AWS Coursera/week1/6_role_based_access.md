
Don't __create a user for the EC2 instance to interact with AWS__. Instead, create a role

Signed
    - Verify your identity
    - Users have credentials
        - Access / secret keys
Authenticated

IAM Roles:
    - Users:   
        - Username + Password + Static
    - Roles:
        - No Username + Password 
        - credentials are programmatically acquired
        - Rotated
        - Many different Identities

External Identity Providers:
    - Example:  
        - External company lots of tech roles, don't need to create a bunch of different users
    - (Federated Users)