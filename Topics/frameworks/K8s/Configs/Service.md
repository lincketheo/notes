```yaml
apiVersion: v1
kind: Service
metadata:
  name: mongo-service
spec:
  selector:
    app: mongo # Matches the tag of the pod in the deployment
  ports:
    - protocol: TCP
      port: 8080 # The port to expose
      targetPort: 27017 # The internal port to be exposed (inside the pod)
```