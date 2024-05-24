# Docker Compose
- `compose.yaml`

```yaml
name: myapp
services:
    foo:
        image: busybox
        command: echo "I'm running ${COMPOSE_PROJECT_NAME}"
        #... https://docs.docker.com/compose/compose-file/05-services/
    
```

```bash
$ docker compose up # In the directory that compose.yaml is in
```