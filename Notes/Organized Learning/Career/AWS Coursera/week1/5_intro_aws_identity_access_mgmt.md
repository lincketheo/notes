
Access Control and Credential Management

Application Level
    - Require username + password

## Example Generic
```
{
"Version": "2012-10-17",    
     "Statement": [{        
          "Effect": "Allow",        
          "Action": "*",        
          "Resource": "*"     
     }]
}
```

## Example Granular
```
{"Version": "2012-10-17",    
     "Statement": [{        
          "Effect": "Allow",        
          "Action": [            
               "iam: ChangePassword",            
               "iam: GetUser"            
               ]        
          "Resource": "arn:aws:iam::123456789012:user/${aws:username}"    
     }]
}
```

## Fields
    - "Effect": Specifies whether the statement results in an allow or an explicit deny
    - "Action": Describes the specific actions that will be allowed or denied
    - "Resource" Specifies the object or objects that the statement covers

Attached to either Groups or Users

## Best Practice:
    - Create groups 
        - Pros:
            - Changing job roles
    - After root user, create admin users
        - Can't add roles to root user
