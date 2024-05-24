```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deployment
  labels:
    app: nginx
spec:
  replicas: 1
  selector:
    matchLabels: # Searches through all pods. Each one with this label is part of this deployment
      app: mongo
  template: # Specify each pod
    metadata:
      labels: # Label to search for in the deployment
        app: mongo
    spec: # Pod spec
      containers:
      - name: mongodb
        image: mongo:5.0
        ports:
        - containerPort: 27017
```