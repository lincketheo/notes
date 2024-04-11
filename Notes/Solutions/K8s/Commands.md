# Interactions
```bash
$ kubectl get # - list resources <services> | <pods> | <nodes> ...
$ kubectl describe # - show detailed information about a resource
$ kubectl logs # - print the logs from a container in a pod
$ kubectl exec # - execute a command on a container in a pod
```

# Proxy:
Each node is an isolated network, proxy to create proxy to that isolated network:
```bash
$ kubectl proxy
```

# Get Data:
Get a list of names of pods. Useful for storing as variable for api call:
```bash
$ kubectl get pods -o go-template --template '{{range .items}}{{.metadata.name}}{{"\n"}}{{end}}'
```

Get port number of a service:
```bash
$ kubectl get services/kubernetes-bootcamp -o go-template='{{(index .spec.ports 0).nodePort}}'
```

# Execute Command in pod's container:
```bash
kubectl exec <pod name> -- <command>
```

Example (interactive bash shell)
```bash
$ kubectl exec -it <pod name> -- bash
```

# Get things associated other things

## Get pods (or services etc.) associated with a deployment
Get the name of the deployment:
```bash
$ kubectl get deployments
NAME ...
<deployment> ....
```

```bash
$ kubectl get pods -l app=<deployment>
```

## Get pods associated with a version
Set the version of a pod
```bash
$ kubectl label pods <pod name> version=v1
```

Get all pods with version:
```bash
$ kubectl get pods -l version=v1
```

# Delete a service
```
$ kubectl delete service -l app=<deployment name>
```